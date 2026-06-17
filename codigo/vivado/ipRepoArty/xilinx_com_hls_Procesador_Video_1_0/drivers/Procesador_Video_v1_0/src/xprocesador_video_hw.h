// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// CONTROL
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of mode
//        bit 31~0 - mode[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of mbs_entero
//        bit 31~0 - mbs_entero[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of mbs_decimal
//        bit 31~0 - mbs_decimal[31:0] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of camara_activa
//        bit 31~0 - camara_activa[31:0] (Read/Write)
// 0x2c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL            0x00
#define XPROCESADOR_VIDEO_CONTROL_ADDR_GIE                0x04
#define XPROCESADOR_VIDEO_CONTROL_ADDR_IER                0x08
#define XPROCESADOR_VIDEO_CONTROL_ADDR_ISR                0x0c
#define XPROCESADOR_VIDEO_CONTROL_ADDR_MODE_DATA          0x10
#define XPROCESADOR_VIDEO_CONTROL_BITS_MODE_DATA          32
#define XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_ENTERO_DATA    0x18
#define XPROCESADOR_VIDEO_CONTROL_BITS_MBS_ENTERO_DATA    32
#define XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_DECIMAL_DATA   0x20
#define XPROCESADOR_VIDEO_CONTROL_BITS_MBS_DECIMAL_DATA   32
#define XPROCESADOR_VIDEO_CONTROL_ADDR_CAMARA_ACTIVA_DATA 0x28
#define XPROCESADOR_VIDEO_CONTROL_BITS_CAMARA_ACTIVA_DATA 32

