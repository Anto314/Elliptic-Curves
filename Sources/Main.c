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
	
	printf("--- TESTS ---\n");
	
	//ECLoadFromFile("w256-001.gp", &Curve);
	ECLoadFromFile("test.gp", &Curve);
	
	PointCreate(3, 1, &A);
	PointCreate(2, 4, &B);
	PointCreate(0, 0, &C);
	ECAddition(&Curve, &A, &B, &C);
	
	return 0;
}
