// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XPROCESADOR_VIDEO_H
#define XPROCESADOR_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xprocesador_video_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XProcesador_video_Config;
#endif

typedef struct {
    u32 Control_BaseAddress;
    u32 IsReady;
} XProcesador_video;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XProcesador_video_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XProcesador_video_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XProcesador_video_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XProcesador_video_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XProcesador_video_Initialize(XProcesador_video *InstancePtr, u16 DeviceId);
XProcesador_video_Config* XProcesador_video_LookupConfig(u16 DeviceId);
int XProcesador_video_CfgInitialize(XProcesador_video *InstancePtr, XProcesador_video_Config *ConfigPtr);
#else
int XProcesador_video_Initialize(XProcesador_video *InstancePtr, const char* InstanceName);
int XProcesador_video_Release(XProcesador_video *InstancePtr);
#endif

void XProcesador_video_Start(XProcesador_video *InstancePtr);
u32 XProcesador_video_IsDone(XProcesador_video *InstancePtr);
u32 XProcesador_video_IsIdle(XProcesador_video *InstancePtr);
u32 XProcesador_video_IsReady(XProcesador_video *InstancePtr);
void XProcesador_video_EnableAutoRestart(XProcesador_video *InstancePtr);
void XProcesador_video_DisableAutoRestart(XProcesador_video *InstancePtr);

void XProcesador_video_Set_mode(XProcesador_video *InstancePtr, u32 Data);
u32 XProcesador_video_Get_mode(XProcesador_video *InstancePtr);
void XProcesador_video_Set_mbs_entero(XProcesador_video *InstancePtr, u32 Data);
u32 XProcesador_video_Get_mbs_entero(XProcesador_video *InstancePtr);
void XProcesador_video_Set_mbs_decimal(XProcesador_video *InstancePtr, u32 Data);
u32 XProcesador_video_Get_mbs_decimal(XProcesador_video *InstancePtr);
void XProcesador_video_Set_camara_activa(XProcesador_video *InstancePtr, u32 Data);
u32 XProcesador_video_Get_camara_activa(XProcesador_video *InstancePtr);

void XProcesador_video_InterruptGlobalEnable(XProcesador_video *InstancePtr);
void XProcesador_video_InterruptGlobalDisable(XProcesador_video *InstancePtr);
void XProcesador_video_InterruptEnable(XProcesador_video *InstancePtr, u32 Mask);
void XProcesador_video_InterruptDisable(XProcesador_video *InstancePtr, u32 Mask);
void XProcesador_video_InterruptClear(XProcesador_video *InstancePtr, u32 Mask);
u32 XProcesador_video_InterruptGetEnabled(XProcesador_video *InstancePtr);
u32 XProcesador_video_InterruptGetStatus(XProcesador_video *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
