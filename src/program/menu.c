// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ce_threads/ce_thread.h"
#include "boat_manager.h"
#include "../calendar/calendarizador.h"

// GLOBAL VARIABLES
int global_normal_left;   // Global variable of number of normal boats in the left-side of the ocean
int global_fishing_left;  // Global variable of number of fishing boats in the left-side of the ocean
int global_patrols_left;  // Global variable of number of patrols boats in the left-side of the ocean
int global_normal_right;  // Global variable of number of normal boats in the right-side of the ocean
int global_fishing_right; // Global variable of number of fishing boats in the right-side of the ocean
int global_patrols_right; // Global variable of number of patrol boats in the right-side of the ocean

// Define a structure to store the configuration values
typedef struct
{
    int flow_control_method;
    int length;
    int queue_quantity;
    int direction_change_period;
    int w;
    int scheduling_method;
    int all_values_present; // Flag to indicate whether all values were successfully read
} Config;

// Make config a global variable
Config config; // Declare it here

// Function for obtaining selection from user on the scheduling method
void main_program()
{
    printf("Iniciando prueba del sistema...\n");

    // Run the tests (boats crossing the channel)
    start_threads(config.flow_control_method, config.w, config.direction_change_period);

    printf("Prueba del sistema finalizada.\n");
}

// Function for cleaning input buffer
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Discard all characters until newline
}

// Function to load configuration from file
Config loadConfig(const char *filename)
{
    printf("Welcome to Scheduling Ships V1.0");
    printf("\nChecking the configuration file\n");
    FILE *file = fopen(filename, "r");
    Config loaded_config = {-1, -1, -1, -1, -1, -1, 1}; // Initialize with default "missing" values

    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        loaded_config.all_values_present = 0;
        return loaded_config;
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Parse each line and store the values in the config structure
        if (sscanf(line, "flow_control_method: %d", &loaded_config.flow_control_method) == 1)
            continue;
        if (sscanf(line, "length: %d", &loaded_config.length) == 1)
            continue;
        if (sscanf(line, "queue_quantity: %d", &loaded_config.queue_quantity) == 1)
            continue;
        if (sscanf(line, "direction_change_period: %d", &loaded_config.direction_change_period) == 1)
            continue;
        if (sscanf(line, "w: %d", &loaded_config.w) == 1)
            continue;
        if (sscanf(line, "scheduling_method: %d", &loaded_config.scheduling_method) == 1)
            continue;
    }

    fclose(file);

    // Check if any required values are still missing (-1 means missing)
    if (loaded_config.flow_control_method == -1 || loaded_config.length == -1 || loaded_config.queue_quantity == -1 ||
        loaded_config.direction_change_period == -1 || loaded_config.w == -1 || loaded_config.scheduling_method == -1)
    {
        loaded_config.all_values_present = 0;
    }

    return loaded_config;
}

// Function to check if all config values are present and flag missing ones
void checkMissingValues(Config config)
{
    if (!config.all_values_present)
    {
        printf("\n--- --- --- --- --- --- ---\n");
        printf("Warning: Missing configuration values:\n");
        if (config.flow_control_method == -1)
            printf("- flow_control_method is missing\n");
        if (config.length == -1)
            printf("- length is missing\n");
        if (config.queue_quantity == -1)
            printf("- queue_quantity is missing\n");
        if (config.direction_change_period == -1)
            printf("- direction_change_period is missing\n");
        if (config.w == -1)
            printf("- w is missing\n");
        if (config.scheduling_method == -1)
            printf("- scheduling_method is missing\n");
    }
    else
    {
        initialize_boats_left(config.queue_quantity);
        initialize_boats_right(config.queue_quantity);
        printf("All configuration values loaded successfully.\n");
    }
}

