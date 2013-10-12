/** @file Elliptic_Curves.h
 * Basic operations for Weierstrass elliptic curves.
 */
#ifndef H_ELLIPTIC_CURVES_H
#define H_ELLIPTIC_CURVES_H

#include <gmp.h>
#include <assert.h>
#include "Point.h"

//--------------------------------------------------------------------------------------------------------
// Types
//--------------------------------------------------------------------------------------------------------
/** Full elliptic curve description. */
typedef struct
{
	mpz_t p;
	mpz_t n;
	mpz_t a4;
	mpz_t a6;
} TEllipticCurve;

//--------------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------------

/** Load an elliptic curve from a .gp file.
 * @param String_Path Path to the file.
 * @param Pointer_Curve Where to store the curve.
 * @return 0 if the file was not found or 1 if the curve was successfully loaded.
 */
int ECLoadFromFile(char *String_Path, TEllipticCurve *Pointer_Curve);

/** Free a curve.
 * @param Pointer_Curve The curve to destroy.
 */
void ECFree(TEllipticCurve *Pointer_Curve);

/** Compute the opposite of a point.
 * @param Pointer_Curve The elliptic curve.
 * @param Pointer_Input_Point The point to compute the opposite.
 * @param Pointer_Output_Point The computed opposite (the point must be created by the user).
 */
void ECOpposite(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Input_Point, TPoint *Pointer_Output_Point);

/** Add two points.
 * @param Pointer_Curve The elliptic curve used for addition.
 * @param Pointer_Point_P First operand.
 * @param Pointer_Point_Q Second operand.
 * @param Pointer_Output_Point Result (the point must be created by the user).
 */
void ECAddition(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point_P, TPoint *Pointer_Point_Q, TPoint *Pointer_Output_Point);

#endif