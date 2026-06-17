/*
 * video.c
 *	Trabajo de Fin de Grado: MIGRACIÓN DE UN CANAL DE VÍDEO EN TIEMPO REAL DESDE UNA FPGA SOC A UNA FPGA DISCRETA DE BAJO COSTE
 *  Created on: 14 may. 2026
 *      Author: Alfonso Suárez Gasull
 */



#include "xparameters.h"
#include "xiic_l.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xaxivdma.h"
#include "xil_types.h"
#include "xil_cache.h"
#include "xprocesador_video.h"
#include <ctype.h>
#include <stdlib.h>
#include "math.h"
#include "mb_interface.h"
#include "xaxipmon.h"
#include "xtmrctr.h"
#include "xuartlite_l.h"




/////////////////////////////////////////////////////////////
//GLOBAL #defineS
/////////////////////////////////////////////////////////////

// Dirección I2C del sensor MT9V111
#define CAM_ADDR        0x5C   // 0b1011100



// Dirección del dispositivo AXI IIC
#define IIC_BASEADDR    XPAR_AXI_IIC_0_BASEADDR


//Identificdor del AXI VDMA
#define DMA_DEVICE_ID XPAR_AXIVDMA_0_DEVICE_ID



//Definiciones para la configuración del AXI VDMA


#define VDMA_NOCIRC 0
#define VDMA_INT_GENLOCK 2



//Direcciones de la memoria DDR3, extraidas del Address Editor de Vivado

#define DDR_BASE_ADDR 0x80000000
#define DDR_HIGH_ADDR 0x8FFFFFFF

#define MEM_BASE_ADDR (DDR_BASE_ADDR + 0x01000000)
#define MEM_HIGH_ADDR DDR_HIGH_ADDR

//Reserva de espacio para los frame buffer
#define MEM_SPACE (MEM_HIGH_ADDR - MEM_BASE_ADDR)

//Direcciones de lectura y escritura del VDMA coincidentes
#define READ_ADDRESS_BASE MEM_BASE_ADDR
#define WRITE_ADDRESS_BASE MEM_BASE_ADDR

#define FRAME_HORIZONTAL_LEN 0x780 /* 640 pixels, cada pixel 3 bytes */
#define FRAME_VERTICAL_LEN 0x1E0 /* 480 pixels */



//Declaración de periféricos
XAxiVdma VDMA1;
XAxiPmon APM;
XTmrCtr TimerInstance;
XProcesador_video mi_procesador;
XProcesador_video_Config *procesador_cfg;
XGpio camrst;



// ------------------------------------------------------------
//  Inicio de periféricos hardware
// ------------------------------------------------------------


// ------------------------------------------------------------
//  Inicio de VDMA y asignación de frame buffers
// ------------------------------------------------------------

void vdma_init(XAxiVdma *VDMAPtr, int device_id){

	int Status;
	XAxiVdma_Config *VDMACfgPtr;

	VDMACfgPtr = XAxiVdma_LookupConfig(device_id); // las funciones son case-sensitive
	if (!VDMACfgPtr)
	{
		xil_printf("ERROR: No VDMA found for ID %d\r\n", device_id);
	}

	/* Initialize DMA engine */


	Status = XAxiVdma_CfgInitialize(VDMAPtr, VDMACfgPtr, VDMACfgPtr->BaseAddress);
	if(Status != XST_SUCCESS){
		xil_printf("ERROR: VDMA Configuration Initialization failed %d\r\n", Status);
	}
}
static int vdma_reset(XAxiVdma *VDMAPtr, int direction){

	int polls;

	xil_printf("Reiniciando Canal de %s  VDMA ... \r\n", (direction == XAXIVDMA_READ)?"Lectura":"Escritura");
	XAxiVdma_Reset(VDMAPtr, direction);
	polls = 100000;

	while (polls && XAxiVdma_ResetNotDone(VDMAPtr, direction)){
		polls -= 1;
	}

	if (!polls){
		xil_printf("ERROR: VDMA %s channel reset failed %x\n\r", (direction==XAXIVDMA_READ)?"Read":"Write", 0);

		return XST_FAILURE;
	}
	return 1;
}

