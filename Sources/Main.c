/** @file Main.c
 */
#include <stdio.h>
#include <gmp.h>
#include "Elliptic_Curves.h"
#include "Point.h"

static void Tests(void)
{
	TEllipticCurve Curve;
	TPoint A, B, C;
	
	printf("--- TESTS ---\n");
	
	//ECLoadFromFile("w256-001.gp", &Curve);
	if (!ECLoadFromFile("test.gp", &Curve))
	{
		printf("Error : can't load curve file.\n");
		return -1;
	}
	
	PointCreate(3, 1, &A);
	PointCreate(2, 4, &B);
	PointCreate(0, 0, &C);
	
	// Test doubling
	ECAddition(&Curve, &B, &B, &C);
	
	// Test different points addition
	ECAddition(&Curve, &A, &B, &C);
	
	PointDelete(&A);
	PointDelete(&B);
	PointDelete(&C);
}

int main(void)
{
	Tests();
	
	return 0;
}
