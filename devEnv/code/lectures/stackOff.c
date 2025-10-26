/**
 * @file stackOff.c (recursive sum)
 * @brief Demonstrates recursion in C by summing numbers from 1 to N.
 *
 * This program defines a recursive function `suma()` that returns the sum of
 * all natural numbers from 1 up to a given limit `n`.
 *
 * Example:
 *   Input: 5
 *   Output: 15  (because 1+2+3+4+5 = 15)
 *
 * Course: Embedded Systems on Linux – UNAL 2025-2
 */

#include <stdio.h>

/**
 * @brief Recursive function that returns the sum of all integers from 1 to i.
 * @param i Upper limit of the sum (must be >= 1).
 * @return Sum of all numbers from 1 to i.
 */
unsigned long int suma(unsigned long int i)
{
    if (i == 1)
        return 1; // base case
    else
        return i + suma(i - 1); // recursive case
}

/**
 * @brief Main function that tests the recursive sum.
 */
int main(void)
{
    unsigned long int tope = 1000;
    unsigned long int result = suma(tope);

    printf("Suma recursiva de 1 a %lu = %lu\n", tope, result);
    return 0;
}
