/** @file Elliptic_Curves.c
 * Basic operations for Weierstrass elliptic curves.
 */
#include <stdio.h>
#include <gmp.h>
#include "Elliptic_Curves.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
/** Add two points.
 * @param Pointer_Curve Elliptic curve.
 * @param Pointer_Point_P First point to add.
 * @param Pointer_Point_Q Second point to add.
 * @param Lambda Precalculated lambda.
 * @param Pointer_Output_Point Result.
 */
static inline void ECAdd(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point_P, TPoint *Pointer_Point_Q, mpz_t Lambda, TPoint *Pointer_Output_Point)
{
	mpz_t Temp, Result_X, Result_Y;
	
	mpz_init(Temp);
	mpz_init(Result_X);
	mpz_init(Result_Y);
	
	// Compute xr
	mpz_mul(Result_X, Lambda, Lambda); // lambda^2
	mpz_sub(Result_X, Result_X, Pointer_Point_P->X); // lambda^2 - xp
	mpz_sub(Result_X, Result_X, Pointer_Point_Q->X); // lambda^2 - xp - xq
	mpz_mod(Result_X, Result_X, Pointer_Curve->p);
	
	// Compute yr
	mpz_neg(Temp, Lambda); // -lambda
	mpz_mul(Temp, Temp, Result_X); // -lambda * xr
	mpz_mul(Result_Y, Lambda, Pointer_Point_P->X); // lambda * xp
	mpz_add(Result_Y, Temp, Result_Y); // (-lambda * xr) + (lambda * xp)
	mpz_sub(Result_Y, Result_Y, Pointer_Point_P->Y); // (-lambda * xr) + (lambda * xp) - yp
	mpz_mod(Result_Y, Result_Y, Pointer_Curve->p);
	
	// Set result
	mpz_set(Pointer_Output_Point->X, Result_X);
	mpz_set(Pointer_Output_Point->Y, Result_Y);
	
	mpz_clear(Temp);
	mpz_clear(Result_X);
	mpz_clear(Result_Y);
}

/** Compute the double of a point.
 * @param Pointer_Curve The elliptic curve.
 * @param Pointer_Point_P The point to double.
 * @param Pointer_Output_Point Result.
 */
static inline void ECDouble(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point_P, TPoint *Pointer_Output_Point)
{
	mpz_t Lambda, Temp;
	
	if (Pointer_Point_P->Is_Infinite)
	{
		Pointer_Output_Point->Is_Infinite = 1;
		return;
	}	
	
	mpz_init(Lambda);
	mpz_init(Temp);
	
	// Compute lambda
	// Compute numerator
	mpz_mul(Lambda, Pointer_Point_P->X, Pointer_Point_P->X); // xp^2
	mpz_mul_ui(Lambda, Lambda, 3); // 3*xp^2
	mpz_add(Lambda, Lambda, Pointer_Curve->a4); // 3*xp^2 + a4
	
	// Compute denominator
	mpz_mul_ui(Temp, Pointer_Point_P->Y, 2);
	// We can't compute integer division as it is not reliable, so invert denominator to compute a*b^-1 instead of a/b
	mpz_invert(Temp, Temp, Pointer_Curve->p);
	
	// Compute "division"
	mpz_mul(Lambda, Lambda, Temp);
	// Compute remainder to stay on Fp
	mpz_mod(Lambda, Lambda, Pointer_Curve->p);
	
	// Double the point
	ECAdd(Pointer_Curve, Pointer_Point_P, Pointer_Point_P, Lambda, Pointer_Output_Point);
	
	#ifdef DEBUG
		if (Pointer_Output_Point->Is_Infinite) printf("[ECDouble] Result is infinite\n");
	#endif
	
	mpz_clear(Lambda);
	mpz_clear(Temp);
}

/** Add two different points.
 * @param Pointer_Curve The ellipitic curve.
 * @param Pointer_Point_P First point.
 * @param Pointer_Point_Q Second point.
 * @param Pointer_Output_Point Result.
 */
