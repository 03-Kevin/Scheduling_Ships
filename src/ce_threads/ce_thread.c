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
#include "../calendar/calendarizador.h"
#include <stdlib.h>
#include <unistd.h>

// Variable estática para asignar IDs únicos a los hilos
static int next_thread_id = 0;
int arrival_counter_left = 0;
int arrival_counter_right = 0;
int join_flag = 0;

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
    if (original_side == OCEANO_DER)
    {
        thread->arrival_time = arrival_counter_right++;
    }

    else if (original_side == OCEANO_IZQ)
    {
        thread->arrival_time = arrival_counter_left++;
    }

    // Calcular el tiempo de cruce en milisegundos
    thread->burst_time = canal_length / speed;

    // Asigna la función que ejecutará el hilo (barco) y sus argumentos
    thread->thread_function = thread_function;
    thread->arg = arg;

    // Inicializa el state del hilo como READY
    thread->state = READY;

    if (original_side == OCEANO_DER)
    {
        enqueue_thread(thread, queue_right);
    }

    else if (original_side == OCEANO_IZQ)
    {
        enqueue_thread(thread, queue_left);
    }

    return 0;
}

void CEthread_execute(CEthread *thread)
{

    thread->state = RUNNING; // Set the state to RUNNING
    printf("Running ID %d\n", thread->thread_id);
    thread->thread_function(thread->arg); // Execute the thread function
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
    join_flag = 1;
    thread->state = RUNNING; // Set the state to RUNNING
    printf("Running ID %d\n", thread->thread_id);
    thread->thread_function(thread->arg); // Execute the thread function
    join_flag = 0;
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
void CEthread_create_batch(CEthread *threads, int base_number, int num_threads, int speed, int canal_length, int arrival_time, int priority, int original_side, void (*thread_function)(void *), void *arg)
{
    printf("Creating boats from %d to %d.\n", base_number, num_threads);
    for (int i = base_number; i < num_threads; i++)
    {
        // Crear cada hilo (barco) con los mismos parámetros
        CEthread_create(&threads[i], original_side, priority, speed, canal_length, arrival_time, thread_function, &threads[i]);
    }
}