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
- `codigo/vivado/` — Script Tcl para reconstruir el proyecto de
  Vivado (block design, configuración de IP y restricciones).
- `codigo/vivado/IpRepoArty/` — Bloques IP externos empleados (cámara MT9V111 y salida
    HDMI TX), procedentes de las bibliotecas de Digilent.
- `codigo/vivado/hdl/` — Wrapper top del diseño.
- `codigo/vivado/mig/` — Archivo de configuración del controlador de memoria (MIG).
- `codigo/vivado/constraints/` — Archivo de restricciones de la placa (.xdc).

## Requisitos

- Vivado / Vitis 2019.1, de AMD/Xilinx.
- Board files de Digilent para la Arty S7-50 instalados en Vivado (ver nota más abajo).
- Placa Arty S7-50 (Spartan-7 XC7S50) de Digilent.
- Cámara MT9V111 de Aptina.
- Conexión HDMI adaptada como la que se describe en la memoria, o un módulo de
  expansión de la misma conectividad (necesario un cable HDMI).
- Monitor con entrada HDMI.

> **Nota sobre los board files:** la reconstrucción del proyecto requiere que
> Vivado tenga instalados los board files de la Arty S7-50 de Digilent. Si
> Vivado no reconoce la placa al reconstruir, pueden obtenerse del repositorio
> oficial de board files de Digilent e instalarse en la carpeta de boards de
> Vivado, o a través de su gestor de placas.

## Montaje
1. Conectar la placa Arty S7-50 a un puerto USB de la máquina local mediante un
   cable micro USB.
2. Conectar la cámara MT9V111 a los puertos indicados en el archivo de
   restricciones, según la descripción de pines que este recoge para los puertos JC y JD.
3. Conectar al puerto JB la conexión HDMI adaptada, o un módulo de expansión
   HDMI de la misma conectividad.
4. Conectar el otro extremo de la conexión HDMI al monitor de salida.

## Reconstrucción del proyecto

1. **Abrir Vivado 2019.1.** Debe disponerse de esta misma versión instalada y de
   los board files de Digilent (ver Requisitos).
2. **Clonar el repositorio** en la máquina local.
3. **Ejecutar el script Tcl.** En la página inicial de Vivado: `Tools → Run Tcl
   Script` y seleccionar el script disponible en `codigo/vivado/`.
4. **Reconstrucción automática.** Vivado reconstruye el proyecto completo de
   forma autónoma, incluyendo el diagrama de bloques y los archivos necesarios.
5. **Generación del bitstream.** Arrancar la síntesis, la implementación y la
   generación del bitstream.
6. **Exportar a SDK.** Finalizada la generación del bitstream, seleccionar
   `File → Export hardware` y marcar la opción de incluir el bitstream.


## Creación de la aplicación software en SDK

1. **Crear la aplicación.** En SDK, `File → New → Application Project`. Asignar
   un nombre, seleccionar el hardware exportado, pulsar *Next* y elegir *Empty
   Application*.
2. **Añadir el código.** En el explorador de proyectos, desplegar la aplicación
   creada y la carpeta `src`, crear un nuevo *Source File* con extensión `.c` y
   copiar en él el código disponible en `codigo/software/`.
3. **Abrir el terminal serie.** Para interactuar con la aplicación y ver los
   mensajes de depuración: `Window → Show View → Other → Terminal`. Configurar el
   puerto USB al que se conecta la placa con los siguientes parámetros:

   - Baudrate: 9600 
   - Bits de datos: 8
   - Paridad: ninguna
   - Bits de parada: 1
   - Control de flujo: ninguno

   Pulsar el botón verde para conectar con la placa.

## Carga en la placa
1. **Programar la FPGA.** Pulsar *Program FPGA* y seleccionar el bitstream y el
   hardware exportados.
2. **Ejecutar la aplicación.** Clic derecho sobre la aplicación desarrollada y
   `Run As → Launch on Hardware (System Debugger)`.
3. El terminal mostrará los mensajes de inicialización del sistema de vídeo y,
   finalmente, el menú de opciones, con el que el usuario puede interactuar. En
   el monitor se mostrará el vídeo según el modo seleccionado.



