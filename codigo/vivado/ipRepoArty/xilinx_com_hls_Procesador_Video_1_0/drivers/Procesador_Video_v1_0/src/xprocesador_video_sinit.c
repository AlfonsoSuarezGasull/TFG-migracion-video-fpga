// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xprocesador_video.h"

extern XProcesador_video_Config XProcesador_video_ConfigTable[];

XProcesador_video_Config *XProcesador_video_LookupConfig(u16 DeviceId) {
	XProcesador_video_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XPROCESADOR_VIDEO_NUM_INSTANCES; Index++) {
		if (XProcesador_video_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XProcesador_video_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XProcesador_video_Initialize(XProcesador_video *InstancePtr, u16 DeviceId) {
	XProcesador_video_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XProcesador_video_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XProcesador_video_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

