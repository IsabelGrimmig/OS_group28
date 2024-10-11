
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "io.h"

/* Reads next char from stdin. If no more characters, it returns EOF */
int read_char() {
    char c;
    int result = read(0, &c, 1);    //File descriptor 0 is stdin
    
//Successfully read a character
    if (result == 1) {
        return (int)c;
    } else {
//Either an error occurred or EOF
        return EOF;
    }
}

/* Writes c to stdout.  If no errors occur, it returns 0, otherwise EOF */
int write_char(char c) {
    int result = write(0,&c,1 );    // File descriptor 1 is stdout
    if (result == 1) {
        return 0;
    } else {
        return EOF;
    }
}

/* Writes a null-terminated string to stdout.  If no errors occur, it returns 0, otherwise EOF */
int write_string(char* s) {
    int length = strlen(s);
    int result = write(1, s, length);        // Write the entire string to stdout
    if (result == length) {
        return 0;
    } else {
        return EOF;
    }
    
}

// Writes n to stdout (without any formatting). If no errors occur, it returns 0, otherwise EOF
//int write_int(int n) {
    int write_int(int n)
    {
        char buffer[13]; // Buffer large enough to hold an integer of 11 digits, including sign and null terminator.
        int length = 0;

        // Handles special cases where n = 0
        if (n == 0)
        {
            buffer[length++] = '0';
        }
        else
        {
            // Handles negative numbers
            if (n < 0)
            {
                buffer[length++] = '-'; // Adds a '-' to the buffer
                n = -n;                 // Converts n to a positive
            }

            // converts the integer to a string (is stored in reverse in the buffer)
            int start = length;
            while (n > 0)
            {
                buffer[length++] = '0' + (n % 10); // Adds the digit to the buffer
                n /= 10;                           // Go to the next digit
            }

            // Reverses the contents of the buffer to get it in the correct order
            for (int i = start, j = length - 1; i < j; i++, j--)
            {
                char temp = buffer[i];
                buffer[i] = buffer[j];
                buffer[j] = temp;
            }
        }

        // Null-terminates the string
        buffer[length] = '\0';

        // use the write_string function to print the result
        return write_string(buffer);
    }

/*
    char buffer[12] = {'\0'};    // Buffer large enough to hold an integer, assuming no more than 11 digits + null terminator
        int length = 0;
        
    //Convert the integer to a string
        if (n == 0) {
            buffer[length++] = '0';
        } else {
            if (n < 0) {
                buffer[length] = '-';
                n = -n;
            }
            
            int start = length;
            while (n > 0) {
                buffer[length++] = '0' + (n % 10);
                n /= 10;
            }
    //Reverse the digits in the buffer (simple string reversal)
            for (int i = start; i < length - 1; ++i) {
                char temp = buffer[i];
                buffer[i] = buffer [length - 1];
                buffer[length - 1] = temp;
            }
            
        }
        //buffer[length] = '\0';              // Null-terminate the string
        return write_string(buffer);        // Use write_string to write it to stdout
*/
  
//}
