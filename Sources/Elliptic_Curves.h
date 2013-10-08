/** @file Elliptic_Curves.h
 * Basic operations for Weierstrass elliptic curves.
 */
#ifndef H_ELLIPTIC_CURVES_H
#define H_ELLIPTIC_CURVES_H

#include <gmp.h>

//--------------------------------------------------------------------------------------------------------
// Types
//--------------------------------------------------------------------------------------------------------
/** An elliptic curve point. */
typedef struct
{
	mpz_t X; //! X coordinate.
	mpz_t Y; //! Y coordinate.
	char Is_Infinite; //! Indicate if the point can be used for computations or not.
} TPoint;

/** Full elliptic curve description. */
/*typedef struct
{
	TODO
} TEllipticCurve;*/

//--------------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------------
/** Compute the opposite of a point.
 * @param Pointer_Input_Point The point to compute the opposite.
 * @param Pointer_Output_Point The computed opposite.
 */
void ECOpposite(TPoint *Pointer_Input_Point, TPoint *Pointer_Output_Point);

#endif