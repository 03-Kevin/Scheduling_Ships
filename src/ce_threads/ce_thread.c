/**
 * @brief ce_thread.c: Este archivo contiene las funciones necesarias para
 * gestionar hilos a nivel de usuario, específicamente para la creación,
 * finalización y espera de hilos (barcos) que simulan el cruce de un canal.
 *
 * @param thread_id Identificador único para cada hilo.
 * @param start_routine Función que ejecutará el hilo.
 * @param arg Argumentos pasados a la función del hilo.
 */

#include "ce_thread.h"
#include <stdlib.h>
#include <unistd.h>

// Variable estática para asignar IDs únicos a los hilos
static int next_thread_id = 0;

/**
 * @brief CEthread_create: Crea un nuevo hilo (barco) y lo pone en estado LISTO.
 * El hilo ejecutará la función pasada como parámetro (start_routine).
 *
 * @param thread Puntero a la estructura CEthread que representará el hilo.
 * @param start_routine Función que ejecutará el hilo.
 * @param arg Argumentos que se pasan a la función del hilo.
 * @return int 0 si la creación es exitosa.
 */
int CEthread_create(CEthread *thread, void (*start_routine)(void *), void *arg)
{
    // Asigna un ID único al hilo (barco)
    thread->thread_id = next_thread_id++;

    // Asigna la función que ejecutará el hilo (barco) y sus argumentos
    thread->start_routine = start_routine;
    thread->arg = arg;

    // Inicializa el estado del hilo como LISTO
    thread->estado = LISTO;

    // TODO Aqui es donde supongo se agregarían a la cola que trabaja kevin

    return 0;
}

/**
 * @brief CEthread_end: Finaliza el hilo (barco) cambiando su estado a TERMINADO.
 * Esta función se llama cuando el barco ha completado su cruce del canal.
 *
 * @param thread Puntero a la estructura CEthread que representa el hilo (barco).
 */
void CEthread_end(CEthread *thread)
{
    // Cambia el estado del hilo (barco) a TERMINADO
    thread->estado = TERMINADO;
}

/**
 * @brief CEthread_join: Espera a que el hilo (barco) termine su ejecución.
 * Esta función simula la espera activa, revisando el estado del hilo hasta que
 * se marque como TERMINADO.
 *
 * @param thread Puntero a la estructura CEthread que representa el hilo (barco).
 */
void CEthread_join(CEthread *thread)
{
    // Espera activa hasta que el hilo (barco) complete su ejecución
    while (thread->estado != TERMINADO)
    {
        usleep(1000); // Simulación de espera
    }
}
