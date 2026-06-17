/* Código del módulo de procesamiento de vídeo incluido en el TFG: Migración de un canal de vídeo en tiempo real desde una 
FPGA SoC a una FPGA discreta de bajo coste. Alfonso Suárez Gasull, 2026. */

#include "hls_video.h"
#include <ap_fixed.h>
#define ROWS 480
#define COLS 640

typedef ap_axiu<24,1,1,1> AXI_VAL;
typedef hls::stream<AXI_VAL> pixel;
void Procesador_Video(pixel& input, pixel& output, int mode, int mbs_entero, int mbs_decimal, int camara_activa);

