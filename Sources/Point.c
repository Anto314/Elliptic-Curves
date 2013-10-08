/** @file Point.c
 * An elliptic curve point.
 */
#include <stdio.h>
#include <gmp.h>
#include "Point.h"

void PointCreate(unsigned int X, unsigned int Y, TPoint *Pointer_Output_Point)
{
	mpz_init_set_ui(Pointer_Output_Point->X, X);
	mpz_init_set_ui(Pointer_Output_Point->Y, Y);
	Pointer_Output_Point->Is_Infinite = 0;
}

int PointIsEqualX(TPoint *Pointer_Point_A, TPoint *Pointer_Point_B)
{
	if (mpz_cmp(Pointer_Point_A->X, Pointer_Point_B->X) == 0) return 1;
	return 0;
}

void PointCopy(TPoint *Pointer_Destination_Point, TPoint *Pointer_Source_Point)
{
	mpz_set(Pointer_Destination_Point->X, Pointer_Source_Point->X);
	mpz_set(Pointer_Destination_Point->Y, Pointer_Source_Point->Y);
	Pointer_Destination_Point->Is_Infinite = Pointer_Source_Point->Is_Infinite;
}