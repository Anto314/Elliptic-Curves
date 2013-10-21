/** @file Point.h
 * An elliptic curve point.
 */
#ifndef H_POINT_H
#define H_POINT_H

/** An elliptic curve point. */
typedef struct
{
	mpz_t X; //! X coordinate.
	mpz_t Y; //! Y coordinate.
	char Is_Infinite; //! Indicate if the point can be used for computations or not.
} TPoint;

/** Initialize a new point.
 * @param X X coordinate.
 * @param Y Y coordinate.
 * @param Pointer_Output_Point The point to create.
 */
void PointCreate(unsigned int X, unsigned int Y, TPoint *Pointer_Output_Point);

/** Free a previously created point.
 * @param Pointer_Point The point to delete.
 */
void PointFree(TPoint *Pointer_Point);

/** Check if two points are equal.
 * @param Pointer_Point_A First point.
 * @param Pointer_Point_B Second point.
 * @return 1 if they are equal or 0 otherwise.
 */
int PointIsEqual(TPoint *Pointer_Point_A, TPoint *Pointer_Point_B);

/** Copy a point into another point.
 * @param Pointer_Source_Point Source.
 * @param Pointer_Destination_Point Destination.
 * @warning The two points must have been created by the user.
 */
void PointCopy(TPoint *Pointer_Source_Point, TPoint *Pointer_Destination_Point);

/** Display the coordinates of a point.
 * @param Pointer_Point The point to display.
 */
void PointShow(TPoint *Pointer_Point);

/** Set a point coordinates to zero.
 * @param Pointer_Point The point to reset.
 */
void PointClear(TPoint *Pointer_Point);

#endif
