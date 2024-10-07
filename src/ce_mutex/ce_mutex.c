#include <stdio.h>
#include <unistd.h>   // Para usleep
#include "ce_mutex.h" // Incluir la cabecera de CEmutex

/**
 * @brief CEmutex_init: Inicializa el mutex.
 *
 * Establece el estado del mutex como desbloqueado.
 *
 * @param mutex Puntero al mutex que se va a inicializar.
 */
void CEmutex_init(CEmutex *mutex)
{
    mutex->locked = 0; // El mutex comienza desbloqueado
}

/**
 * @brief CEmutex_lock: Bloquea el mutex.
 *
 * Si el mutex ya está bloqueado, el hilo espera hasta que esté libre.
 *
 * @param mutex Puntero al mutex que se va a bloquear.
 */
void CEmutex_lock(CEmutex *mutex)
{
    // Espera activa (busy-wait) hasta que el mutex esté desbloqueado
    while (__sync_lock_test_and_set(&mutex->locked, 1))
    {
        usleep(1000); // Simular un breve retraso antes de volver a intentar
    }
}

/**
 * @brief CEmutex_unlock: Desbloquea el mutex.
 *
 * Libera el mutex para que otros hilos puedan usar el recurso.
 *
 * @param mutex Puntero al mutex que se va a desbloquear.
 */
void CEmutex_unlock(CEmutex *mutex)
{
    __sync_lock_release(&mutex->locked); // Liberar el mutex
}

/**
 * @brief CEmutex_destroy: Destruye el mutex.
 *
 * Resetea el estado del mutex a desbloqueado.
 *
 * @param mutex Puntero al mutex que se va a destruir.
 */
void CEmutex_destroy(CEmutex *mutex)
{
    // En este caso, simplemente reseteamos el estado
    mutex->locked = 0; // Restablecer el mutex
}