static int vdma_setup(XAxiVdma *VDMAPtr,
	int direction,
	int frame_stores,
	int mode
	)
{

	int Status, i, Addr;
	int width, height;
	XAxiVdma_DmaSetup DmaSetup;


	if (direction == XAXIVDMA_WRITE){

		height = FRAME_VERTICAL_LEN; //SUBFRAME_VERTICAL_SIZE;
		width = FRAME_HORIZONTAL_LEN; //SUBFRAME_HORIZONTAL_SIZE;
		DmaSetup.EnableSync = 1;
	}
	else{

		height = FRAME_VERTICAL_LEN; //SUBFRAME_VERTICAL_SIZE;
		width = FRAME_HORIZONTAL_LEN; //SUBFRAME_HORIZONTAL_SIZE;

		DmaSetup.EnableSync = 1;
	}

	xil_printf("Estableciendo canal de %s del VDMA ... \r\n", (direction == XAXIVDMA_READ)?"Lectura":"Escritura");
	DmaSetup.VertSizeInput = height;
	DmaSetup.HoriSizeInput = width;
	DmaSetup.Stride = FRAME_HORIZONTAL_LEN; //Stride = zancada, paso, salto
	DmaSetup.FrameDelay = 0;
	DmaSetup.EnableCircularBuf = 1;

	DmaSetup.PointNum = 0;
	DmaSetup.EnableFrameCounter = 0; /*Endless Transfers */
	DmaSetup.FixedFrameStoreAddr = 0; // We are not doing parking

	if (VDMAPtr->MaxNumFrames > frame_stores){

		xil_printf("Estableciendo frame buffer para el canal de %s . \r\n", (direction==XAXIVDMA_READ)? "Lectura":"Escritura");
		Status = XAxiVdma_SetFrmStore(VDMAPtr, frame_stores, direction);
		if (Status != XST_SUCCESS){

			xil_printf("WARNING %d: VDMA - Setting Frame Store Number to %d Failed for %s Channel. Exiting config. \r\n", Status, frame_stores, (direction==XAXIVDMA_READ)? "Read":"Write");
			return XST_FAILURE;

		}

	}

	Status = XAxiVdma_DmaConfig(VDMAPtr, direction, &DmaSetup);
	if (Status != XST_SUCCESS){

		xil_printf("ERROR: VDMA -%s channel config failed. (%d)\r\n", (direction == XAXIVDMA_READ)?"Read":"Write", Status);
		return XST_FAILURE;
	}

		/* Ahora se inicializan las direcciones del buffer VDMA
		 * Estas direcciones son físicas
		 */

	if (direction == XAXIVDMA_WRITE){
		Addr = WRITE_ADDRESS_BASE + 0; // Parámetro de entrada
	}
	else{
		Addr = READ_ADDRESS_BASE + 0;
	}

	for(i=0; i < frame_stores; i++){
		xil_printf(" vdma_setup: Dirección %d = 0x%08x. \n\r", i, Addr);
		DmaSetup.FrameStoreStartAddr[i] = Addr;

		Addr += 640 * 480 *3;
		//Addr += 1080 * 1920 *3;
	}

	/* Ahora se establecen las direcciones del buffer para las transacciones en el dispositivo VDMA
	 * las direcciones son direcciones físicas
	 */

	 Status = XAxiVdma_DmaSetBufferAddr(VDMAPtr, direction, DmaSetup.FrameStoreStartAddr);

	 if (Status != XST_SUCCESS){

		xil_printf("ERROR: VDMA - %s channel set buffer address failed %d\r\n",(direction==XAXIVDMA_READ)?"Read":"Write", Status);
		return XST_FAILURE;
	}

	return 1;

}

