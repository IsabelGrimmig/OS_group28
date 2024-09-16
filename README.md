# OS_group28
Mandatory assignment for OS

Task 1: code explanation.
read_char:
This function uses the read system call to read a single character from stdin (file descriptor 0).
If the read system call successfully reads 1 character, it returns that character. If no characters are available or there is an error, it returns EOF.

write_char:
This function uses the write system call to write a single character to stdout (file descriptor 1).
If the write system call successfully writes 1 character, it returns 0. If an error occurs, it returns EOF.

write_string:
This function calculates the length of the string using strlen, then writes the entire string to stdout using the write system call.
If all characters are successfully written, it returns 0. Otherwise, it returns EOF.

write_int:
This function converts an integer to its string representation. If the integer is negative, it adds a '-' sign.
It then reverses the digits because numbers are constructed in reverse order when using modulus (% 10).
Finally, it calls write_string to output the integer to stdout.



Task 2: code explanation.

