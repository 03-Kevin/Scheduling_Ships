/*
 * mutex.h
 *
 *  Created on: 10/11/2016
 * author: @kevinrodriguez
 *
 * este archivo contiene las funciones para el mutex manual.
 */
#ifndef MUTEX_H
#define MUTEX_H

void lock(volatile int *lock);

void unlock(volatile int *lock);

#endif // MUTEX_H