static int vdma_enable(XAxiVdma *VDMAPtr, int direction){

	int Status;

	Status = XAxiVdma_DmaStart(&VDMA1, direction);
	if (Status != XST_SUCCESS){
		xil_printf("ERROR: VDMA Start %d transfer failed", direction);
		return XST_FAILURE;
	}
	else{
		xil_printf("VDMA - Iniciando canal de %s  ... \r\n",(direction==XAXIVDMA_READ)?"Lectura":"Escritura");
	}

	return 1;
}


// ------------------------------------------------------------
//   Rutina  para escribir en los registros de la cámara usando el protocolo I2C
// ------------------------------------------------------------

int I2CWriteData(u8 regAddr, u8 regData)
{
	unsigned Status;
	u8 SendBuffer[2];

	SendBuffer[0] = regAddr;
	SendBuffer[1] = regData;

	xil_printf("Enviando Registro: address=%02x val=%02x\r\n", SendBuffer[0], SendBuffer[1]);
	/*
	 * Send the Data.
	 */


	Status =XIic_Send(IIC_BASEADDR, CAM_ADDR, SendBuffer, 2, XIIC_STOP);
	if (Status != 2) {
		xil_printf("Error en la escritura de datos\r\n");
		return XST_FAILURE;

	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */

	Status = XIic_WaitBusFree(IIC_BASEADDR);

	if(Status == XST_FAILURE){
		xil_printf("Bus IIC no está liberado \r\n");
	}


	/*
	 * Wait for a bit of time to allow the programming to complete
	 */
	usleep(200000);

	return XST_SUCCESS;
}

// ------------------------------------------------------------
//   Rutina para la lectura de registros de la cámara usando el protocolo I2C
// ------------------------------------------------------------
 
u8 I2CReadData(u8 regAddr)
{
	int Status;
	u8 Addr, Val;

	Addr = regAddr;
	Val  = 0x00;

	//En primer lugar, se escribe el registro del que se quiere leer
	Status = XIic_Send(IIC_BASEADDR,CAM_ADDR, &Addr, 1 , XIIC_REPEATED_START);
	if (Status != 1) {
		xil_printf("Error en la escritura de dirección\r\n");
		return XST_FAILURE;

	}
	/*
	 * Receive the Data.
	 */
	//Ahora, se lee el valor del registro.
	Status = XIic_Recv(IIC_BASEADDR, CAM_ADDR, &Val, 1, XIIC_STOP);
	if (Status != 1) {
		xil_printf("Error en la lectura de datos\r\n");

		return XST_FAILURE;

	}



	/*
	 * Wait until bus is idle to start another transfer.
	 */
		while(XIic_CheckIsBusBusy(IIC_BASEADDR));

	xil_printf("Recibido Registro: address=%02x val=%02x\r\n", Addr, Val);

	/*
	 * Wait for a bit of time to allow the programming to complete
	 */
	usleep(2500);

	return Val;

}
// ------------------------------------------------------------
//  Rutina de reinicio de la cámara
// ------------------------------------------------------------

void reset_camara(){

    // Reset de la cámara
    xil_printf("Aplicando reset a la cámara...\r\n");
	XGpio_DiscreteWrite(&camrst, 1, 0);
	usleep(200000);
	XGpio_DiscreteWrite(&camrst, 1, 1);
	usleep(200000);


}

// ------------------------------------------------------------
//  Rutina de inicio del temporizador
// ------------------------------------------------------------
int init_timer() {
    int status;

    // Inicializar el AXI Timer por Hardware
    status = XTmrCtr_Initialize(&TimerInstance, XPAR_AXI_TIMER_0_DEVICE_ID);
    if (status != XST_SUCCESS) return XST_FAILURE;
    XTmrCtr_Reset(&TimerInstance, 0);



    XTmrCtr_SetResetValue(&TimerInstance, 0, 0);


    // Configurar el temporizador en modo libre (Free-running counter)
    XTmrCtr_SetOptions(&TimerInstance, 0, XTC_AUTO_RELOAD_OPTION);


    XTmrCtr_Start(&TimerInstance, 0);



    return XST_SUCCESS;
}

// ------------------------------------------------------------
//Inicia la cámara y manda la configuración necesaria para el funcionamiento deseado.
//En caso de inicio correcto, se notifica al procesador HLS que la cámara está funcionando.
// ------------------------------------------------------------


int I2CInit() {
	int Status;
	int error_count = 0;


		Status = I2CWriteData(0x07, 0x00);
		if(Status == XST_FAILURE){
			error_count++;
		}
		Status =I2CWriteData(0x7F, 0x01);
		if(Status == XST_FAILURE){
			error_count++;
		}

		Status =I2CWriteData(0x07, 0x00);
		if(Status == XST_FAILURE){
			error_count++;
		}
		Status =I2CWriteData(0x7F, 0x00);
		if(Status == XST_FAILURE){
			error_count++;
		}

		Status =I2CWriteData(0x08, 0xD8);
		if(Status == XST_FAILURE){
			error_count++;
		}
		Status =I2CWriteData(0x7F, 0x00);
		if(Status == XST_FAILURE){
			error_count++;
		}

		Status =I2CReadData(0x08);
		if(Status == XST_FAILURE){
			error_count++;
		}
		Status =I2CReadData(0x7F);
		if(Status == XST_FAILURE){
			error_count++;
		}

		return error_count;




}
// ------------------------------------------------------------
//  Inicio del AXI Performance Monitor
// ------------------------------------------------------------


int init_apm() {
    XAxiPmon_Config *ConfigPtr;
    ConfigPtr = XAxiPmon_LookupConfig(XPAR_AXI_PERF_MON_0_DEVICE_ID);
    if (ConfigPtr == NULL) return XST_FAILURE;

    XAxiPmon_CfgInitialize(&APM, ConfigPtr, ConfigPtr->BaseAddress);



    return XST_SUCCESS;
}
// ------------------------------------------------------------
//  Rutina para medir el rendimiento de la DDR3
// ------------------------------------------------------------


void medir_rendimiento(u32 bytes_escritura, u32 bytes_lectura, u8 imprimir_pantalla) {
    u32 bytes_totales;
    u32 ancho_banda_entero, ancho_banda_decimal;
    u32 ancho_banda_entero_lectura, ancho_banda_decimal_lectura;
    u32 ancho_banda_entero_escritura, ancho_banda_decimal_escritura;
    u32 uso_bus_entero, uso_bus_decimal;

    bytes_totales = bytes_escritura + bytes_lectura;

    // 1. Ancho de banda en MB/s (Multiplicar por 100 para extraer 2 decimales)
    // Formula: (Bytes / 1000 / 1000) * 100 -> Simplificado para evitar desborde
    u32 mb_x100 = (u32)(((u64)bytes_totales * 200) / ( 1000 * 1000));
    ancho_banda_entero  = mb_x100 / 100;
    ancho_banda_decimal = mb_x100 % 100;

    u32 mb_x100_lectura = (u32)(((u64)bytes_lectura * 200) / ( 1000 * 1000));
    ancho_banda_entero_lectura  = mb_x100_lectura / 100;
    ancho_banda_decimal_lectura = mb_x100_lectura % 100;

    u32 mb_x100_escritura = (u32)(((u64)bytes_escritura * 200) / ( 1000 * 1000));
    ancho_banda_entero_escritura  = mb_x100_escritura / 100;
    ancho_banda_decimal_escritura = mb_x100_escritura % 100;


    // 2. Uso del bus sobre 1300 MB/s
    // Formula: (AnchoBandaMB / 1300) * 100
    u32 uso_x100 = (mb_x100 * 100) / 1300;
    uso_bus_entero  = uso_x100 / 100;
    uso_bus_decimal = uso_x100 % 100;

    if(imprimir_pantalla== 1){
        xil_printf("--- Métrica de Rendimiento DDR3 ---\r\n");
        xil_printf("Tráfico detectado: %u bytes\r\n", (unsigned int)bytes_totales);
        xil_printf("Ancho de banda lectura: %u.%02u MB/s\r\n", (unsigned int)ancho_banda_entero_lectura, (unsigned int)ancho_banda_decimal_lectura);
        xil_printf("Ancho de banda escritura: %u.%02u MB/s\r\n", (unsigned int)ancho_banda_entero_escritura, (unsigned int)ancho_banda_decimal_escritura);
        xil_printf("Ancho de banda total: %u.%02u MB/s\r\n", (unsigned int)ancho_banda_entero, (unsigned int)ancho_banda_decimal);
        xil_printf("Uso del bus (sobre 1300 MB/s): %u.%02u%%\r\n", (unsigned int)uso_bus_entero, (unsigned int)uso_bus_decimal);
        xil_printf("-----------------------------------\r\n");


    }

    //Se manda la medida descompuesta en parte entera y decimal al procesador HLS
    XProcesador_video_Set_mbs_entero(&mi_procesador,ancho_banda_entero);
    XProcesador_video_Set_mbs_decimal(&mi_procesador,ancho_banda_decimal);
}



// ------------------------------------------------------------
//  MAIN
// ------------------------------------------------------------
int main(void)
{



    xil_printf("Inicializando...\r\n");
    xil_printf("\r\n--- Entrando main() --- \r\n");
    xil_printf("\r\n----------------------------------- \r\n");

    int status;
    // Inicializar GPIO para reset
	status = XGpio_Initialize(&camrst, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&camrst, 1, 0x00000000);

    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }
    else {
    	xil_printf("Bloque GPIO iniciado\r\n");
    	xil_printf("\r\n");

    }

	//Inicializar el procesador de vídeo diseñado
    procesador_cfg = XProcesador_video_LookupConfig(XPAR_PROCESADOR_VIDEO_0_DEVICE_ID);
    if (!procesador_cfg) {
        return XST_FAILURE;
    }
     status = XProcesador_video_CfgInitialize(&mi_procesador, procesador_cfg);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }
    else {
    	xil_printf("Bloque HLS iniciado\r\n");
    	xil_printf("\r\n");

    }
    XProcesador_video_Set_camara_activa(&mi_procesador, 0);//Camara desactivada por defecto. No hay modos de operacion.
    XProcesador_video_Start(&mi_procesador);
    XProcesador_video_EnableAutoRestart(&mi_procesador);

	sleep(1);

	//xil_printf("\x1B[H"); //Set cursor to top left of terminal
	//xil_printf("\x1B[2J"); //Clear terminal



	xil_printf("\r\n--- Inicializando canales ... \r\n");
	vdma_init(&VDMA1, DMA_DEVICE_ID); // La inicialización es para los dos planos

	xil_printf("\r\n Configurando canal de escritura ... \r\n");
	vdma_reset(&VDMA1, XAXIVDMA_WRITE);	//S2MM
	vdma_setup(&VDMA1, XAXIVDMA_WRITE, 3, VDMA_NOCIRC | VDMA_INT_GENLOCK); //S2MM
	vdma_enable(&VDMA1, XAXIVDMA_WRITE); //S2MM

	xil_printf("\r\n--- Configurando canal de lectura ... \r\n");
	vdma_reset(&VDMA1, XAXIVDMA_READ); //MM2S
	vdma_setup(&VDMA1, XAXIVDMA_READ, 3, VDMA_NOCIRC | VDMA_INT_GENLOCK); //MM2S
	vdma_enable(&VDMA1, XAXIVDMA_READ); //MM2S

	xil_printf("\r\n");

	sleep(1);











    if(init_apm() != XST_SUCCESS) {
        xil_printf("Error: No se pudo inicializar APM\r\n");
        return -1;
    }
    else {
    	xil_printf("Bloque APM iniciado\r\n");
    	xil_printf("\r\n");
    }
    if (init_timer() != XST_SUCCESS) {
        xil_printf("Error: No se pudo inicializar TIMER\r\n");
        return -1;
    }
    else {
    	xil_printf("Bloque Timer iniciado\r\n");
    	xil_printf("\r\n");

    }

     //Variables para guardar los ciclos del times, los bytes leidos y los bytes escritos
     u32 clk_actual;
     u32 bytes_escritura =0;
     u32 bytes_lectura = 0;
     //Gestiona el alternado lectura-escritura del APM
     u8 lectura = 0;

            //Arranca el APM
	    XAxiPmon_ResetMetricCounter(&APM);
	    XAxiPmon_SetMetrics(&APM, 0, XAPM_METRIC_SET_3, XAPM_METRIC_COUNTER_0); // Set 3: Bytes Leidos
	    XAxiPmon_StartCounters(&APM, 0xFFFF);

	    sleep(1);

	    xil_printf("--- Sistema de Procesado de Video Online ---\r\n");
	    xil_printf("Seleccione una opción:\r\n");
	    xil_printf("0: Modo de vídeo : Bypass\r\n");
	    xil_printf("1: Modo de vídeo : Escala de Grises\r\n");
	    xil_printf("2: Modo de vídeo : Test Pattern Barras Fijas\r\n");
	    xil_printf("3: Modo de vídeo : Test Pattern en movimiento\r\n");
	    xil_printf("4: Mostrar ancho de banda de DDR3L\r\n");
	    xil_printf("5: Mostrar tiempo de ejecución software de 4\r\n");
	    xil_printf("6: Iniciar la cámara MT9V111\r\n");
	    xil_printf("7: Reiniciar la cámara\r\n");
	    xil_printf("\r\n");

            char c;
	while(1)
	{
		//Verificación cíclica de la ventana temporal para la medida de ancho de banda
        clk_actual = XTmrCtr_GetValue(&TimerInstance, 0);


        if (clk_actual >= 25000000 && lectura == 1) {

        	    XAxiPmon_StopCounters(&APM);
        	    bytes_lectura = XAxiPmon_GetMetricCounter(&APM, XAPM_METRIC_COUNTER_0);


        	    XAxiPmon_ResetMetricCounter(&APM);

        	    XAxiPmon_SetMetrics(&APM, 0, XAPM_METRIC_SET_2, XAPM_METRIC_COUNTER_0);// Set 2: Bytes Escritos
        	    XAxiPmon_StartCounters(&APM, 0xFFFF);
        	    medir_rendimiento(bytes_escritura, bytes_lectura, 0); // Calcula y actualiza el OSD del HLS
        	    lectura = 0; //Cambio a escritura



        	}
        else if(clk_actual >= 50000000){
    	    XAxiPmon_StopCounters(&APM);
    	    bytes_escritura = XAxiPmon_GetMetricCounter(&APM, XAPM_METRIC_COUNTER_0);


    	    XAxiPmon_ResetMetricCounter(&APM);

    	    XAxiPmon_SetMetrics(&APM, 0, XAPM_METRIC_SET_3, XAPM_METRIC_COUNTER_0);// Set 3: Bytes Lectura
    	    XAxiPmon_StartCounters(&APM, 0xFFFF);
    	    medir_rendimiento(bytes_escritura, bytes_lectura, 0); // Calcula y actualiza el OSD del HLS
    	    XTmrCtr_Reset(&TimerInstance, 0);
    	    lectura = 1; //Cambio a lectura


        }






        // Función no bloqueante para comprobar si hay entradas por la UART
	       if (!XUartLite_IsReceiveEmpty(XPAR_AXI_UARTLITE_0_BASEADDR))
	        {

	            c = XUartLite_RecvByte(XPAR_AXI_UARTLITE_0_BASEADDR);

	            if (c == '0') { //Bypass

	            	XProcesador_video_Set_mode(&mi_procesador,0);
	            	xil_printf("Modo: Bypass activado\n\r");
	            	xil_printf("\r\n");
	            }
	            else if (c == '1') { //Escalado a Gris

	            	XProcesador_video_Set_mode(&mi_procesador,1);
	            	xil_printf("Modo: Escala de grises activado\n\r");
	            	xil_printf("\r\n");
	            }
	            else if (c == '2') { //Test Pattern  Barras

	            	XProcesador_video_Set_mode(&mi_procesador,2);
	            	xil_printf("Modo: Test Pattern fijo activado\n\r");
	            	xil_printf("\r\n");
	            }
	            else if(c=='3'){ //Test Pattern Dinámico

	            	XProcesador_video_Set_mode(&mi_procesador,3);
	            	xil_printf("Modo: Test Pattern en movimiento activado\n\r");
	            	xil_printf("\r\n");
	            }
	            else if(c=='4'){ // Medida Ancho de banda
	            	xil_printf("Mostrando rendimiento de la DDR3\n\r");
	            	xil_printf("\r\n");
	            	medir_rendimiento(bytes_escritura, bytes_lectura, 1);


	            }
	            else if(c=='5'){ // Medida tiempo de ejecución software de la rutina 4
 


	            	u32 ciclos_inicial = XTmrCtr_GetValue(&TimerInstance, 0);


	            	medir_rendimiento(bytes_escritura, bytes_lectura, 0);


	            	u32 ciclos_final = XTmrCtr_GetValue(&TimerInstance, 0);

	            	u32 ciclos_totales = ciclos_final - ciclos_inicial;
	                  u32 ms_entero  = ciclos_totales / 50000;
	                  u32 ms_decimal = (ciclos_totales % 50000) / 50;

	                     xil_printf("--- Tiempo de ejecución de MicroBlaze ---\r\n");
	                     xil_printf("Latencia en ciclos del temporizador (50 MHz) : %u\r\n", (unsigned int)ciclos_totales);
	                     xil_printf("Tiempo de ejecucion total: %u.%03u ms\r\n", (unsigned int)ms_entero, (unsigned int)ms_decimal);
	                     xil_printf("------------------------------------\r\n");



	            }

	            else if(c=='6'){ //Arranque de la cámara
	    			xil_printf("Arranque de la cámara\r\n");
	    			xil_printf("\r\n");
	    			reset_camara();
	    			int exito = 0;
	    			exito = I2CInit();
	    			usleep(200000);
	    			if(exito == 0) {
					//Inicia la cámara si no han habido errores escribiendo la configuración
	    				XProcesador_video_Set_camara_activa(&mi_procesador,1); 
	    				
	    			}



	            }

	            else if(c=='7'){ //Reinicio de la cámara
	    			xil_printf("Reinicio de la cámara\r\n");

	    			reset_camara();
	    			I2CWriteData(0x08, 0xD8);
	    			I2CWriteData(0x7F, 0x00);





	            }
	            else { //Carácter no contemplado
	            	xil_printf("Opcion no valida\n\r");
	                xil_printf("---------------------------------------------------------\r\n");
	                xil_printf("Seleccione una opción:\r\n");
	                xil_printf("0: Modo de vídeo : Bypass\r\n");
	                xil_printf("1: Modo de vídeo : Escala de Grises\r\n");
	                xil_printf("2: Modo de vídeo : Test Pattern Barras Fijas\r\n");
	                xil_printf("3: Modo de vídeo : Test Pattern en movimiento\r\n");
	                xil_printf("4: Mostrar ancho de banda de DDR3L\r\n");
	                xil_printf("5: Mostrar tiempo de ejecución software de 4\r\n");
	                xil_printf("6: Iniciar la cámara MT9V111\r\n");
	                xil_printf("7: Reiniciar la cámara\r\n");
	            }

	        }



	}
}
