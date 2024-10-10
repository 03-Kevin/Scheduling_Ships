// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include "boat_manager.h" // Include the boat manager header
#include "../ce_threads/ce_thread.h"

// Global variables
int boat_count = 0;    // Current number of boats
int boat_quantity = 0; // Maximum number of boats
CEthread *boats = NULL;

// Boat types with their respective speeds and priorities
BoatType boat_types[] = {
    {10, 3}, // Normal: speed 10, priority 3
    {20, 2}, // Pesquero: speed 20, priority 2
    {25, 1}  // Patrulla: speed 25, priority 1
};

CEmutex canal_mutex; // Mutex para controlar el acceso al canal

// Function to initialize the boat list with queue_quantity size
void initialize_boats(int queue_quantity)
{
    // Set the global boat quantity
    boat_quantity = queue_quantity;

    // Allocate memory for the boats
    boats = malloc(queue_quantity * sizeof(CEthread));
    if (boats == NULL)
    {
        printf("Error: Could not allocate memory for boats.\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    printf("Boat list initialized with capacity for %d boats.\n", queue_quantity);
}

void add_boats_from_menu(int normal_left, int fishing_left, int patrol_left,
                         int normal_right, int fishing_right, int patrol_right,
                         int canal_length, int queue_quantity)
{
    int total_to_add = normal_left + fishing_left + patrol_left + normal_right + fishing_right + patrol_right;

    // Check if adding the boats would exceed the total capacity
    if (boat_count + total_to_add > queue_quantity)
    {
        printf("Cannot create more boats. Maximum capacity reached.\n");
        return;
    }

    // Create boats for the left side
    if (normal_left > 0)
    {
        CEthread_create_batch(boats + boat_count, normal_left, 10, canal_length, 0, 3, OCEANO_IZQ, cross_channel, NULL);
        boat_count += normal_left;
    }

    if (fishing_left > 0)
    {
        CEthread_create_batch(boats + boat_count, fishing_left, 20, canal_length, 0, 2, OCEANO_IZQ, cross_channel, NULL);
        boat_count += fishing_left;
    }

    if (patrol_left > 0)
    {
        CEthread_create_batch(boats + boat_count, patrol_left, 25, canal_length, 0, 1, OCEANO_IZQ, cross_channel, NULL);
        boat_count += patrol_left;
    }

    // Create boats for the right side
    if (normal_right > 0)
    {
        CEthread_create_batch(boats + boat_count, normal_right, 10, canal_length, 0, 3, OCEANO_DER, cross_channel, NULL);
        boat_count += normal_right;
    }

    if (fishing_right > 0)
    {
        CEthread_create_batch(boats + boat_count, fishing_right, 20, canal_length, 0, 2, OCEANO_DER, cross_channel, NULL);
        boat_count += fishing_right;
    }

    if (patrol_right > 0)
    {
        CEthread_create_batch(boats + boat_count, patrol_right, 25, canal_length, 0, 1, OCEANO_DER, cross_channel, NULL);
        boat_count += patrol_right;
    }

    printf("%d boats added to the list. Total count: %d\n", total_to_add, boat_count);
}

// Function to check if a key has been pressed
int kbhit(void)
{
    struct termios oldt, newt;
    int oldf;
    char ch;
    int retval;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    retval = read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (retval == 1)
    {
        return ch; // Return the character read
    }

    return 0; // No key was pressed
}

// Cross channel function: locks the canal mutex and starts the crossing
void cross_channel(void *arg)
{
    CEthread *barco = (CEthread *)arg;

    // Lock the canal mutex before crossing
    CEmutex_lock(&canal_mutex);
    printf("Barco %d ha bloqueado el canal. Empieza a cruzar con tiempo estimado: %d segundos.\n", barco->thread_id, barco->burst_time);

    // Decrement burst time in a loop until it reaches 0
    while (barco->burst_time > 0)
    {
        sleep(1);            // Simulate crossing for 1 second
        barco->burst_time--; // Decrement burst time
        printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
    }

    printf("Barco %d ha cruzado el canal.\n", barco->thread_id);

    // Unlock the canal mutex after crossing
    CEmutex_unlock(&canal_mutex);
}

// Function to create a boat based on key press
void create_boat(char key, int queue_quantity)
{
    if (boat_count >= queue_quantity)
    {
        printf("No se pueden crear más boats. Reached maximum capacity of %d boats.\n", queue_quantity);
        return;
    }

    int original_side = (key == 'q' || key == 'w' || key == 'e') ? OCEANO_DER : OCEANO_IZQ;
    BoatType selected_boat;

    switch (key)
    {
    case 'q':
        selected_boat = boat_types[0]; // Normal
        break;
    case 'w':
        selected_boat = boat_types[1]; // Pesquero
        break;
    case 'e':
        selected_boat = boat_types[2]; // Patrulla
        break;
    case 'r':
        selected_boat = boat_types[0]; // Normal
        original_side = OCEANO_IZQ;
        break;
    case 't':
        selected_boat = boat_types[1]; // Pesquero
        original_side = OCEANO_IZQ;
        break;
    case 'y':
        selected_boat = boat_types[2]; // Patrulla
        original_side = OCEANO_IZQ;
        break;
    default:
        return; // Invalid key
    }

    // Set the burst time based on the selected speed
    int burst_time = CANAL_LENGTH / selected_boat.speed; // Example burst time calculation

    // Create the thread/boat with the selected speed and priority
    CEthread_create(&boats[boat_count], original_side, selected_boat.priority, selected_boat.speed, CANAL_LENGTH, 0, cross_channel, &boats[boat_count]);
    boats[boat_count].burst_time = burst_time; // Set burst time here
    boat_count++;

    printf("Creado barco %d: velocidad=%d, prioridad=%d, lado=%d\n", boat_count, selected_boat.speed, selected_boat.priority, original_side);
}

void cleanup_boats()
{
    if (boats != NULL)
    {
        free(boats);
        boats = NULL;
        printf("Boat list memory freed.\n");
    }
}

// Main program loop that handles the test
void start_threads()
{
    printf("XXXXXX");
    // Initialize the canal mutex
    CEmutex_init(&canal_mutex);

    // Seed random number generator
    srand(time(NULL));

    printf("XXXXXX");
    // Main loop to continuously check for key presses and run boats
    while (1)
    {
        // Check for key presses
        int key = kbhit();
        if (key)
        {
            create_boat(key, boat_quantity);
        }

        // Run all the boats sequentially
        for (int i = 0; i < boat_count; i++)
        {
            if (boats[i].state != DONE)
            {
                CEthread_execute(&boats[i]); // Each boat crosses the channel
                CEthread_join(&boats[i]);    // Wait for the boat to finish crossing
            }
        }

        // Allow some time to process other events
        usleep(100000); // 100 milliseconds
    }

    // Destroy the mutex after all threads are done
    CEmutex_destroy(&canal_mutex);
}
