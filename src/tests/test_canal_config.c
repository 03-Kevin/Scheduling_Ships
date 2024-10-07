#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../canal_config.h" // Incluir el archivo de encabezado

void test_read_canal_config()
{
    CanalConfig config = read_canal_config("canal_config_test.txt"); // Cambiar al nombre de tu archivo de prueba

    // Comprobar que los valores se leyeron correctamente
    assert(strcmp(config.control_flujo, "equidad") == 0);
    assert(config.largo_canal == 1000);
    assert(config.velocidad_barco == 10);
    assert(config.cantidad_barcos_listos == 5);
    assert(config.tiempo_letrero == 3000);
    assert(config.parametro_w == 2.5f);

    printf("Prueba de lectura del archivo de configuración: PASADA.\n");
}

int main()
{
    printf("Ejecutando prueba de lectura de archivo de configuración...\n");
    test_read_canal_config(); // Ejecutar la prueba
    printf("Fin de las pruebas.\n");
    return 0;
}
