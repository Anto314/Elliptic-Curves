/** @file Utils.h
 * Utility functions.
 */
#ifndef H_UTILS_H
#define H_UTILS_H

/** Initialize the GMP random generator (same as srand()). */
void UtilsInitializeRandomGenerator(void);

/** Generate a random number.
 * @param Modulus The number will be in range 0..Modulus - 1.
 * @param Output_Number On output, store the generated random number.
 */
void UtilsGenerateRandomNumber(mpz_t Modulus, mpz_t Output_Number);

#endif