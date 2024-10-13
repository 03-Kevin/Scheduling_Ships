// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include "boat_manager.h" // Include the boat manager header
#include "../ce_threads/ce_thread.h"
#include "../calendar/calendarizador.h"

// Global variables
int boat_count = 1;    // Current number of boats
int boat_quantity = 0; // Maximum number of boats
int canal_length = 0;  // Default initialization
int serial_port = -1; //
int boat_count_arduino = 0;
CEthread *boat_queue = NULL;

// Boat types with their respective speeds and priorities
BoatType boat_types[] = {
    {10, 2}, // Normal: speed 10, priority 2
    {20, 2}, // Pesquero: speed 20, priority 2
    {25, 1}  // Patrulla: speed 25, priority 1
};

CEmutex canal_mutex; // Mutex para controlar el acceso al canal

// Function to initialize the boat list with queue_quantity size
void initialize_boats(int queue_quantity)
{
    create_ready_queue();
    // Set the global boat quantity
    boat_quantity = queue_quantity;

    // Allocate memory for the boats
    boat_queue = malloc(queue_quantity * sizeof(CEthread));
    if (boat_queue == NULL)
    {
        printf("Error: Could not allocate memory for boats.\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    printf("Boat list initialized with capacity for %d boats.\n", queue_quantity);
}

// Cross channel function: locks the canal mutex and starts the crossing
// case de 3 casos: 1. no apropiativo - 2. rr - 3. tiempo real (revisar primero de la lista)

int quantum = 4;       // Quantum para Round Robin

void cross_channel(void *arg)
{
    CEthread *barco = (CEthread *)arg;

    // Lock the canal mutex before crossing
    CEmutex_lock(&canal_mutex);
    printf("Barco %d ha bloqueado el canal. Empieza a cruzar con tiempo estimado: %d segundos.\n", barco->thread_id, barco->burst_time);
    boat_count_arduino--;

    // Diferenciar el comportamiento según el tipo de calendarización
    if (scheduling_type == 3) // Round Robin
    {
        // Round Robin: usar el quantum
        int time_to_cross = (barco->burst_time < quantum) ? barco->burst_time : quantum;
        for (int i = 0; i < time_to_cross; i++)
        {
            sleep(1);            // Simulate crossing for 1 second
            barco->burst_time--; // Decrement burst time
            printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
            send_boat_count_to_arduino(boat_count_arduino);

            if (barco->burst_time == 0)
            {
                break;
            }
        }

        if (barco->burst_time > 0)
        {
            printf("Barco %d no ha terminado de cruzar. Reprogramando...\n", barco->thread_id);
            enqueue_thread(barco); // Lo volvemos a agregar al final de la cola
        }
        else
        {
            printf("Barco %d ha cruzado el canal.\n", barco->thread_id);

        }
    }
    else if (scheduling_type == 4) // SJF con interrupciones (preemptive)
    {
        // SJF preemptive: verificar constantemente si hay un barco con menor burst_time
        while (barco->burst_time > 0)
        {
            // Antes de disminuir el burst_time, verifica si hay un barco en la cola con un burst_time menor.
            if (queue->count > 0)
            {
                ReadyQueueNode *first_in_queue = queue->head;

                // Si el burst_time del barco en la cola es menor, se intercambian.
                if (first_in_queue->thread->burst_time < barco->burst_time)
                {
                    printf("Intercambiando barco %d (tiempo restante %d) con barco %d (tiempo restante %d).\n",
                           barco->thread_id, barco->burst_time, first_in_queue->thread->thread_id, first_in_queue->thread->burst_time);

                    // Encola el barco actual para que espere su turno
                    enqueue_thread(barco);

                    // Dequeue el barco con menor burst_time y lo asigna como el que está ejecutándose
                    barco = dequeue_thread();

                    printf("Barco %d ahora está cruzando el canal con tiempo restante: %d segundos.\n", barco->thread_id, barco->burst_time);
                }
            }

            sleep(1);            // Simulate crossing for 1 second
            barco->burst_time--; // Decrement burst time
            printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
        }
        printf("Barco %d ha cruzado el canal.\n", barco->thread_id);
    }
    else
    {
        // Otros calendarizadores (FCFS, SJF sin interrupciones, Prioridad)
        while (barco->burst_time > 0)
        {
            sleep(1);            // Simulate crossing for 1 second
            barco->burst_time--; // Decrement burst time
            printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
        }
        printf("Barco %d ha cruzado el canal.\n", barco->thread_id);
    }

    // Unlock the canal mutex after crossing
    CEmutex_unlock(&canal_mutex);
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
    if (boat_queue != NULL)
    {
        printf("Memory is allocated for boats.\n");
    }
    else
    {
        printf("Memory is not allocated for boats.\n");
    }
    // Create boats for the left side
    if (normal_left > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, normal_left, 10, canal_length, 0, 2, OCEANO_IZQ, cross_channel, NULL);
        boat_count += normal_left;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
    }

    if (fishing_left > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, boat_count + fishing_left, 20, canal_length, 0, 2, OCEANO_IZQ, cross_channel, NULL);
        boat_count += fishing_left;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
    }

    if (patrol_left > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, patrol_left + boat_count, 25, canal_length, 0, 1, OCEANO_IZQ, cross_channel, NULL);
        boat_count += patrol_left;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
    }

    // Create boats for the right side
    if (normal_right > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, normal_right + boat_count, 10, canal_length, 0, 2, OCEANO_DER, cross_channel, NULL);
        boat_count += normal_right;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
    }

    if (fishing_right > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, fishing_right + boat_count, 20, canal_length, 0, 2, OCEANO_DER, cross_channel, NULL);
        boat_count += fishing_right;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
    }

    if (patrol_right > 0)
    {
        CEthread_create_batch(boat_queue, boat_count, patrol_right + boat_count, 25, canal_length, 0, 1, OCEANO_DER, cross_channel, NULL);
        boat_count += patrol_right;
        printf("BOAT COUNT AFTER NORMAL LEFT: %d.\n", boat_count);
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
    int burst_time = canal_length / selected_boat.speed; // Example burst time calculation


    // Create the thread/boat with the selected speed and priority
    CEthread_create(&boat_queue[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue[boat_count]);
    boat_count++;


    printf("Creado barco %d: velocidad=%d, prioridad=%d, lado=%d\n", boat_count, selected_boat.speed, selected_boat.priority, original_side);
}

void cleanup_boats()
{
    if (boat_queue != NULL)
    {
        free(boat_queue);
        boat_queue = NULL;
        printf("Boat list memory freed.\n");
    }
}

// Function to send the number of boats to the Arduino
void send_boat_count_to_arduino(int boat_count) {
    if (serial_port == -1) {
        printf("El puerto serial no está abierto.\n");
        return;
    }

    char buffer[2];
    snprintf(buffer, sizeof(buffer), "%d", boat_count); // Convertir número de barcos a cadena
    write(serial_port, buffer, sizeof(buffer));         // Enviar la cadena por el puerto serial
    printf("Enviando número de barcos: %d al Arduino\n", boat_count);
}

// Function to open and configure the serial port
void arduino_init() {
    serial_port = open("/dev/ttyUSB0", O_RDWR);
    
    if (serial_port < 0) {
        printf("Error al abrir el puerto serie.\n");
        return;
    }

    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error configurando el puerto serie.\n");
        close(serial_port);
        serial_port = -1;
        return;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD); // Activar lectura y ajustar línea para que no cuelgue
    tty.c_cflag &= ~PARENB;          // No paridad
    tty.c_cflag &= ~CSTOPB;          // 1 bit de parada
    tty.c_cflag &= ~CSIZE;           // Limpiar los bits de tamaño de datos
    tty.c_cflag |= CS8;              // 8 bits de datos

    tty.c_lflag &= ~ICANON;          // Modo no canónico
    tty.c_lflag &= ~ECHO;            // No eco de los datos leídos
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ISIG;

    tty.c_oflag &= ~OPOST; // Modo de salida bruta

    // Aplicar configuraciones
    tcsetattr(serial_port, TCSANOW, &tty);
}


