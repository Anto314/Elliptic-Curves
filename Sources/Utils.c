/** @file Utils.c
 * Utility functions.
 */
#include <sys/time.h>
#include <stdio.h>
#include <gmp.h>
#include <openssl/evp.h>
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

int UtilsComputeHash(unsigned char *Pointer_Data_Buffer, size_t Data_Buffer_Size, unsigned char *Pointer_Output_Hash)
{
	EVP_MD_CTX Context;
	
	OpenSSL_add_all_digests();
	
	// Initialize SSL context
	EVP_MD_CTX_init(&Context);
	
	// Select SHA-1 algorithm
	if (!EVP_DigestInit_ex(&Context, EVP_sha1(), NULL))
	{
		EVP_MD_CTX_cleanup(&Context);
		return 0;
	}
	
	// "Digest" data to hash
	if (!EVP_DigestUpdate(&Context, Pointer_Data_Buffer, Data_Buffer_Size))
	{
		EVP_MD_CTX_cleanup(&Context);
		return 0;
	}
	
	// output hash
	if (!EVP_DigestFinal_ex(&Context, Pointer_Output_Hash, NULL))
	{
		EVP_MD_CTX_cleanup(&Context);
		return 0;
	}
	
	EVP_MD_CTX_cleanup(&Context);
	return 1;
}

void UtilsShowHash(unsigned char *Pointer_Hash_Buffer)
{
	int i;
	
	for (i = 0; i < UTILS_HASH_LENGTH; i++) printf("%02x", Pointer_Hash_Buffer[i]);
	putchar('\n');
}