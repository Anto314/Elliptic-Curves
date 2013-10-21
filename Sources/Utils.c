/** @file Utils.c
 * Utility functions.
 */
#include <sys/time.h>
#include <stdio.h>
#include <gmp.h>
#include "Utils.h"

/** Internal random state. */
static gmp_randstate_t Random_State;

void UtilsInitializeRandomGenerator(void)
{
	struct timeval Time;
	
	// Get a pseudo random seed
	gettimeofday(&Time, NULL);
	
	// Initialize GMP random number generator
	gmp_randinit_default(Random_State);
	gmp_randseed_ui(Random_State, (unsigned long) Time.tv_usec);
}

void UtilsGenerateRandomNumber(mpz_t Modulus, mpz_t Random_Number)
{
	mpz_urandomm(Random_Number, Random_State, Modulus);
}