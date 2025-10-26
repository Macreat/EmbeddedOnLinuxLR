/**
 * @file hello_args.c
 * @brief Simple example of handling command-line arguments in C.
 *
 * This program demonstrates how to use `argc` and `argv`
 * to read arguments passed to the executable at runtime.
 *
 * Usage examples:
 *   $ ./hello_args
 *   → Output: "Error: Missing arguments."
 *
 *   $ ./hello_args name
 *   → Output: "Hello, name!"
 *
 * Course: Embedded Systems on Linux – UNAL 2025-2
 */

#include <stdio.h>

/**
 * @brief Entry point of the program.
 * @param argc Number of command-line arguments (including program name).
 * @param argv Array of argument strings.
 * @return int Exit status code (0 = success).
 */
int main(int argc, char *argv[])
{
    // Case 1: No arguments passed
    if (argc == 1)
    {
        printf("Error: Missing arguments.\n");
        printf("Usage: %s <name>\n", argv[0]);
        return 1; // Return non-zero to indicate incorrect usage
    }

    // Case 2: At least one argument is provided
    // argv[1] = first user argument (after program name)
    printf("Hello, %s!\n", argv[1]);

    return 0;
}
