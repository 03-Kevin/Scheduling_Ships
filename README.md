# Scheduling_Ships

El objetivo es crear una biblioteca de hilos, con el fin de ejecutar distintas tareas de manera simultánea y calendarizarlas según se requiera.

## Paper del proyecto

link--> https://www.overleaf.com/read/xwccjvrffbrf#6e4f60

## Tabla de Contenidos

- [Descripción](#descripción)
- [Instalación](#instalación)
- [Uso](#uso)

## Descripción

`Scheduling_Ships` es una proyecto diseñada para gestionar y calendarizar tareas utilizando hilos. Permite la ejecución simultánea de múltiples tareas y ofrece diferentes algoritmos de calendarización como Round Robin, SJF, FCFS, Priority y EDF.

## Instalación

Para compilar y ejecutar el proyecto, sigue estos pasos:

1. Clona el repositorio:
   ```sh
   git clone https://github.com/03-Kevin/Scheduling_Ships.git
   ```
2. Hacer make en carpeta src.
3. Ejecutar ./menu en carpeta programa.

## Calendarización y control de flujo

Luego, es posible modificar el archivo `config.tx`. Es importante destacar que, para los algoritmos de control de flujo, la variable `flow_control_method` puede tomar los siguientes valores:

- `1`: Equidad
- `2`: Letrero
- `3`: Tico

En cuanto a los métodos de calendarización, la variable `scheduling_method` puede tomar los siguientes valores:

- `0`: Prioridad
- `1`: Shortest Job First
- `2`: First Come First Served
- `3`: Round Robin
- `4`: Tiempo Real
