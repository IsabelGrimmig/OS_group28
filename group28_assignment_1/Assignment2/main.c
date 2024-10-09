
/* You are not allowed to use <stdio.h> */
#include "io.h"
#include <stdlib.h>


/**
 * @name  main
 * @brief This function is the entry point to your program
 * @return 0 for success, anything else for failure
 *
 *
 * Then it has a place for you to implementation the command 
 * interpreter as  specified in the handout.
 */
int main(){
    /*write_int (1);
    write_int (10);
    write_char('\n');
    return;*/
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

  //#define NULL 0
  #define TRUE 1
  #define MAX_ELEMENTS


    // Declare structure
    int *collection = NULL;    // Array to store collection of elements
    int counter = 0;           // Counter to keep track of the incrementation
    int capacity = 10;         // Initial capacity of the collection
    int command;               // Variable to store the command read from read_char()
    int collectionSpace = 0;   // Counter to keep track of the number of elements


    // Allocate memory for the collection
    collection = (int *)malloc(capacity * sizeof(int));
    if (collection == NULL) {
        write_string("Memory allocation failed\n");
        return 1;  // Exit if memory allocation fails
    }

    // Loop to read and process commands
    while (TRUE) {
        command = read_char();  // Read a command using read_char()

        if (counter >= capacity) {
            capacity *= 2;
            collection = (int *)realloc(collection, capacity * sizeof(int));
            if (collection == NULL) {
                write_string("Memory reallocation failed\n");
                return 1;  // Exit if memory reallocation fails
            }
        }

        // Check if the command is valid ('a', 'b', 'c'). If not, break the loop.
        if (command != 'a' && command != 'b' && command != 'c') {
        //if (command != 'a' || 'b' || 'c'){
            break;
        }else {

        // Process commands
        if (command == 'a'){
            collection[collectionSpace] = counter;
            collectionSpace++;
        } else if (command == 'c'){
            if (collectionSpace != 0)
            {
                collectionSpace--;
            }
collection[collectionSpace] = 0;
        }
        //write_int(counter);
        //write_string("\n");
        counter += 1;
        }
    }

    // Print the collection as a comma-delimited series of integers
    for (int i = 0; i < counter; i++) {
        if (i != 0 && collection[i] != 0) {
            write_char(',');  // Print a comma before each element except the first
        }
        if (i != 0 && collection[i] == 0)
        {
        
        }else
        {
            write_int(collection[i]);
        }
    }
    write_char(';');
    write_char('\n');  // Print a newline after the collection
    free(collection);

    return 0;
}
