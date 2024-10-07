#ifndef CE_MUTEX_H // Include guard
#define CE_MUTEX_H

// Definición de la estructura de un mutex
// 0 = desbloqueado, 1 = bloqueado
typedef struct
{
    int locked;
} CEmutex;

// Funciones para manejar el mutex
void CEmutex_init(CEmutex *mutex);
void CEmutex_lock(CEmutex *mutex);
void CEmutex_unlock(CEmutex *mutex);
void CEmutex_destroy(CEmutex *mutex);

#endif // CE_MUTEX_H
