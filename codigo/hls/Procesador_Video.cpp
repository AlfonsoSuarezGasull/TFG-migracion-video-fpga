#include "Procesador_Video.hpp"

// Fuente OSD: cada carácter es una matriz de 8x8 bits
const unsigned char font_rom[15][8] = {
    {0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // 0
    {0x08,0x18,0x08,0x08,0x08,0x08,0x1C,0x00}, // 1
    {0x3C,0x42,0x02,0x3C,0x40,0x40,0x7E,0x00}, // 2
    {0x3C,0x42,0x02,0x1C,0x02,0x42,0x3C,0x00}, // 3
    {0x04,0x0C,0x14,0x24,0x7E,0x04,0x04,0x00}, // 4
    {0x7E,0x40,0x7C,0x02,0x02,0x42,0x3C,0x00}, // 5
    {0x3C,0x40,0x7C,0x42,0x42,0x42,0x3C,0x00}, // 6
    {0x7E,0x02,0x04,0x08,0x10,0x10,0x10,0x00}, // 7
    {0x3C,0x42,0x42,0x3C,0x42,0x42,0x3C,0x00}, // 8
    {0x3C,0x42,0x42,0x3E,0x02,0x42,0x3C,0x00}, // 9
    {0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00}, // 10: .
    {0x42,0x66,0x5A,0x42,0x42,0x42,0x42,0x00}, // 11: M
    {0x7C,0x42,0x7C,0x42,0x42,0x7C,0x00,0x00}, // 12: B
    {0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00}, // 13: /
    {0x3C,0x40,0x3C,0x02,0x3C,0x00,0x00,0x00}  // 14: s
};

//Fuente Barras Fijas: Cada patrón dibujado
const unsigned int paleta_barras[8] = {
    0xFFFFFF, // 0: Blanco
    0xFFFF00, // 1: Amarillo
    0x00FFFF, // 2: Cian
    0x00FF00, // 3: Verde
    0xFF00FF, // 4: Magenta
    0xFF0000, // 5: Rojo
    0x0000FF, // 6: Azul
    0x000000  // 7: Negro
};

void Procesador_Video(pixel& input, pixel& output, int mode, int mbs_entero, int mbs_decimal, int camara_activa) {
    // Interfaces de vídeo AXI4-Stream (81,25 MHz)
    #pragma HLS INTERFACE axis port=input
    #pragma HLS INTERFACE axis port=output

    // Interfaz de control AXI4-Lite (50 MHz, reloj independiente)
    #pragma HLS INTERFACE s_axilite port=mode        bundle=CONTROL
    #pragma HLS INTERFACE s_axilite port=mbs_entero  bundle=CONTROL
    #pragma HLS INTERFACE s_axilite port=mbs_decimal bundle=CONTROL
    #pragma HLS INTERFACE s_axilite port=camara_activa bundle=CONTROL
    #pragma HLS INTERFACE s_axilite port=return      bundle=CONTROL clock=control_clk

    // Cadena OSD: solo se recalcula cuando cambia la métrica ---
    static ap_uint<4> cadena_osd[10];
    #pragma HLS ARRAY_PARTITION variable=cadena_osd complete
    static int mbs_entero_prev = -1;
    static int mbs_decimal_prev = -1;

    if (mbs_entero != mbs_entero_prev || mbs_decimal != mbs_decimal_prev) {
        cadena_osd[0] = (mbs_entero / 100) % 10; // Centena entero
        cadena_osd[1] = (mbs_entero / 10) % 10;  // Decena entero
        cadena_osd[2] = mbs_entero % 10;         // Unidad entero
        cadena_osd[3] = 10;                      // Índice del punto '.'
        cadena_osd[4] = (mbs_decimal / 10) % 10; // Décima
        cadena_osd[5] = mbs_decimal % 10;        // Centésima
        cadena_osd[6] = 11;                      // 'M'
        cadena_osd[7] = 12;                      // 'B'
        cadena_osd[8] = 13;                      // '/'
        cadena_osd[9] = 14;                      // 's'
        mbs_entero_prev = mbs_entero;
        mbs_decimal_prev = mbs_decimal;
    }

    // --- Estado persistente del patrón dinámico ---
    static int ball_x = 100;
    static int ball_y = 100;
    static int dir_x = 2;
    static int dir_y = 2;
    static unsigned char color_state = 0;
    static int contador_tiempo = 0;

    //  --- Cuenta de fotogramas para el parpadeo de la salvagurda ---
    static int parpadeo_salvaguarda = 0;
    static int salvaguarda_state = 0;

    // --- Constantes ---
    const int TICKS_POR_PASO = 1350000;  // ciclos para un paso a ~60 Hz
    const int ball_size = 20;            // Tamaño figura del patrón dinámico

    // Origen del OSD
    const int OSD_START_ROW = 20;
    const int OSD_START_COL = 20;
	
    //Fotogramas totales en un segundo en modo de generación. Se usa para gestionar parpadeo.
    const int fotogramas_totales = 266;
 

    AXI_VAL f_in;

    //--- Salvaguarda: En caso de modo de procesado sin encender la cámara, se entra en modo de salvaguarda ---
    int modo_efectivo = mode;
    if ((mode == 0 || mode == 1) && camara_activa==0) {
        modo_efectivo = 4;   //Modo de salvaguarda
    }

    // --- Resincronización: realinea el contador con el inicio de fotograma
    //     de la cámara al entrar/cambiar a un modo de procesado ---
    if (modo_efectivo == 0 || modo_efectivo == 1) {
        do {
            #pragma HLS LOOP_TRIPCOUNT min=0 max=307200 avg=1
            #pragma HLS PIPELINE II=1
            f_in = input.read(); //Busca inicio de fotograma.
        } while (f_in.user == 0);
    }

    // --- Bucle principal: un fotograma completo, un píxel por ciclo (II=1) ---
    for (int i = 0; i < ROWS; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=480 max=480 avg=480
        for (int j = 0; j < COLS; j++) {
            #pragma HLS LOOP_TRIPCOUNT min=640 max=640 avg=640
            #pragma HLS PIPELINE II=1

            AXI_VAL f_out;

            if (modo_efectivo == 0 || modo_efectivo == 1) {
                // Modos de procesado: leer el stream, salvo el primer píxel
                // (ya capturado por la resincronización)
                if (!(i == 0 && j == 0)) {
                    f_in = input.read();
                }

                if (modo_efectivo == 0) {            // Bypass
                    f_out = f_in;
                } else {                    // Escala de grises (entera con multiplicación)
                    unsigned char b = f_in.data.range(7, 0);
                    unsigned char g = f_in.data.range(15, 8);
                    unsigned char r = f_in.data.range(23, 16);
                    unsigned char gray = (r * 76 + g * 150 + b * 30) >> 8;
                    f_out.data.range(7, 0)   = gray;
                    f_out.data.range(15, 8)  = gray;
                    f_out.data.range(23, 16) = gray;

		    //Se conservan los metadatos de AXI4-Stream de la cámara
                    f_out.user = f_in.user;
                    f_out.last = f_in.last;
                    f_out.keep = f_in.keep;
                    f_out.strb = f_in.strb;
                }
            } else {
                // Modos de generación: metadatos propios, sin leer la entrada
                f_out.keep = 0xF;
                f_out.strb = 0xF;
                f_out.user = (i == 0 && j == 0) ? 1 : 0;
                f_out.last = (j == COLS - 1) ? 1 : 0;

                if (modo_efectivo == 2) {                       // Barras de color
                    int barra_idx;
                    if      (j <= 80)  barra_idx = 0;
                    else if (j <= 160) barra_idx = 1;
                    else if (j <= 240) barra_idx = 2;
                    else if (j <= 320) barra_idx = 3;
                    else if (j <= 400) barra_idx = 4;
                    else if (j <= 480) barra_idx = 5;
                    else if (j <= 560) barra_idx = 6;
                    else               barra_idx = 7;
                    f_out.data = paleta_barras[barra_idx];
                } else if (modo_efectivo == 3) {                // Figura en movimiento
                    f_out.data = 0x000000;
                    if (j >= ball_x && j < (ball_x + ball_size) &&
                        i >= ball_y && i < (ball_y + ball_size)) {
                        switch (color_state) {
                            case 0:  f_out.data = 0x0000FF; break;
                            case 1:  f_out.data = 0x00FF00; break;
                            case 2:  f_out.data = 0xFF0000; break;
                            case 3:  f_out.data = 0xFFFF00; break;
                            case 4:  f_out.data = 0x00FFFF; break;
                            case 5:  f_out.data = 0xFF00FF; break;
                            default: f_out.data = 0xFFFFFF; break;
                        }
                    }
                }
                else if (modo_efectivo == 4) {                 // Salvaguarda
                    f_out.data = 0x000000; // Fondo Negro predeterminado

		    //Constantes para representar triángulo
                    const int CUSPIDE_I = 119;
                    const int CUSPIDE_J = 319;
                    const int BASE_I = 359;

	            //Constantes para representar una exclamación dentro del triángulo
                    const int RECTANGULO_I = 179;
                    const int BASE_RECTANGULO_I = 279;
                    const int IZQUIERDA_J = 309;
                    const int DERECHA_J = 329;

		
                    if (i <= RECTANGULO_I || i >= BASE_RECTANGULO_I || j < IZQUIERDA_J || j > DERECHA_J) {
			//Se pinta donde no se encuentre el rectángulo de la exclamación
                        if (i >= CUSPIDE_I && i <= BASE_I) {
                            int dist_v = i - CUSPIDE_I;
                            int dist_h = j - CUSPIDE_J;
                            if (dist_h < 0) dist_h = -dist_h;

                            // Condición geométrica escalada para el triángulo equilátero
                            if (((dist_h << 6) <= dist_v * 37) && salvaguarda_state == 1) {

                            		f_out.data = 0xFFFF00; // Relleno Amarillo



                            }
                        }
                        // Dibujar el punto del signo de exclamación central
                        if (j >= IZQUIERDA_J && j <= DERECHA_J &&
                            i >= (BASE_RECTANGULO_I + 10) && i <= (BASE_RECTANGULO_I + 30)) {
                            f_out.data = 0x000000;
                        }
                    }
                }

                else {                               // Modo no contemplado
                    f_out.data = 0x000000;
                }
            }

            // --- Capa OSD, unificada sobre cualquier modo ---
            if (i >= OSD_START_ROW && i < (OSD_START_ROW + 8) &&
                j >= OSD_START_COL && j < (OSD_START_COL + 80)) {
                //Se divide por 8 para obtener el carácter a dibujar de la cadena
                int char_index = (j - OSD_START_COL) >> 3; 
                int font_id    = cadena_osd[char_index];
                //Fila del carácter que se tiene que representar
                int font_row   = i - OSD_START_ROW;
                
                int font_col   = 7 - ((j - OSD_START_COL) % 8);
                unsigned char font_line = font_rom[font_id][font_row];
                if ((font_line >> font_col) & 0x01) {
                    f_out.data = 0x00FF00;  // texto verde
                }
            }
            //Se escribe el píxel por la interfaz AXI4-Stream de salida, hacia el VDMA
            output.write(f_out);
        }
    }

    // --- Avance temporal del patrón dinámico, una vez por fotograma ---
    if (modo_efectivo == 3) {
        contador_tiempo += 307200;          // ciclos de un fotograma completo
        if (contador_tiempo >= TICKS_POR_PASO) {
            ball_x += dir_x;
            ball_y += dir_y;
            contador_tiempo -= TICKS_POR_PASO;
            if (ball_x <= 0 || ball_x >= (COLS - ball_size)) {
                dir_x = -dir_x;
                color_state = (color_state + 1) % 6;
            }
            if (ball_y <= 0 || ball_y >= (ROWS - ball_size)) {
                dir_y = -dir_y;
                color_state = (color_state + 1) % 6;
            }
        }
    } 
    //--- Parpadeo de la salvaguarda, cada 1 segundo (266 fotogramas) ---
    if(modo_efectivo == 4){
    	parpadeo_salvaguarda +=1;
    	if(parpadeo_salvaguarda>=fotogramas_totales){
    		parpadeo_salvaguarda -= fotogramas_totales;
    		salvaguarda_state = (salvaguarda_state + 1) % 2; //Conmutra entre estado de parpadeo (apagado) y
							 	 //No parpadeo (triangulo representado)
    	}

    }


}