// Function for confirming the number of boats
int confirmBoatCounts()
{
    char response[10]; // Buffer to store the response

    // Calculate total number of boats
    int total_boats = global_normal_left + global_fishing_left + global_patrols_left +
                      global_normal_right + global_fishing_right + global_patrols_right;

    // Check if the total number of boats exceeds the queue quantity
    if (total_boats > config.queue_quantity) // Use the global config variable
    {
        printf("\n--- --- --- --- --- --- ---\n");
        printf("Warning: Total number of boats (%d) exceeds the queue quantity (%d).\n", total_boats, config.queue_quantity);
        printf("Please adjust the numbers before proceeding.\n");
        return 0; // Return 0 indicating the confirmation failed
    }

    // Display current counts
    printf("\nCantidad de barcos normales a la izquierda: %d\n", global_normal_left);
    printf("Cantidad de barcos pesqueros a la izquierda: %d\n", global_fishing_left);
    printf("Cantidad de barcos patrulleros a la izquierda: %d\n", global_patrols_left);
    printf("Cantidad de barcos normales a la derecha: %d\n", global_normal_right);
    printf("Cantidad de barcos pesqueros a la derecha: %d\n", global_fishing_right);
    printf("Cantidad de barcos patrulleros a la derecha: %d\n", global_patrols_right);

    // Confirmation prompt
    printf("¿Es correcto?:\n");
    printf("1. Si\n");
    printf("2. No\n");

    // Clear input buffer
    clearInputBuffer();

    // Read the response
    fgets(response, sizeof(response), stdin);
    clearInputBuffer(); // Clear the buffer after reading

    // Remove newline character from the response
    response[strcspn(response, "\n")] = 0;

    // Convert the input to an integer
    int answer = atoi(response);

    // Handle the user's choice
    switch (answer)
    {
    case 1:
        return 1; // Confirm the counts (true)
    case 2:
        return 0; // Reject the counts (false)
    default:
        printf("Opción inválida. Debe ingresar 1 para sí o 2 para no.\n");
        return confirmBoatCounts(); // Retry for valid input
    }
}

// Function for obtaining preset load of boats from user
void preset_load()
{
    printf("\n--- --- --- --- --- --- ---\n");
    printf("Bienvenido a la configuración inicial de barcos!\n");

    // Ask for boat quantities
    printf("Indique la cantidad de barcos normales que desea de lado izquierdo: ");
    scanf("%d", &global_normal_left);
    printf("Indique la cantidad de barcos pesqueros que desea de lado izquierdo: ");
    scanf("%d", &global_fishing_left);
    printf("Indique la cantidad de barcos patrulleros que desea de lado izquierdo: ");
    scanf("%d", &global_patrols_left);
    printf("Indique la cantidad de barcos normales que desea de lado derecho: ");
    scanf("%d", &global_normal_right);
    printf("Indique la cantidad de barcos pesqueros que desea de lado derecho: ");
    scanf("%d", &global_fishing_right);
    printf("Indique la cantidad de barcos patrulleros que desea de lado derecho: ");
    scanf("%d", &global_patrols_right);

    // Confirm the entered values
    if (confirmBoatCounts())
    {
        printf("Valores confirmados.\n");

                add_boats_from_menu(global_normal_left, global_fishing_left, global_patrols_left,
                            global_normal_right, global_fishing_right, global_patrols_right,
                            canal_length, config.queue_quantity);

        // Proceed to the main program logic
        main_program();
    }
    else
    {
        printf("Reingrese la configuración de barcos.\n");
        preset_load(); // Restart the preset load function to re-enter values
    }
}

// Function for displaying initial Menu options
void displayMenu()
{
    printf("\n--- Menu ---\n");
    printf("¿Desea generar una carga preestablecida?: \n");
    printf("1. Si\n");
    printf("2. No\n");
    printf("3. Salir\n");
    printf("\n--- --- --- --- --- --- ---\n");
}

// Function for obtaining preset load of boats from user
int main()
{
    int choice;

    const char *filename = "config.txt"; // Specify your configuration file here
    config = loadConfig(filename);       // Load config into the global variable

    // Check for missing values
    checkMissingValues(config);

    // Proceed with the rest of your program if all values are present
    if (config.all_values_present)
    {
        // Program logic goes here using the loaded config values
        printf("\nConfiguration values:\n");
        printf("Flow Control Method: %d\n", config.flow_control_method);
        printf("Length: %d\n", config.length);
        printf("Queue Quantity: %d\n", config.queue_quantity);
        printf("Direction Change Period: %d\n", config.direction_change_period);
        printf("W: %d\n", config.w);
        printf("Scheduling Method: %d\n", config.scheduling_method);
        scheduling_type = config.scheduling_method;
        canal_length = config.length;
    }
    else
    {
        printf("Please fix the missing values in the configuration file.\n");
    }

    while (1)
    {
        displayMenu();

        // Check if scanf correctly read an integer
        if (scanf("%d", &choice) != 1)
        {
            printf("Opción inválida. Intente digitar uno de los números mostrados en el menú.\n");
            clearInputBuffer(); // Clear invalid input from buffer
            continue;           // Prompt the user again
        }

        // Handle the user's choice
        switch (choice)
        {
        case 1:
            preset_load();
            exit(0); // Exit the program
        case 2:
            main_program();
            exit(0); // Exit the program
        case 3:
            printf("Saliendo del program. Hasta luego!\n");
            exit(0); // Exit the program
        default:
            printf("Opción fuera del menú. Intente digitar uno de los números mostrados en el menú.\n");
        }
    }

    return 0;
}
