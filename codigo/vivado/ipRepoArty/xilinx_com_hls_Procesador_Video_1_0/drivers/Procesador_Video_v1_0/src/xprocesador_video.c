// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xprocesador_video.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XProcesador_video_CfgInitialize(XProcesador_video *InstancePtr, XProcesador_video_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XProcesador_video_Start(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL) & 0x80;
    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XProcesador_video_IsDone(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XProcesador_video_IsIdle(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XProcesador_video_IsReady(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XProcesador_video_EnableAutoRestart(XProcesador_video *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XProcesador_video_DisableAutoRestart(XProcesador_video *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_AP_CTRL, 0);
}

void XProcesador_video_Set_mode(XProcesador_video *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MODE_DATA, Data);
}

u32 XProcesador_video_Get_mode(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MODE_DATA);
    return Data;
}

void XProcesador_video_Set_mbs_entero(XProcesador_video *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_ENTERO_DATA, Data);
}

u32 XProcesador_video_Get_mbs_entero(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_ENTERO_DATA);
    return Data;
}

void XProcesador_video_Set_mbs_decimal(XProcesador_video *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_DECIMAL_DATA, Data);
}

u32 XProcesador_video_Get_mbs_decimal(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_MBS_DECIMAL_DATA);
    return Data;
}

void XProcesador_video_Set_camara_activa(XProcesador_video *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_CAMARA_ACTIVA_DATA, Data);
}

u32 XProcesador_video_Get_camara_activa(XProcesador_video *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_CAMARA_ACTIVA_DATA);
    return Data;
}

void XProcesador_video_InterruptGlobalEnable(XProcesador_video *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_GIE, 1);
}

void XProcesador_video_InterruptGlobalDisable(XProcesador_video *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_GIE, 0);
}

void XProcesador_video_InterruptEnable(XProcesador_video *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_IER);
    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_IER, Register | Mask);
}

void XProcesador_video_InterruptDisable(XProcesador_video *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_IER);
    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_IER, Register & (~Mask));
}

void XProcesador_video_InterruptClear(XProcesador_video *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XProcesador_video_WriteReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_ISR, Mask);
}

u32 XProcesador_video_InterruptGetEnabled(XProcesador_video *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_IER);
}

u32 XProcesador_video_InterruptGetStatus(XProcesador_video *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XProcesador_video_ReadReg(InstancePtr->Control_BaseAddress, XPROCESADOR_VIDEO_CONTROL_ADDR_ISR);
}

