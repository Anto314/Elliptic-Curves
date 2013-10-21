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

void PointFree(TPoint *Pointer_Point)
{
	mpz_clear(Pointer_Point->X);
	mpz_clear(Pointer_Point->Y);
}

int PointIsEqual(TPoint *Pointer_Point_A, TPoint *Pointer_Point_B)
{
	if (mpz_cmp(Pointer_Point_A->X, Pointer_Point_B->X) != 0) return 0;
	if (mpz_cmp(Pointer_Point_A->Y, Pointer_Point_B->Y) != 0) return 0;
	if (Pointer_Point_A->Is_Infinite != Pointer_Point_B->Is_Infinite) return 0;
	return 1;
}

int PointIsEqualXOnly(TPoint *Pointer_Point_A, TPoint *Pointer_Point_B)
{
	if (mpz_cmp(Pointer_Point_A->X, Pointer_Point_B->X) != 0) return 0;
	return 1;
}

void PointCopy(TPoint *Pointer_Source_Point, TPoint *Pointer_Destination_Point)
{
	mpz_set(Pointer_Destination_Point->X, Pointer_Source_Point->X);
	mpz_set(Pointer_Destination_Point->Y, Pointer_Source_Point->Y);
	Pointer_Destination_Point->Is_Infinite = Pointer_Source_Point->Is_Infinite;
}

void PointShow(TPoint *Pointer_Point)
{
	if (Pointer_Point->Is_Infinite) printf("Point is infinite.\n");
	else gmp_printf("X = %Zd, Y = %Zd\n", Pointer_Point->X, Pointer_Point->Y);
}

void PointClear(TPoint *Pointer_Point)
{
	mpz_set_ui(Pointer_Point->X, 0);
	mpz_set_ui(Pointer_Point->Y, 0);
	Pointer_Point->Is_Infinite = 0;
}