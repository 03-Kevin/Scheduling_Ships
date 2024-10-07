#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "../ce_threads/ce_thread.h" // Incluir el archivo con las funciones de CEthreads
#include "../ce_mutex/ce_mutex.h"    // Incluir el archivo con las funciones de CEmutex

#define NUM_BARCO 5 // Número de barcos a simular

CEmutex canal_mutex; // Mutex para controlar el acceso al canal

#define TIMEOUT 1 // Tiempo en segundos antes de que la prueba falle

#define ITERACIONES 10 // Número de iteraciones por hilo

CEmutex mutex;              // Mutex para proteger el recurso compartido
int recurso_compartido = 0; // Recurso compartido

// Variable para manejar el timeout
sigjmp_buf jump_buffer;

// Manejar la señal de timeout
void timeout_handler(int signum)
{
    siglongjmp(jump_buffer, 1);
}

// Función dummy que no termina el hilo
void dummy_function_no_end(void *arg)
{
    CEthread *barco = (CEthread *)arg; // Convertir el argumento a CEthread*
    usleep(5000000);                   // Simular trabajo, pero nunca se termina
}

// Función dummy para que el hilo (barco) la ejecute
void dummy_function(void *arg)
{
    CEthread *barco = (CEthread *)arg; // Convertir el argumento a CEthread*
    usleep(5000);                      // Simular trabajo
    barco->estado = TERMINADO;         // Cambiar el estado a TERMINADO
}

// Planificador de hilos
void run_threads(CEthread *threads, int num_threads)
{
    while (1)
    {
        int all_finished = 1; // Supón que todos están terminados inicialmente

        for (int i = 0; i < num_threads; i++)
        {
            if (threads[i].estado == LISTO)
            {
                all_finished = 0;                      // Al menos un hilo está listo
                threads[i].estado = EJECUTANDO;        // Cambia el estado a EJECUTANDO
                threads[i].start_routine(&threads[i]); // Ejecuta la función
                // No cambiar el estado a TERMINADO aquí, lo hará el hilo
            }
        }

        // Salir del bucle si todos los hilos han terminado
        if (all_finished)
        {
            break;
        }

        usleep(100000); // Agregar una pequeña pausa para evitar un bucle intenso
    }
}

// Prueba para la creación de hilos (barcos)
void test_CEthread_create()
{
    CEthread barco;
    int arg = 42;

    // Crear el barco (hilo)
    int result = CEthread_create(&barco, dummy_function, &arg);

    assert(result == 0);
    assert(barco.thread_id >= 0);
    assert(barco.estado == LISTO);
    assert(barco.start_routine == dummy_function);
    assert(barco.arg == &arg);

    printf("Prueba CEthread_create: PASADA\n");
}

// Prueba para la función CEthread_end
void test_CEthread_end()
{
    CEthread barco;
    int arg = 42;

    // Crear el barco (hilo)
    CEthread_create(&barco, dummy_function, &arg);

    CEthread_end(&barco);

    // Verificar que el hilo ha terminado
    assert(barco.estado == TERMINADO);

    printf("Prueba CEthread_end: PASADA\n");
}

// Prueba para CEthread_join (espera a que todos los barcos terminen)
void test_CEthread_join()
{
    CEthread barcos[NUM_BARCO]; // Arreglo de hilos (barcos)
    int ids[NUM_BARCO];         // Arreglo para IDs de barcos

    // Inicializar el mutex
    CEmutex_init(&canal_mutex);

    // Crear varios barcos (hilos)
    for (int i = 0; i < NUM_BARCO; i++)
    {
        ids[i] = i + 1;
        CEthread_create(&barcos[i], dummy_function, &barcos[i]); // Cambiar aquí
    }

    // Ejecutar el planificador
    run_threads(barcos, NUM_BARCO);

    // Unir (esperar) a todos los hilos
    for (int i = 0; i < NUM_BARCO; i++)
    {
        CEthread_join(&barcos[i]);
    }

    // Verificar que todos los barcos han terminado
    for (int i = 0; i < NUM_BARCO; i++)
    {
        assert(barcos[i].estado == TERMINADO);
    }

    printf("Prueba CEthread_join: PASADA\n");

    // Destruir el mutex al final
    CEmutex_destroy(&canal_mutex);
}

