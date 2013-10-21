/** @file Utils.c
 * Utility functions.
 */
#include <sys/time.h>
#include <stdio.h>

unsigned long UtilsGenerateRandomSeed(void)
{
	struct timeval Time;
	
	gettimeofday(&Time, NULL);
	return (unsigned long) Time.tv_usec;
}