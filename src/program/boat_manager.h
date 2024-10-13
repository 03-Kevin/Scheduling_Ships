#ifndef BOAT_MANAGER_H
#define BOAT_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ce_threads/ce_thread.h" // Include CEthread functions
#include "../ce_mutex/ce_mutex.h"    // Include CEmutex functions

// Boat types with their respective speeds and priorities
typedef struct
{
    int speed;
    int priority;
} BoatType;

extern BoatType boat_types[];

// Global variables for the boats
extern int boat_count;      // Current number of boats
extern CEmutex canal_mutex; // Mutex to control access to the canal
extern int canal_length;

// Function declarations
void initialize_boats_left(int queue_quantity); // To initialize boats array
void initialize_boats_right(int queue_quantity); // To initialize boats array
void add_boats_from_menu(int normal_left, int fishing_left, int patrol_left,
                         int normal_right, int fishing_right, int patrol_right,
                         int canal_length, int queue_quantity);
void create_boat(char key, int queue_quantity); // Create boat based on key
void start_threads(int flow_control_method, int w, int change_direction_period);
int kbhit(void);               // Check if a key has been pressed
void cross_channel(void *arg); // Function for crossing the channel

#endif // BOAT_MANAGER_H
