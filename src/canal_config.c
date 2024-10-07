#include "canal_config.h"

// TODO Esto lo estaba haciendo leyendo desde el txt pero no funciona, hay que buscar una mejor forma probablemente.
// Función para leer el archivo de configuración
CanalConfig read_canal_config(const char *filename)
{
    CanalConfig config = {0}; // Inicializar la configuración
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error al abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        // Eliminar el salto de línea al final de la línea
        line[strcspn(line, "\n")] = 0;

        // Imprimir la línea leída para depuración
        printf("Leyendo línea: '%s' (Longitud: %zu)\n", line, strlen(line));

        // Parsear cada línea de la configuración
        if (strncmp(line, "CONTROL_FLUJO=", 15) == 0)
        {
            // Leer y eliminar espacios en blanco
            char value[MAX_LINE_LENGTH];
            sscanf(line + 15, "%99s", value); // Leer el valor
            // Limpiar posibles espacios
            strcpy(config.control_flujo, value);                         // Asignar el valor limpio
            printf("CONTROL_FLUJO leído: '%s'\n", config.control_flujo); // Para depuración
        }
        else if (strncmp(line, "LARGO_CANAL=", 12) == 0)
        {
            sscanf(line + 12, "%d", &config.largo_canal);
        }
        else if (strncmp(line, "VELOCIDAD_BARCO=", 16) == 0)
        {
            sscanf(line + 16, "%d", &config.velocidad_barco);
        }
        else if (strncmp(line, "CANTIDAD_BARCO_LISTOS=", 22) == 0)
        {
            sscanf(line + 22, "%d", &config.cantidad_barcos_listos);
        }
        else if (strncmp(line, "TIEMPO_LETRERO=", 15) == 0)
        {
            sscanf(line + 15, "%d", &config.tiempo_letrero);
        }
        else if (strncmp(line, "PARAMETRO_W=", 12) == 0)
        {
            sscanf(line + 12, "%f", &config.parametro_w);
        }
    }

    fclose(file);
    return config;
}
