# TFG-migracion-video-fpga
Repositorio que recoge el diseño en Vivado, la aplicación software, el código del módulo HLS y los documentos pertenicentes al Trabajo de Fin de Grado: Migración de un canal de vídeo en tiempo real desde una FPGA SoC a una FPGA discreta de bajo coste.


# Migración de un canal de vídeo en tiempo real de FPGA SoC a FPGA discreta

Trabajo de Fin de Grado — Alfonso Suárez Gasull
Grado en Ingeniería de Tecnologías de la Telecomunicación, Universidad de Málaga, 2026
Tutor: Martín González García
Cotutor: Juan Antonio Rodríguez Fernández

## Descripción

Repositorio con el material del TFG sobre la migración de un canal de vídeo
en tiempo real desde una plataforma SoC (Zybo Z7, Zynq-7000) a una FPGA
discreta de bajo coste (Arty S7-50, Spartan-7). Incluye el código fuente,
los scripts de reconstrucción del proyecto de Vivado y la memoria.

## Estructura del repositorio

- `memoria/` — Memoria del TFG en PDF.
- `presentacion/` — Presentación empleada en la defensa.
- `codigo/hls/` — Código C++ del módulo de procesamiento y generación
  de vídeo, diseñado mediante Vitis HLS.
- `codigo/software/` — Aplicación de control bare-metal
  ejecutada por el procesador soft-core MicroBlaze.
- `codigo/vivado/` — Scripts Tcl para reconstruir el proyecto de
  Vivado (block design, configuración de IP y restricciones) y archivos necesarios.
- `bitstreams/` — Bitstream final verificado del sistema.

## Requisitos

- Vivado / Vitis 2019.1, de AMD/Xilinx.
- Placa Arty S7-50 (Spartan-7 XC7S50) de Digilent.
- Cámara MT9V111.
- Conexión HDMI adaptada como la que se describe en la memoria o módulo de misma conectividad.

## Reconstrucción del proyecto


Instrucciones detalladas en `codigo/vivado/README_vivado.md`.

## Carga en la placa



