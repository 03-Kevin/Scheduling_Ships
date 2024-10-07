#ifndef CANAL_CONFIG_H
#define CANAL_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

// Estructura para almacenar la configuración del canal
typedef struct
{
    char control_flujo[MAX_LINE_LENGTH];
    int largo_canal;            // Longitud del canal
    int velocidad_barco;        // Velocidad del barco
    int cantidad_barcos_listos; // Barcos en cola de listos
    int tiempo_letrero;         // Tiempo que el letrero cambia
    float parametro_w;          // Parámetro W
} CanalConfig;

// Función para leer el archivo de configuración
CanalConfig read_canal_config(const char *filename);

#endif // CANAL_CONFIG_H
