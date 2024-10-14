#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

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

int main()
{
    printf("Press any key to see the output. Press 'x' to exit.\n");
    while (1)
    {
        // Check for key press
        int key = kbhit();
        if (key)
        {
            if (key == 'x')
            {
                printf("Exiting...\n");
                break; // Exit if 'q' is pressed
            }
            printf("Key pressed: %c\n", key);
        }

        // Simulate some work (or sleep)
        usleep(100000); // Sleep for 100 milliseconds
    }

    return 0;
}
