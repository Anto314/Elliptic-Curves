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
	ECLoadFromFile("test.gp", &Curve);
	
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
