/*
 * mutex.c
 *
 *  Created on: 10/11/2016
 * author: @kevinrodriguez
 */
#include "mutex.h"
/**
 * @brief mutex.c: Este archivo contiene las funciones para el mutex manual.
 *  especificamente para bloquear y desbloquear el acceso a la cola de listos.
 *  esperando a que el lock sea 0 para poder acceder a la cola.
 *
 * @param lock
 */
void lock(volatile int *lock)
{
    while (__sync_lock_test_and_set(lock, 1))
    {
    }
}

void unlock(volatile int *lock)
{
    __sync_lock_release(lock);
}