// Prueba para verificar que el código no avanza si join se queda esperando
void test_CEthread_join_no_end()
{
    CEthread barco; // Un solo hilo

    // Crear el barco (hilo) que no terminará
    CEthread_create(&barco, dummy_function_no_end, &barco);

    // Configurar el handler de la señal
    signal(SIGALRM, timeout_handler);
    alarm(TIMEOUT); // Establecer el timeout

    // Ejecutar la prueba en un entorno controlado
    if (sigsetjmp(jump_buffer, 1) == 0)
    {
        CEthread_join(&barco); // Este debería quedarse esperando indefinidamente
        // Esta línea no debería ejecutarse
        printf("La prueba debería no haber pasado. El hilo no terminó.\n");
    }
    else
    {
        printf("Prueba CEthread_join_no_end: PASADA\n");
    }

    // Desactivar el handler
    alarm(0);
}

// Prueba para los mutexesde lock y unlock
void test_CEmutex()
{
    CEmutex canal_mutex;

    // Inicializar el mutex
    CEmutex_init(&canal_mutex);

    // Bloquear el mutex
    CEmutex_lock(&canal_mutex);
    assert(canal_mutex.locked == 1); // Verificar que el mutex está bloqueado

    // Desbloquear el mutex
    CEmutex_unlock(&canal_mutex);
    assert(canal_mutex.locked == 0); // Verificar que el mutex está desbloqueado

    // Destruir el mutex
    CEmutex_destroy(&canal_mutex);

    printf("Prueba CEmutex: PASADA\n");
}

// Función que los hilos ejecutarán
void incrementar_recurso(void *arg)
{
    CEthread *barco = (CEthread *)arg; // Convertir el argumento a CEthread*
    for (int i = 0; i < ITERACIONES; i++)
    {
        // Bloquear el mutex antes de acceder al recurso compartido
        CEmutex_lock(&mutex);

        // Modificar el recurso compartido
        recurso_compartido++;

        // Desbloquear el mutex
        CEmutex_unlock(&mutex);

        // Simular un breve retraso
        usleep(100);
    }

    // Terminar el hilo después de realizar su tarea
    CEthread_end(barco); // Asegúrate de marcar el hilo como terminado
}

// Prueba para verificar el uso de hilos y mutex
void test_mutex_with_threads()
{
    CEthread hilos[NUM_BARCO]; // Arreglo de hilos
    int ids[NUM_BARCO];        // Arreglo para IDs de hilos

    // Inicializar el mutex
    CEmutex_init(&mutex);

    // Crear varios hilos
    for (int i = 0; i < NUM_BARCO; i++)
    {
        ids[i] = i;                                                 // Asignar ID a cada hilo
        CEthread_create(&hilos[i], incrementar_recurso, &hilos[i]); // Cambiar a hilos[i]
    }

    // Ejecutar el planificador
    run_threads(hilos, NUM_BARCO); // Esta función debería estar definida como en ejemplos anteriores

    // Unir (esperar) a todos los hilos
    for (int i = 0; i < NUM_BARCO; i++)
    {
        CEthread_join(&hilos[i]);
    }

    // Verificar el resultado final
    assert(recurso_compartido == NUM_BARCO * ITERACIONES); // Debe ser igual al total de incrementos

    printf("Prueba de hilos con mutex: PASADA\n");

    // Destruir el mutex al final
    CEmutex_destroy(&mutex);
}

// Función principal para ejecutar todas las pruebas
int main()
{
    printf("Ejecutando pruebas de CEthreads...\n");

    test_CEthread_create();      // Prueba la creación de hilos
    test_CEthread_end();         // Prueba la finalización de hilos
    test_CEthread_join();        // Prueba la función join (espera)
    test_CEthread_join_no_end(); // Prueba la función join (espera)
    test_CEmutex();              // Prueba para los mutexes
    test_mutex_with_threads();

    printf("Todas las pruebas han pasado correctamente.\n");
    return 0;
}
