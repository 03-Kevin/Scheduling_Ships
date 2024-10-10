/**
 * @brief ce_thread.c: Este archivo contiene las funciones necesarias para
 * gestionar hilos a nivel de usuario, específicamente para la creación,
 * finalización y espera de hilos (barcos) que simulan el cruce de un canal.
 *
 * @param thread_id Identificador único para cada hilo.
 * @param thread_function Función que ejecutará el hilo.
 * @param arg Argumentos pasados a la función del hilo.
 */

#include "ce_thread.h"
#include <stdlib.h>
#include <unistd.h>

// Variable estática para asignar IDs únicos a los hilos
static int next_thread_id = 0;

/**
 * @brief CEthread_create: Crea un nuevo hilo (barco) y lo pone en state READY.
 * El hilo ejecutará la función pasada como parámetro (thread_function).
 *
 * @param thread Puntero a la estructura CEthread que representará el hilo.
 * @param thread_function Función que ejecutará el hilo.
 * @param arg Argumentos que se pasan a la función del hilo.
 * @return int 0 si la creación es exitosa.
 */
int CEthread_create(CEthread *thread, int original_side, int priority, int speed, int canal_length, int arrival_time, void (*thread_function)(void *), void *arg)
{
    // Asigna un ID único al hilo (barco)
    thread->thread_id = next_thread_id++;

    // Asignar atributos especificos del barco
    thread->original_side = original_side;
    thread->speed = speed;
    thread->priority = priority;
    thread->arrival_time = arrival_time;
    // Calcular el tiempo de cruce en milisegundos
    thread->burst_time = canal_length / speed;

    // Asigna la función que ejecutará el hilo (barco) y sus argumentos
    thread->thread_function = thread_function;
    thread->arg = arg;

    // Inicializa el state del hilo como READY
    thread->state = READY;

    // TODO Aqui es donde supongo se agregarían a la cola que trabaja kevin

    return 0;
}

void CEthread_execute(CEthread *thread)
{
    if (thread->state == READY)
    {
        thread->state = RUNNING;              // Set the state to RUNNING
        thread->thread_function(thread->arg); // Execute the thread function
        thread->state = DONE;                 // Set the state to DONE after execution
    }
}

/**
 * @brief CEthread_end: Finaliza el hilo (barco) cambiando su state a DONE.
 * Esta función se llama cuando el barco ha completado su cruce del canal.
 *
 * @param thread Puntero a la estructura CEthread que representa el hilo (barco).
 */
void CEthread_end(CEthread *thread)
{
    // Cambia el state del hilo (barco) a DONE
    thread->state = DONE;
}

/**
 * @brief CEthread_join: Espera a que el hilo (barco) termine su ejecución.
 * Esta función simula la espera activa, revisando el state del hilo hasta que
 * se marque como DONE.
 *
 * @param thread Puntero a la estructura CEthread que representa el hilo (barco).
 */
void CEthread_join(CEthread *thread)
{
    // Espera activa hasta que el hilo (barco) complete su ejecución
    while (thread->state != DONE)
    {
        usleep(1000); // Simulación de espera
    }
}

/**
 * @brief CEthread_create_batch: Crea varios hilos (barcos) con los mismos parámetros
 * de velocidad, prioridad y lado de origen. Todos los hilos son creados en estado READY
 * y son inicializados con la misma función y argumentos.
 *
 * @param threads Puntero a un arreglo de estructuras CEthread, donde se almacenarán
 * los hilos creados.
 * @param num_threads Número de hilos (barcos) a crear.
 * @param speed Velocidad asignada a todos los hilos (barcos).
 * @param priority Prioridad asignada a todos los hilos (barcos).
 * @param original_side Lado de origen de los hilos (OCEANO_IZQ o OCEANO_DER).
 * @param thread_function Función que ejecutarán todos los hilos (barcos).
 * @param arg Argumento que se pasará a la función de cada hilo.
 */
void CEthread_create_batch(CEthread *threads, int num_threads, int speed, int canal_length, int arrival_time, int priority, int original_side, void (*thread_function)(void *), void *arg)
{
    for (int i = 0; i < num_threads; i++)
    {
        // Crear cada hilo (barco) con los mismos parámetros
        CEthread_create(&threads[i], original_side, priority, speed, canal_length, arrival_time, thread_function, arg);
    }
}