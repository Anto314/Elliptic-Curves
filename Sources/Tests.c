/** @file Main.c
 */
#include <stdio.h>
#include <gmp.h>
#include "Elliptic_Curves.h"
#include "Point.h"

int main(void)
{
	TEllipticCurve Curve;
	TPoint A, B, C;
	mpz_t Number;
	
	printf("--- TESTS ---\n");
	
	// Load curve
	if (!ECLoadFromFile("../Curves/Test.gp", &Curve))
	{
		printf("Error : can't load curve file.\n");
		return -1;
	}
	
	PointCreate(3, 1, &A);
	PointCreate(2, 4, &B);
	PointCreate(0, 0, &C);
	mpz_init(Number);
	mpz_set_ui(Number, 53);
	//mpz_set_str(Number, "12978522537291826747218973855244672319565387", 10);
	
	// Test distinct points addition
	printf("Adding 2 distinct points : (expected value is X = 4, Y = 2)\n");
	ECAddition(&Curve, &A, &B, &C);
	PointShow(&C);
	// Check values
	if ((mpz_cmp_ui(C.X, 4) != 0) || (mpz_cmp_ui(C.Y, 2) != 0))
	{
		printf("FAILED\n");
		return 0;
	}
	printf("SUCCESS\n\n");
	
	// Test doubling
	printf("Point doubling : (expected value is X = 2, Y = 1)\n");
	ECAddition(&Curve, &B, &B, &C);
	PointShow(&C);
	// Check values
	if ((mpz_cmp_ui(C.X, 2) != 0) || (mpz_cmp_ui(C.Y, 1) != 0))
	{
		printf("FAILED\n");
		return 0;
	}	
	printf("SUCCESS\n\n");
	
	// Test multiplying by a scalar
	printf("Multiplying by a scalar : (expected value is X = 3, Y = 4)\n");
	ECMultiplication(&Curve, &A, Number, &C);
	PointShow(&C);
	// Check values
	if ((mpz_cmp_ui(C.X, 3) != 0) || (mpz_cmp_ui(C.Y, 4) != 0))
	{
		printf("FAILED\n");
		return 0;
	}	
	printf("SUCCESS\n\n");
	
	return 0;
}
