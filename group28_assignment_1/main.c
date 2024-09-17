
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

  #define NULL 0


    // Declare variables
    int *collection = NULL;    // Pointer to store the collection dynamically
    int counter = 0;           // Counter to keep track of the incrementation
    int capacity = 10;         // Initial capacity of the collection
    int command;               // Variable to store the command read from read_char()
    int collectionSpace = 0;    // counter to keep track of the number of elements

    // Allocate memory for the collection
    collection = (int *)malloc(capacity * sizeof(int));
    if (collection == NULL) {
        printf("Memory allocation failed\n");
        return 1;  // Exit if memory allocation fails
    }

    // Loop to read and process commands
    while (1) {
        command = read_char();  // Read a command using read_char()

        // Check if the command is valid ('a', 'b', 'c'). If not, break the loop.
        if (command != 'a' | 'b' | 'c') {
            break;
        }

        // If the collection is full, double its capacity
        if (collectionSpace >= capacity) {
            capacity *= 2;
            collection = (int *)realloc(collection, capacity * sizeof(int));
            if (collection == NULL) {
                printf("Memory reallocation failed\n");
                return 1;  // Exit if memory reallocation fails
            }
        }

        // Process commands
        switch (command) {
            case 'a':
                collection[collectionSpace++] = counter;  // Add the current count to the collection
                break;

            case 'b':                                     // Do nothing except incrementing count
                break;

            case 'c':
                collection[collectionSpace] = 0;          // Remove the latest added value from the collection
                collectionSpace--;
                break;
        }
        counter++;                                        // Increment counter
    }

    // Sum the collection
    int sum = 0;
    for (int i = 0; i < collectionSpace; i++) {
        sum = sum + collection[i];
    }
    printf(sum);  
    print ("\n"); // Print a newline after the sum of collection

    // Free the allocated memory
    free(collection);

  return 0;
}
