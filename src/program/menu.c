// LIBRARIES
#include <stdio.h>
#include <stdlib.h>

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

// Function to load configuration from file
// I: char filename - filename/path of the file used.
// O: Config - struct for configuration parameters of the channel
// R: Only numbers will be taken from the user
Config loadConfig(const char *filename)
{
    printf("Welcome to Scheduling Ships V1.0");
    printf("\nChecking the configutation file\n");
    FILE *file = fopen(filename, "r");
    Config config = {-1, -1, -1, -1, -1, -1, 1}; // Initialize with default "missing" values

    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        config.all_values_present = 0;
        return config;
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Parse each line and store the values in the config structure
        if (sscanf(line, "flow_control_method: %d", &config.flow_control_method) == 1)
            continue;
        if (sscanf(line, "length: %d", &config.length) == 1)
            continue;
        if (sscanf(line, "queue_quantity: %d", &config.queue_quantity) == 1)
            continue;
        if (sscanf(line, "direction_change_period: %d", &config.direction_change_period) == 1)
            continue;
        if (sscanf(line, "w: %d", &config.w) == 1)
            continue;
        if (sscanf(line, "scheduling_method: %d", &config.scheduling_method) == 1)
            continue;
    }

    fclose(file);

    // Check if any required values are still missing (-1 means missing)
    if (config.flow_control_method == -1 || config.length == -1 || config.queue_quantity == -1 ||
        config.direction_change_period == -1 || config.w == -1 || config.scheduling_method == -1)
    {
        config.all_values_present = 0;
    }

    return config;
}

// Function to check if all config values are present and flag missing ones
// I: Config - struct for configuration parameters of the channel
// O: No output
// R: No letters, only numbers
void checkMissingValues(Config config)
{
    if (!config.all_values_present)
    {
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
        printf("All configuration values loaded successfully.\n");
    }
}

// Function for obtaining selection from user on the scheduling method
// I: No input
// O: No output
// R: Only numbers will be taken from the user
void main_program()
{
    printf("\n--- Loading UI ---\n");
}

// Function for obtaining preset load of boats from user
// I: No input
// O: No output
// R: Only numbers will be taken from the user
void preset_load()
{
    printf("Bienvenido a la configuración inicial de barcos!\n");
    printf("Indique la cantidad de barcos normales que desea de lado izquierdo: ");
    scanf("%d", &global_normal_left);
    printf("Indique la cantidad de barcos pesqueros que desea de lado izquierdo: ");
    scanf("%d", &global_fishing_left);
    printf("Indique la cantidad de barcos patrulleros que desea de lado izquierdo: ");
    scanf("%d", &global_patrols_left);
    printf("Indique la cantidad de barcos normales que desea de lado derecho: ");
    scanf("%d", &global_normal_right);
    printf("Indique la cantidad de barcos patrulleros que desea de lado derecho: ");
    scanf("%d", &global_fishing_right);
    printf("Indique la cantidad de barcos patrulleros que desea de lado derecho: ");
    scanf("%d", &global_fishing_right);
    main_program();
}

// Function for cleaning input buffer
// I: No input
// O: No output
// R: No restrictions
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Discard all characters until newline
}

// Function for displaying initial Menu uptions
// I: No input
// O: No output
// R: No restrictions
void displayMenu()
{
    printf("\n--- Menu ---\n");
    printf("1. Si\n");
    printf("2. No\n");
    printf("3. Salir\n");
    printf("¿Desea generar una carga preestablecida?: ");
}

// Function for obtaining preset load of boats from user
// I: No input
// O: 0 of OK
// R: Restrictions are hanlded separately by each child-function
int main()
{
    int choice;

    const char *filename = "config.txt"; // Specify your configuration file here
    Config config = loadConfig(filename);

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