static inline void ECAddDifferentPoints(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point_P, TPoint *Pointer_Point_Q, TPoint *Pointer_Output_Point)
{
	mpz_t Lambda, Temp;
	
	mpz_init(Lambda);
	mpz_init(Temp);
	
	// Compute lambda
	// Compute numerator
	mpz_sub(Lambda, Pointer_Point_P->Y, Pointer_Point_Q->Y); // yp - yq
	
	// Compute denominator
	mpz_sub(Temp, Pointer_Point_P->X, Pointer_Point_Q->X); // xp - xq
	// We can't compute integer division as it is not reliable, so invert denominator to compute a*b^-1 instead of a/b
	mpz_invert(Temp, Temp, Pointer_Curve->p);
	
	// Compute "division"
	mpz_mul(Lambda, Lambda, Temp);
	// Compute remainder to stay on Fp
	mpz_mod(Lambda, Lambda, Pointer_Curve->p);
		
	// Add the two points
	ECAdd(Pointer_Curve, Pointer_Point_P, Pointer_Point_Q, Lambda, Pointer_Output_Point);
	
	mpz_clear(Lambda);
	mpz_clear(Temp);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int ECLoadFromFile(char *String_Path, TEllipticCurve *Pointer_Curve)
{
	FILE *File;
	
	File = fopen(String_Path, "r");
	if (File == NULL) return 0;
	
	// Initialize curve members
	mpz_init(Pointer_Curve->p);
	mpz_init(Pointer_Curve->n);
	mpz_init(Pointer_Curve->a4);
	mpz_init(Pointer_Curve->a6);
	mpz_init(Pointer_Curve->Point_Generator.X);
	mpz_init(Pointer_Curve->Point_Generator.Y);
	Pointer_Curve->Point_Generator.Is_Infinite = 0;
	
	// Load values
	gmp_fscanf(File, "p=%Zd\n", &Pointer_Curve->p);
	gmp_fscanf(File, "n=%Zd\n", &Pointer_Curve->n);
	gmp_fscanf(File, "a4=%Zd\n", &Pointer_Curve->a4);
	gmp_fscanf(File, "a6=%Zd\n", &Pointer_Curve->a6);
	
	// Bypass r4 and r6 (they are not used but they can desynchronize the gmp_scanf())
	gmp_fscanf(File, "r4=%Zd\n", &Pointer_Curve->Point_Generator.X);
	gmp_fscanf(File, "r6=%Zd\n", &Pointer_Curve->Point_Generator.X);
	
	// Continue to load useful values
	gmp_fscanf(File, "gx=%Zd\n", &Pointer_Curve->Point_Generator.X);
	gmp_fscanf(File, "gy=%Zd\n", &Pointer_Curve->Point_Generator.Y);
	
	fclose(File);
	return 1;
}

void ECFree(TEllipticCurve *Pointer_Curve)
{
	mpz_clear(Pointer_Curve->p);
	mpz_clear(Pointer_Curve->n);
	mpz_clear(Pointer_Curve->a4);
	mpz_clear(Pointer_Curve->a6);
}

void ECOpposite(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Input_Point, TPoint *Pointer_Output_Point)
{
	mpz_set(Pointer_Output_Point->X, Pointer_Input_Point->X);
	mpz_neg(Pointer_Output_Point->Y, Pointer_Input_Point->Y);
	mpz_mod(Pointer_Output_Point->Y, Pointer_Output_Point->Y, Pointer_Curve->p);
	Pointer_Output_Point->Is_Infinite = Pointer_Input_Point->Is_Infinite;
}

void ECAddition(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point_P, TPoint *Pointer_Point_Q, TPoint *Pointer_Output_Point)
{
	TPoint Point_Temp;
	
	// Is P infinite ?
	if (Pointer_Point_P->Is_Infinite)
	{
		// Result is Q
		PointCopy(Pointer_Point_Q, Pointer_Output_Point);
		#ifdef DEBUG
			printf("[ECAddition] P is infinite, returning Q\n");
		#endif
		return;
	}
	
	// Is Q infinite ?
	if (Pointer_Point_Q->Is_Infinite)
	{
		// Result is P
		PointCopy(Pointer_Point_P, Pointer_Output_Point);
		#ifdef DEBUG
			printf("[ECAddition] Q is infinite, returning P\n");
		#endif
		return;
	}
	
	// Are P and Q opposite ?
	PointCreate(0, 0, &Point_Temp);
	ECOpposite(Pointer_Curve, Pointer_Point_Q, &Point_Temp);
	
	// Result is infinite
	if (PointIsEqual(Pointer_Point_P, &Point_Temp))
	{
		Pointer_Output_Point->Is_Infinite = 1;
		#ifdef DEBUG
			printf("[ECAddition] Result is infinite\n");
		#endif
	}
	// Not infinite, choose correct adding function
	else
	{
		// Is P equal to Q ?
		if (PointIsEqual(Pointer_Point_P, Pointer_Point_Q)) ECDouble(Pointer_Curve, Pointer_Point_P, Pointer_Output_Point); // Double the point
		else ECAddDifferentPoints(Pointer_Curve, Pointer_Point_P, Pointer_Point_Q, Pointer_Output_Point); // Add the two different points
		Pointer_Output_Point->Is_Infinite = 0;
		#ifdef DEBUG
			printf("[ECAddition] Result : ");
			PointShow(Pointer_Output_Point);
		#endif
	}
	PointFree(&Point_Temp);
}

void ECMultiplication(TEllipticCurve *Pointer_Curve, TPoint *Pointer_Point, mpz_t Factor, TPoint *Pointer_Output_Point)
{
	int Bits_Count, i;
	TPoint Point_Temp;
	
	// Initialize variables
	PointCreate(0, 0, &Point_Temp);
	PointCopy(Pointer_Point, &Point_Temp); // Allow using the same variable for Pointer_Point and Pointer_Output_Point
	
	Pointer_Output_Point->Is_Infinite = 1;
	
	// Retrieve how many bits are used to store the factor number
	Bits_Count = mpz_size(Factor) * mp_bits_per_limb;
	
	// Double-and-add starting from most significant bit to minimize computations
	for (i = Bits_Count - 1; i >= 0; i--)
	{
		// Always double the point
		ECDouble(Pointer_Curve, Pointer_Output_Point, Pointer_Output_Point);
		
		// But add doubled values only when a factor bit is set
		if (mpz_tstbit(Factor, i)) ECAddition(Pointer_Curve, Pointer_Output_Point, &Point_Temp, Pointer_Output_Point);
	}
	
	// Free resources
	PointClear(&Point_Temp);
}