// Main program loop that handles the test
void start_threads()
{
    arduino_init();
    // Seed random number generator
    srand(time(NULL));

    while (1)
    {
        // Check for key presses to add new boats
        int key = kbhit();
        if (key)
        {
            boat_count_arduino++;
            send_boat_count_to_arduino(boat_count_arduino);
            create_boat(key, boat_quantity);
        }

        // Process boats from the queue if available
        while (queue->count > 0)
        {
            // Enviar actualización al Arduino incluso mientras el barco cruza
            send_boat_count_to_arduino(boat_count_arduino);
            print_ready_queue();
            // Dequeue and execute the first thread
            CEthread *thread = dequeue_thread(); // Dequeue the first thread
            if (thread != NULL)
            {
                if (scheduling_type == 3)
                {
                    // Enviar actualización al Arduino incluso mientras el barco cruza
                    send_boat_count_to_arduino(boat_count_arduino);

                    // Round Robin: ejecutar con quantum
                    cross_channel(thread);

                    // Si el barco terminó de cruzar, finalizarlo
                    if (thread->burst_time == 0)
                    {
                        send_boat_count_to_arduino(boat_count_arduino);  // Actualizar LEDs
                        CEthread_end(thread);
                    }
                }
                else
                {
                    // Otros calendarizadores: FCFS, SJF, Prioridad
                    cross_channel(thread);
                    CEthread_end(thread); // Siempre finalizar al completar el cruce
                }
            }

            // Allow some time for new boats to be added
            usleep(100000); // 100 milliseconds

            // Check for key presses again after executing a thread
            key = kbhit();
            if (key)
            {
                boat_count_arduino++;
                send_boat_count_to_arduino(boat_count_arduino);
                create_boat(key, boat_quantity);
            }
        }

        // Allow some time to process other events before checking the queue again
        usleep(100000); // 100 milliseconds
    }
}
