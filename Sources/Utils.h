/** @file Utils.h
 * Utility functions.
 */
#ifndef H_UTILS_H
#define H_UTILS_H

/** Length in bytes of a hash computed by the UtilsComputeHash() function. */
#define UTILS_HASH_LENGTH 20

/** Initialize the GMP random generator (same as srand()). */
void UtilsInitializeRandomGenerator(void);

/** Generate a random number.
 * @param Modulus The number will be in range 0..Modulus - 1.
 * @param Output_Number On output, store the generated random number.
 */
void UtilsGenerateRandomNumber(mpz_t Modulus, mpz_t Output_Number);

/** Use the SHA-1 algorithm to compute the hash of the data.
 * @param Pointer_Data_Buffer The buffer containing the data to hash.
 * @param Data_Buffer_Size Size of the data to hash.
 * @param Pointer_Output_Hash On output, hold the hash (the output buffer must be almost UTILS_HASH_LENGTH bytes long).
 * @return 1 if the hash was correctly computed or 0 if an error occured.
 */
int UtilsComputeHash(unsigned char *Pointer_Data_Buffer, size_t Data_Buffer_Size, unsigned char *Pointer_Output_Hash);

/** Display a previously computed hash.
 * @param Pointer_Hash_Buffer The buffer containing the hash to display.
 */
void UtilsShowHash(unsigned char *Pointer_Hash_Buffer);

#endif