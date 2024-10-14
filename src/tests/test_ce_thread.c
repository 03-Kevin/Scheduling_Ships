#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "../ce_threads/ce_thread.h" // Incluir el archivo con las funciones de CEthreads
#include "../ce_mutex/ce_mutex.h"    // Incluir el archivo con las funciones de CEmutex

#define NUM_BARCO 5 // Número de barcos a simular
#define CANAL_LENGTH 20

CEmutex test_threads_canal_mutex; // Mutex para controlar el acceso al canal

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
    barco->state = DONE;               // Cambiar el estado a DONE
}

// Planificador de hilos
void run_threads(CEthread *threads, int num_threads)
{
    while (1)
    {
        int all_finished = 1; // Supón que todos están DONE inicialmente

        for (int i = 0; i < num_threads; i++)
        {
            if (threads[i].state == READY)
            {
                all_finished = 0;                        // Al menos un hilo está READY
                threads[i].state = RUNNING;              // Cambia el estado a RUNNING
                threads[i].thread_function(&threads[i]); // Ejecuta la función
            }
        }

        // Salir del bucle si todos los hilos han DONE
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
    int result = CEthread_create(&barco, OCEANO_IZQ, 1, 10, CANAL_LENGTH, 0, dummy_function, &arg); // Asignar valores a original_side, priority, y speed

    assert(result == 0);
    assert(barco.thread_id >= 0);
    assert(barco.state == READY);
    assert(barco.thread_function == dummy_function);
    assert(barco.arg == &arg);
    assert(barco.burst_time == 2);   // Asegurarse de que burst_time por defecto es 0
    assert(barco.arrival_time == 0); // Asegurarse de que arrival_time por defecto es 0

    printf("Prueba CEthread_create: PASADA\n");
}

// Prueba para la función CEthread_end
void test_CEthread_end()
{
    CEthread barco;
    int arg = 42;

    // Crear el barco (hilo)
    CEthread_create(&barco, OCEANO_IZQ, 1, 10, 0, CANAL_LENGTH, dummy_function, &arg);

    CEthread_end(&barco);

    // Verificar que el hilo ha DONE
    assert(barco.state == DONE);

    printf("Prueba CEthread_end: PASADA\n");
}

// Prueba para CEthread_join (espera a que todos los barcos terminen)
void test_CEthread_join()
{
    CEthread barcos[NUM_BARCO]; // Arreglo de hilos (barcos)
    int ids[NUM_BARCO];         // Arreglo para IDs de barcos

    // Inicializar el mutex
    CEmutex_init(&test_threads_canal_mutex);

    // Crear varios barcos (hilos)
    for (int i = 0; i < NUM_BARCO; i++)
    {
        ids[i] = i + 1;
        CEthread_create(&barcos[i], OCEANO_IZQ, 1, 10, CANAL_LENGTH, 0, dummy_function, &barcos[i]); // Pasar valores adicionales
    }

    // Ejecutar el planificador
    run_threads(barcos, NUM_BARCO);

    // Unir (esperar) a todos los hilos
    for (int i = 0; i < NUM_BARCO; i++)
    {
        CEthread_join(&barcos[i]);
    }

    // Verificar que todos los barcos han DONE
    for (int i = 0; i < NUM_BARCO; i++)
    {
        assert(barcos[i].state == DONE);
    }

    printf("Prueba CEthread_join: PASADA\n");

    // Destruir el mutex al final
    CEmutex_destroy(&test_threads_canal_mutex);
}

// Prueba para verificar que el código no avanza si join se queda esperando
void test_CEthread_join_no_end()
{
    CEthread barco; // Un solo hilo

    // Crear el barco (hilo) que no terminará
    CEthread_create(&barco, OCEANO_IZQ, 1, 10, CANAL_LENGTH, 0, dummy_function_no_end, &barco);

    // Configurar el handler de la señal
    signal(SIGALRM, timeout_handler);
    alarm(TIMEOUT); // Establecer el timeout

    // Ejecutar la prueba en un entorno controlado
    if (sigsetjmp(jump_buffer, 1) == 0)
    {
        CEthread_join(&barco); // Este debería quedarse esperando indefinidamente
        printf("La prueba debería no haber pasado. El hilo no terminó.\n");
    }
    else
    {
        printf("Prueba CEthread_join_no_end: PASADA\n");
    }

    // Desactivar el handler
    alarm(0);
}

// Prueba para los mutexes de lock y unlock
void test_CEmutex()
{
    CEmutex test_threads_canal_mutex;

    // Inicializar el mutex
    CEmutex_init(&test_threads_canal_mutex);

    // Bloquear el mutex
    CEmutex_lock(&test_threads_canal_mutex);
    assert(test_threads_canal_mutex.locked == 1); // Verificar que el mutex está bloqueado

    // Desbloquear el mutex
    CEmutex_unlock(&test_threads_canal_mutex);
    assert(test_threads_canal_mutex.locked == 0); // Verificar que el mutex está desbloqueado

    // Destruir el mutex
    CEmutex_destroy(&test_threads_canal_mutex);

    printf("Prueba CEmutex: PASADA\n");
}

// Función que los hilos ejecutarán
void incrementar_recurso(void *arg)
{
    CEthread *barco = (CEthread *)arg; // Convertir el argumento a CEthread*
    for (int i = 0; i < ITERACIONES; i++)
    {
        CEmutex_lock(&mutex);
        recurso_compartido++;
        CEmutex_unlock(&mutex);
        usleep(100);
    }
    CEthread_end(barco);
}

// Prueba para verificar el uso de hilos y mutex
void test_mutex_with_threads()
{
    CEthread hilos[NUM_BARCO];
    int ids[NUM_BARCO];

    CEmutex_init(&mutex);

    for (int i = 0; i < NUM_BARCO; i++)
    {
        ids[i] = i;
        CEthread_create(&hilos[i], OCEANO_IZQ, 1, 10, CANAL_LENGTH, 0, incrementar_recurso, &hilos[i]);
    }

    run_threads(hilos, NUM_BARCO);

    for (int i = 0; i < NUM_BARCO; i++)
    {
        CEthread_join(&hilos[i]);
    }

    assert(recurso_compartido == NUM_BARCO * ITERACIONES);

    printf("Prueba de hilos con mutex: PASADA\n");

    CEmutex_destroy(&mutex);
}

// Prueba para la creación de varios hilos (barcos) con los mismos parámetros
void test_CEthread_create_batch()
{
    int num_barcos = 5;      // Número de hilos a crear
    CEthread *barcos = NULL; // Arreglo de hilos (barcos)
    barcos = malloc(5 * sizeof(CEthread));

    // Parámetros comunes para todos los barcos
    int velocidad = 10;    // Velocidad del barco
    int prioridad = 1;     // Prioridad del barco
    int lado = OCEANO_IZQ; // Lado de origen (izquierda)

    // Crear varios barcos con los mismos parámetros
    CEthread_create_batch(barcos, 0, num_barcos, velocidad, CANAL_LENGTH, 0, prioridad, lado, dummy_function, NULL);

    // Verificar que todos los barcos están creados correctamente
    for (int i = 0; i < 5; i++)
    {
        assert(barcos[i].thread_id >= 0);
        assert(barcos[i].state == READY);
        assert(barcos[i].thread_function == dummy_function);
        assert(barcos[i].arg == NULL);
        assert(barcos[i].speed == velocidad);
        assert(barcos[i].priority == prioridad);
        assert(barcos[i].original_side == lado);
        assert(barcos[i].burst_time == 2);   // Asegurarse que burst_time por defecto es 0
        assert(barcos[i].arrival_time == 0); // Asegurarse que arrival_time por defecto es 0
    }
    // Crear varios barcos con los mismos parámetros
    CEthread_create_batch(barcos, 5, 10, velocidad, CANAL_LENGTH, 0, prioridad, lado, dummy_function, NULL);

    // Verificar que todos los barcos están creados correctamente
    for (int i = 5; i < 10; i++)
    {
        assert(barcos[i].thread_id >= 0);
        assert(barcos[i].state == READY);
        assert(barcos[i].thread_function == dummy_function);
        assert(barcos[i].arg == NULL);
        assert(barcos[i].speed == velocidad);
        assert(barcos[i].priority == prioridad);
        assert(barcos[i].original_side == lado);
        assert(barcos[i].burst_time == 2);   // Asegurarse que burst_time por defecto es 0
        assert(barcos[i].arrival_time == 0); // Asegurarse que arrival_time por defecto es 0
    }

    printf("Prueba CEthread_create_batch: PASADA\n");
}

// Función principal para ejecutar todas las pruebas
int main()
{
    printf("RUNNING pruebas de CEthreads...\n");

    test_CEthread_create();
    test_CEthread_end();
    test_CEthread_join();
    test_CEthread_join_no_end();
    test_CEmutex();
    test_mutex_with_threads();
    test_CEthread_create_batch();

    printf("Todas las pruebas han pasado correctamente.\n");
    return 0;
}
