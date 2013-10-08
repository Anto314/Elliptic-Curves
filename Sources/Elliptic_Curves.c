/** @file Elliptic_Curves.c
 * Basic operations for Weierstrass elliptic curves.
 */
#include <stdio.h>
#include <gmp.h>
#include "Elliptic_Curves.h"

void ECOpposite(TPoint *Pointer_Input_Point, TPoint *Pointer_Output_Point)
{
	mpz_init(Pointer_Output_Point->X);
	mpz_init(Pointer_Output_Point->Y);
	
	mpz_set(Pointer_Output_Point->X, Pointer_Input_Point->X);
	mpz_neg(Pointer_Output_Point->Y, Pointer_Input_Point->Y);
}

