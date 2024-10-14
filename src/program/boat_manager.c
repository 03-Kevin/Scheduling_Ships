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
int boat_count = 0;    // Current number of boats
int boat_quantity = 0; // Maximum number of boats
int canal_length = 0;  // Default initialization
int serial_port = -1;

int left_quantity = 0;
int right_quantity = 0;
CEthread *boat_queue_left = NULL;
CEthread *boat_queue_right = NULL;
int contadorLetrero = 0;

// Boat types with their respective speeds and priorities
BoatType boat_types[] = {
    {10, 2}, // Normal: speed 10, priority 2
    {20, 2}, // Pesquero: speed 20, priority 2
    {25, 1}  // Patrulla: speed 25, priority 1
};

CEmutex canal_mutex; // Mutex para controlar el acceso al canal

// Function to initialize the boat list with queue_quantity size
void initialize_boats_left(int queue_quantity)
{
    create_ready_queue_left();
    // Set the global boat quantity
    boat_quantity = queue_quantity;

    // Allocate memory for the boats
    boat_queue_left = malloc(queue_quantity * sizeof(CEthread));
    if (boat_queue_left == NULL)
    {
        printf("Error: Could not allocate memory for boats.\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    printf("Boat list initialized with capacity for %d boats.\n", queue_quantity);
}

void initialize_boats_right(int queue_quantity)
{
    create_ready_queue_right();
    // Set the global boat quantity
    boat_quantity = queue_quantity;

    // Allocate memory for the boats
    boat_queue_right = malloc(queue_quantity * sizeof(CEthread));
    if (boat_queue_right == NULL)
    {
        printf("Error: Could not allocate memory for boats.\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    printf("Boat list initialized with capacity for %d boats.\n", queue_quantity);
}

int quantum = 3;

// Cross channel function: locks the canal mutex and starts the crossing
// case de 3 casos: 1. no apropiativo - 2. rr - 3. tiempo real (revisar primero de la lista)
void cross_channel(void *arg)
{
    CEthread *barco = (CEthread *)arg;
    delete_led(barco->original_side);

    // Lock the canal mutex before crossing
    CEmutex_lock(&canal_mutex);
    printf("Barco %d ha bloqueado el canal. Empieza a cruzar con tiempo estimado: %d segundos.\n", barco->thread_id, barco->burst_time);

    // Diferenciar el comportamiento según el tipo de calendarización
    if (scheduling_type == 3) // Round Robin
    {
        // Round Robin: usar el quantum
        int time_to_cross = (join_flag == 1) ? barco->burst_time : ((barco->burst_time < quantum) ? barco->burst_time : quantum);

        for (int i = 0; i < time_to_cross; i++)
        {
            sleep(1);            // Simulate crossing for 1 second
            barco->burst_time--; // Decrement burst time
            contadorLetrero++;
            led_manager();
            printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
            crossing_led();
            
            if (barco->burst_time == 0)
            {
                break;
            }
        }

        if (barco->burst_time > 0)
        {
            printf("Barco %d no ha terminado de cruzar. Reprogramando...\n", barco->thread_id);
            if (barco->original_side == OCEANO_IZQ)
            {
                barco->arrival_time = arrival_counter_left++;
                barco->state = READY;
                enqueue_thread(barco, queue_left); // Lo volvemos a agregar al final de la cola
            }
            else
            {
                barco->arrival_time = arrival_counter_right++;
                barco->state = READY;
                enqueue_thread(barco, queue_right); // Lo volvemos a agregar al final de la cola
            }
        }
        else
        {
            printf("Barco %d ha cruzado el canal.\n", barco->thread_id);
            turn_off_crossing_led();
        }
    }
    else if (scheduling_type == 4) // SJF con interrupciones (preemptive)
    {
        if (barco->original_side == OCEANO_IZQ)
        {
            // SJF preemptive: verificar constantemente si hay un barco con menor burst_time
            while (barco->burst_time > 0)
            {
                // Antes de disminuir el burst_time, verifica si hay un barco en la cola con un burst_time menor.
                if (queue_left->count > 0)
                {
                    ReadyQueueNode *first_in_queue = queue_left->head;

                    // Si el burst_time del barco en la cola es menor, se intercambian.
                    if (first_in_queue->thread->burst_time < barco->burst_time)
                    {
                        printf("-----------\n");
                        printf("Intercambiando barco %d (tiempo restante %d) con barco %d (tiempo restante %d).\n",
                               barco->thread_id, barco->burst_time, first_in_queue->thread->thread_id, first_in_queue->thread->burst_time);
                        printf("-----------\n");
                        // Encola el barco actual para que espere su turno
                        barco->state = READY;
                        enqueue_thread(barco, queue_left);

                        // Dequeue el barco con menor burst_time y lo asigna como el que está ejecutándose
                        barco = dequeue_thread(queue_left);

                        printf("Barco %d ahora está cruzando el canal con tiempo restante: %d segundos.\n", barco->thread_id, barco->burst_time);
                    }
                }

                sleep(1);            // Simulate crossing for 1 second
                barco->burst_time--; // Decrement burst time
                contadorLetrero++;
                int key = kbhit();
                if (key)
                {
                    create_boat(key, boat_quantity);
                }
                printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
                crossing_led();
                led_manager();
            }
        }

        else if (barco->original_side == OCEANO_DER)
        {
            // SJF preemptive: verificar constantemente si hay un barco con menor burst_time
            while (barco->burst_time > 0)
            {
                // Antes de disminuir el burst_time, verifica si hay un barco en la cola con un burst_time menor.
                if (queue_right->count > 0)
                {
                    ReadyQueueNode *first_in_queue = queue_right->head;

                    // Si el burst_time del barco en la cola es menor, se intercambian.
                    if (first_in_queue->thread->burst_time < barco->burst_time)
                    {
                        printf("-----------\n");
                        printf("Intercambiando barco %d (tiempo restante %d) con barco %d (tiempo restante %d).\n",
                               barco->thread_id, barco->burst_time, first_in_queue->thread->thread_id, first_in_queue->thread->burst_time);
                        printf("-----------\n");
                        // Encola el barco actual para que espere su turno
                        barco->state = READY;
                        enqueue_thread(barco, queue_right);

                        // Dequeue el barco con menor burst_time y lo asigna como el que está ejecutándose
                        barco = dequeue_thread(queue_right);

                        printf("Barco %d ahora está cruzando el canal con tiempo restante: %d segundos.\n", barco->thread_id, barco->burst_time);

                    }
                }

                sleep(1);            // Simulate crossing for 1 second
                barco->burst_time--; // Decrement burst time
                contadorLetrero++;

                printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
                crossing_led();
                led_manager();
            }
        }
        printf("Barco %d ha cruzado el canal.\n", barco->thread_id);
    }

    else
    {
        // Decrement burst time in a loop until it reaches 0
        while (barco->burst_time > 0)
        {
            sleep(1);            // Simulate crossing for 1 second
            barco->burst_time--; // Decrement burst time
            contadorLetrero++;
            crossing_led();
            led_manager();
            printf("Barco %d tiene %d segundos restantes para cruzar.\n", barco->thread_id, barco->burst_time);
        }
    }

    printf("Barco %d ha cruzado el canal.\n", barco->thread_id);
    turn_off_crossing_led();

    // Unlock the canal mutex after crossing
    CEmutex_unlock(&canal_mutex);
}

void add_boats_from_menu(int normal_left, int fishing_left, int patrol_left,
                         int normal_right, int fishing_right, int patrol_right,
                         int canal_length, int queue_quantity)
{
    int total_to_add_left = normal_left + fishing_left + patrol_left;
    int total_to_add_right = normal_right + fishing_right + patrol_right;

    left_quantity = total_to_add_left;
    right_quantity = total_to_add_right;

    //carga_arduino(total_to_add_left,total_to_add_right);

    // Check if adding the boats would exceed the total capacity
    if (boat_count + total_to_add_left + total_to_add_right > queue_quantity)
    {
        printf("Cannot create more boats. Maximum capacity reached.\n");
        return;
    }
    if (boat_queue_left != NULL && boat_queue_right != NULL)
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
        CEthread_create_batch(boat_queue_left, boat_count, normal_left, 10, canal_length, 0, 2, OCEANO_IZQ, cross_channel, NULL);
        boat_count += normal_left;
    }

    if (fishing_left > 0)
    {
        CEthread_create_batch(boat_queue_left, boat_count, boat_count + fishing_left, 20, canal_length, 0, 2, OCEANO_IZQ, cross_channel, NULL);
        boat_count += fishing_left;
    }

    if (patrol_left > 0)
    {
        CEthread_create_batch(boat_queue_left, boat_count, patrol_left + boat_count, 25, canal_length, 0, 1, OCEANO_IZQ, cross_channel, NULL);
        boat_count += patrol_left;
    }

    // Create boats for the right side
    if (normal_right > 0)
    {
        CEthread_create_batch(boat_queue_right, boat_count, normal_right + boat_count, 10, canal_length, 0, 2, OCEANO_DER, cross_channel, NULL);
        boat_count += normal_right;
    }

    if (fishing_right > 0)
    {
        CEthread_create_batch(boat_queue_right, boat_count, fishing_right + boat_count, 20, canal_length, 0, 2, OCEANO_DER, cross_channel, NULL);
        boat_count += fishing_right;
    }

    if (patrol_right > 0)
    {
        CEthread_create_batch(boat_queue_right, boat_count, patrol_right + boat_count, 25, canal_length, 0, 1, OCEANO_DER, cross_channel, NULL);
        boat_count += patrol_right;
    }

    printf("%d boats added to the list. Total count: %d\n", total_to_add_right + total_to_add_left, boat_count);
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

    int burst_time;

    switch (key)
    {
    case 'q':
        selected_boat = boat_types[0]; // Normal
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_right[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_right[boat_count]);
        boat_count++;
        break;
    case 'w':
        selected_boat = boat_types[1]; // Pesquero
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_right[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_right[boat_count]);
        boat_count++;
        break;
    case 'e':
        selected_boat = boat_types[2]; // Patrulla
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_right[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_right[boat_count]);
        boat_count++;
        break;
    case 'r':
        selected_boat = boat_types[0]; // Normal
        original_side = OCEANO_IZQ;
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_left[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_left[boat_count]);
        boat_count++;
        break;
    case 't':
        selected_boat = boat_types[1]; // Pesquero
        original_side = OCEANO_IZQ;
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_left[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_left[boat_count]);
        boat_count++;
        break;
    case 'y':
        selected_boat = boat_types[2]; // Patrulla
        original_side = OCEANO_IZQ;
        // Set the burst time based on the selected speed
        burst_time = canal_length / selected_boat.speed; // Example burst time calculation

        // Create the thread/boat with the selected speed and priority
        CEthread_create(&boat_queue_left[boat_count], original_side, selected_boat.priority, selected_boat.speed, canal_length, 0, cross_channel, &boat_queue_left[boat_count]);
        boat_count++;
        break;
    default:
        return; // Invalid key
    }

    printf("Creado barco %d: velocidad=%d, prioridad=%d, lado=%d\n", boat_count, selected_boat.speed, selected_boat.priority, original_side);
}

void cleanup_boats()
{
    if (boat_queue_left != NULL || boat_queue_right != NULL)
    {
        free(boat_queue_left);
        free(boat_queue_right);
        boat_queue_left = NULL;
        boat_queue_right = NULL;
        printf("Boat list memory freed.\n");
    }
}


///////////// ARDUINO ////////////////


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

void delete_led(int original_side) {

    if(original_side == OCEANO_DER){
        // Enviar el número '1' al Arduino
        char signal = 'd';
        write(serial_port, &signal, sizeof(signal));
        printf("Se ha quitado barco del oceano DERECHO\n");
    }

    if(original_side == OCEANO_IZQ){
        // Enviar el número '1' al Arduino
        char signal = 'i';
        write(serial_port, &signal, sizeof(signal));
        printf("Se ha quitado barco del oceano IZQUIERDO\n");
    }


}

void led_manager() {
    int key = kbhit();
    if(key){
        write(serial_port, &key, 1);
        printf("Tecla presionada para el Arduino");
        create_boat(key, boat_quantity);
    }
}

void crossing_led() {
    if (serial_port == -1) {
        printf("El puerto serial no está abierto.\n");
        return;
    }

    char signal = 'c';
    write(serial_port, &signal, sizeof(signal));
}

void turn_off_crossing_led(){
    if (serial_port == -1) {
        printf("El puerto serial no está abierto.\n");
        return;
    }

    char signal = 'o';
    write(serial_port, &signal, sizeof(signal));

}

void carga_arduino_right(int total_right){
    printf("El total de barcos a la derecha es: %d\n", total_right);
    char right_quantity[30];

    // Enviar la cantidad de arcos a la derecha
    sprintf(right_quantity, "R%d\n", total_right);
    printf("CANTIDAD DE BARCOS ENVIADOS: %s\n", right_quantity);
    usleep(500000);  // Ajustar si es necesario darle más tiempo al Arduino
    write(serial_port, right_quantity, strlen(right_quantity));  // Sin & aquí
}

void carga_arduino_left(int total_left) {
    printf("El total de barcos a la izquierda es: %d\n", total_left);
    
    // Enviar la cantidad de barcos a la izquierda
    char left_quantity[30];

    sprintf(left_quantity, "L%d\n", total_left);
    printf("CANTIDAD DE BARCOS ENVIADOS: %s\n", left_quantity);
    usleep(500000); 
    write(serial_port, left_quantity, strlen(left_quantity));  // Sin &
}


// Main program loop that handles the test
void start_threads(int flow_control_method, int w, int change_direction_period)
{
    arduino_init();
    // Seed random number generator
    srand(time(NULL));
    usleep(2000000);
    carga_arduino_right(right_quantity);
    usleep(2000000);
    carga_arduino_left(left_quantity);
   
    while (1)
    {
        // Check for key presses to add new boats
        int key = kbhit();
        if (key)
        {
            create_boat(key, boat_quantity);
        }

        if (flow_control_method == 1)
        { // Modo EQUIDAD
            char signal = '7';
            write(serial_port, &signal, sizeof(signal));

            int contador = 0;
            int flag = 0;
            printf("-----Left Queue-----\n");
            print_ready_queue(queue_left);
            printf("-----Right Queue-----\n");
            print_ready_queue(queue_right);
            printf("-----------\n");
            while (queue_left->count > 0 || queue_right->count > 0)
            {
                if (flag == 0)
                {
                    // Dequeue and execute the first thread
                    if (contador < w)
                    {
                        if (queue_left->count > 0)
                        {
                            CEthread *thread = dequeue_thread(queue_left); // Dequeue the first thread
                            if (thread != NULL)
                            { // Check if the thread is valid
                                if (thread->priority == 1)
                                {
                                    CEthread_join(thread); // Use CEthread_join for priority 1
                                }
                                else
                                {
                                    CEthread_execute(thread); // Execute the thread
                                }

                                CEthread_end(thread);
                                contador++;
                                printf("contador: %d\n", contador);
                                printf("-----Left Queue-----\n");
                                print_ready_queue(queue_left);
                                printf("-----Right Queue-----\n");
                                print_ready_queue(queue_right);
                                printf("-----------\n");
                            }
                        }
                        else
                        {
                            contador++;
                        }
                    }
                    else
                    {
                        contador = 0;
                        flag = 1;
                    }
                }
                else
                {
                    if (contador < w)
                    {
                        if (queue_right->count > 0)
                        {
                            CEthread *thread = dequeue_thread(queue_right); // Dequeue the first thread
                            if (thread != NULL)
                            { // Check if the thread is valid
                                if (thread->priority == 1)
                                {
                                    CEthread_join(thread); // Use CEthread_join for priority 1
                                }
                                else
                                {
                                    CEthread_execute(thread); // Execute the thread
                                }

                                CEthread_end(thread);
                                printf("-----Left Queue-----\n");
                                print_ready_queue(queue_left);
                                printf("-----Right Queue-----\n");
                                print_ready_queue(queue_right);
                                printf("-----------\n");
                                contador++;
                            }
                        }
                        else
                        {
                            contador++;
                        }
                    }
                    else
                    {
                        contador = 0;
                        flag = 0;
                    }
                }

                // Check for key presses again after executing a thread
                key = kbhit();
                if (key)
                {
                    create_boat(key, boat_quantity);
                }
            }
        }

        else if (flow_control_method == 2)
        { // Modo LETRERO
            char signal = '8';
            write(serial_port, &signal, sizeof(signal));
            int flag = 0;
            printf("-----Left Queue-----\n");
            print_ready_queue(queue_left);
            printf("-----Right Queue-----\n");
            print_ready_queue(queue_right);
            printf("-----------\n");
            while (queue_left->count > 0 || queue_right->count > 0)
            {
                printf("tiempo restante antes de cambiar el letrero: %d\n", change_direction_period - contadorLetrero);
                if (flag == 0)
                {
                    // Dequeue and execute the first thread
                    if (contadorLetrero < change_direction_period)
                    {
                        if (queue_left->count > 0)
                        {
                            CEthread *thread = dequeue_thread(queue_left); // Dequeue the first thread
                            if (thread != NULL)
                            {
                                if (thread->priority == 1)
                                {
                                    CEthread_join(thread); // Use CEthread_join for priority 1
                                }
                                else
                                {
                                    CEthread_execute(thread); // Execute the thread
                                }

                                CEthread_end(thread);

                                printf("tiempo transcurrido: %d\n", contadorLetrero);
                                printf("tiempo restante antes de cambiar el letrero: %d\n", change_direction_period - contadorLetrero);
                                printf("-----Left Queue-----\n");
                                print_ready_queue(queue_left);
                                printf("-----Right Queue-----\n");
                                print_ready_queue(queue_right);
                                printf("-----------\n");
                            }
                        }
                        else
                        {
                            contadorLetrero++;
                        }
                    }
                    else
                    {
                        contadorLetrero = 0;
                        flag = 1;
                    }
                }
                else
                {
                    if (contadorLetrero < change_direction_period)
                    {
                        if (queue_right->count > 0)
                        {
                            CEthread *thread = dequeue_thread(queue_right); // Dequeue the first thread
                            if (thread != NULL)
                            { // Check if the thread is valid

                                if (thread->priority == 1)
                                {
                                    CEthread_join(thread); // Use CEthread_join for priority 1
                                }
                                else
                                {
                                    CEthread_execute(thread); // Execute the thread
                                }

                                CEthread_end(thread);
                                printf("tiempo transcurrido: %d\n", contadorLetrero);
                                printf("tiempo restante antes de cambiar el letrero: %d\n", change_direction_period - contadorLetrero);
                                printf("-----Left Queue-----\n");
                                print_ready_queue(queue_left);
                                printf("-----Right Queue-----\n");
                                print_ready_queue(queue_right);
                                printf("-----------\n");
                            }
                        }
                        else
                        {
                            contadorLetrero++;
                        }
                    }
                    else
                    {
                        contadorLetrero = 0;
                        flag = 0;
                    }
                }

                // Allow some time for new boats to be added
                usleep(100000); // 100 milliseconds

                // Check for key presses again after executing a thread
                key = kbhit();
                if (key)
                {
                    create_boat(key, boat_quantity);
                }
            }
        }

        else if (flow_control_method == 3)
        { // Modo Tico
            // Process boats from the queue if available
            char signal = '9';
            write(serial_port, &signal, sizeof(signal));
            while (queue_left->count > 0 || queue_right->count > 0)
            {
                printf("-----Left Queue-----\n");
                print_ready_queue(queue_left);
                printf("-----Right Queue-----\n");
                print_ready_queue(queue_right);
                printf("-----------\n");
                // Dequeue and execute the first thread
                if (queue_left->count > 0)
                {
                    CEthread *thread = dequeue_thread(queue_left); // Dequeue the first thread
                    if (thread != NULL)
                    { // Check if the thread is valid
                        if (thread->priority == 1)
                        {
                            CEthread_join(thread); // Use CEthread_join for priority 1
                        }
                        else
                        {
                            CEthread_execute(thread); // Execute the thread
                        }

                        CEthread_end(thread);
                    }
                }
                if (queue_right->count > 0)
                {
                    CEthread *thread = dequeue_thread(queue_right); // Dequeue the first thread
                    if (thread != NULL)
                    { // Check if the thread is valid
                        if (thread->priority == 1)
                        {
                            CEthread_join(thread); // Use CEthread_join for priority 1
                        }
                        else
                        {
                            CEthread_execute(thread); // Execute the thread
                        }

                        CEthread_end(thread);
                    }
                }

                // Allow some time for new boats to be added
                usleep(100000); // 100 milliseconds

                // Check for key presses again after executing a thread
                key = kbhit();
                if (key)
                {
                    create_boat(key, boat_quantity);
                }
            }
        }

        else if (flow_control_method == 2)
        {
        }

        // Allow some time to process other events before checking the queue again
        usleep(100000); // 100 milliseconds
    }
}