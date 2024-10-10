#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include "../ce_threads/ce_thread.h" // Include CEthread functions
#include "../ce_mutex/ce_mutex.h"    // Include CEmutex functions

#define NUM_BARCO 100 // Adjust size for maximum boats
#define CANAL_LENGTH 100

CEmutex test_canal_mutex; // Mutex to control access to the canal

// Boat types with their respective speeds and priorities
typedef struct
{
    int speed;
    int priority;
} BoatType;

BoatType boat_types[] = {
    {10, 3}, // Normal: speed 10, priority 3
    {20, 2}, // Pesquero: speed 20, priority 2
    {25, 1}  // Patrulla: speed 25, priority 1
};

// Array to hold created boats
CEthread barcos[NUM_BARCO];
int boat_count = 0; // Current number of boats

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
    CEmutex_lock(&test_canal_mutex);
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
    CEmutex_unlock(&test_canal_mutex);
}

// Function to create a boat based on key press
void create_boat(char key)
{
    if (boat_count >= NUM_BARCO)
    {
        printf("No se pueden crear más barcos.\n");
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
    CEthread_create(&barcos[boat_count], original_side, selected_boat.priority, selected_boat.speed, CANAL_LENGTH, 0, cross_channel, &barcos[boat_count]);
    barcos[boat_count].burst_time = burst_time; // Set burst time here
    boat_count++;

    printf("Creado barco %d: velocidad=%d, prioridad=%d, lado=%d\n", boat_count, selected_boat.speed, selected_boat.priority, original_side);
}

// Main program loop that handles the test
void test_cross_channel()
{
    // Initialize the canal mutex
    CEmutex_init(&test_canal_mutex);

    // Seed random number generator
    srand(time(NULL));

    // Create initial boats
    for (int i = 0; i < 2; i++)
    {
        create_boat('q'); // Create normal boat as an example
    }

    // Main loop to continuously check for key presses and run boats
    while (1)
    {

        // Check for key presses
        int key = kbhit();
        if (key)
        {
            create_boat(key);
        }

        // Run all the boats sequentially
        for (int i = 0; i < boat_count; i++)
        {
            if (barcos[i].state != DONE)
            {
                CEthread_execute(&barcos[i]); // Each boat crosses the channel
                CEthread_join(&barcos[i]);    // Wait for the boat to finish crossing
            }
        }

        // Allow some time to process other events
        usleep(100000); // 100 milliseconds
    }

    // Destroy the mutex after all threads are done
    CEmutex_destroy(&test_canal_mutex);
}

int main()
{
    printf("Iniciando prueba de creación de barcos...\n");

    // Run the tests (boats crossing the channel)
    test_cross_channel();

    printf("Prueba finalizada.\n");
    return 0;
}
