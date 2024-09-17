
/* You are not allowed to use <stdio.h> */
#include "io.h"


/**
 * @name  main
 * @brief This function is the entry point to your program
 * @return 0 for success, anything else for failure
 *
 *
 * Then it has a place for you to implementation the command 
 * interpreter as  specified in the handout.
 */
int
main()
{
  /*-----------------------------------------------------------------
   *TODO:  You need to implement the command line driver here as
   *       specified in the assignment handout.
   *
   * The following pseudo code describes what you need to do
   *  
   *  Declare the counter and the collection structure variables
   *
   *
   *  In a loop
   *    1) Read a command from standard in using read_char function
   *    2) If the command is not 'a', 'b', 'c': then break the loop
   *    3) Process the command as specified in the handout
   *  End loop
   *
   *  Print your collection of elements as specified in the handout
   *    as a comma delimited series of integers
   *-----------------------------------------------------------------*/

  #define MAX_ELEMENTS 100  // Define max number of elements in the collection


    // Declare variables
    int collection[MAX_ELEMENTS];  // Array to store collection of elements
    int counter = 0;               // Counter to keep track of the number of elements
    int command;                   // Variable to store the command read from read_char()
    
    // Initialize the collection to all zeros (optional, depending on the use case)
    /*for (int i = 0; i < MAX_ELEMENTS; i++) {
        collection[i] = 0;
    }*/

    // Loop to read and process commands
    while (1) {
        command = read_char();  // Read a command using read_char()

        // Check if the command is valid ('a', 'b', 'c'). If not, break the loop.
        if (command != 'a' | 'b' | 'c') {
            break;
        }

        // Process commands
        switch (command) {
            case 'a':
                // Add an element to the collection
                if (counter < MAX_ELEMENTS) {
                    collection[counter++] = 1;  // Add a 1 to the collection (adjust as per the spec)
                } else {
                    printf("Collection is full!\n");
                    break;
                }

            case 'b':
                // Add a different element to the collection
                if (counter < MAX_ELEMENTS) {
                    collection[counter++] = 2;  // Add a 2 to the collection (adjust as per the spec)
                } else {
                    printf("Collection is full!\n");
                    break;
                }

            case 'c':
                // Add another type of element to the collection
                if (counter < MAX_ELEMENTS) {
                    collection[counter++] = 3;  // Add a 3 to the collection (adjust as per the spec)
                } else {
                    printf("Collection is full!\n");
                    break;
                }
        }
    }

    // Print the collection as a comma-delimited series of integers
    for (int i = 0; i < counter; i++) {
        if (i != 0) {
            printf(",");  // Print a comma before each element except the first
        }
        printf("%d", collection[i]);
    }
    printf("\n");  // Print a newline after the collection
  

  return 0;
}
