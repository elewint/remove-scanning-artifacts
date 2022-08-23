/**************************************************************
 *
 *                     uarray2.h
 *
 *     Assignment: iii
 *     Authors:  Katie Yang (zyang11), Eli Intriligator (eintri01)
 *     Date:     Sep 27, 2021
 *
 *     Summary
 *     The UArray2 interface is an abstraction that implements
 *     two-dimensional, polymorphic, unboxed arrays. The abstraction
 *     is based on Dave Hanson's one-dimensional unboxed array, UArray.
 *
 **************************************************************/

#ifndef __UARRAY2__
#define __UARRAY2__

#include <stdio.h>
#include <stdlib.h>
#include <uarray.h>
#include <assert.h>
#define T UArray2_T

typedef struct T *T;

/* UArray2_new
 * Purpose: Creates a new UArray2 of a given width and height that
 *          can store elements of the given size.
 * Parameters: integers representing width and height of the UArray2, as
 *             well as a size_t representing the size of each element
 *             that will be stored in the UArray2
 * Returns: the new UArray2
 * Expected input: a width, height, and size that are all greater than 0
 * Success output: a new UArray2 is returned
 * Failure output: if the width, height, or size are invalid, a Hanson CRE
 *                 is raised
 */
T UArray2_new(int width, int height, size_t size);

/* UArray2_at
 * Purpose: Returns a pointer to the target element of a given UArray2
 * Parameters: The UArray2, two integers for the column and row the target
 *             element is at
 * Returns: a void pointer to the target element
 * Expected input: a valid UArray2 and a column and row that are both within
 *                 the bounds of the UArray2
 * Success output: a void pointer to the target element is returned
 * Failure output: if the UArray2 is null, or if the column and row are not
 *                 within the bounds of the UArray2, a Hanson CRE is raised
 */
void *UArray2_at(T uarray2, int col, int row);

/* UArray2_width
 * Purpose: Returns the width of a given UArray2
 * Parameters: the UArray2
 * Returns: the width of the given UArray2 as an integer
 * Expected input: a valid UArray2
 * Success output: returns the width of the UArray2
 * Failure output: if the UArray2 is null, a Hanson CRE is raised
 */
int UArray2_width(T uarray2);

/* UArray2_height
 * Purpose: Returns the height of a given UArray2
 * Parameters: the UArray2
 * Returns: the height of the given UArray2 as an integer
 * Expected input: a valid UArray2
 * Success output: returns the height of the UArray2
 * Failure output: if the UArray2 is null, a Hanson CRE is raised
 */
int UArray2_height(T uarray2);

/* UArray2_size
 * Purpose: Returns the size of each element in a given UArray2 in bytes
 * Parameters: the UArray2
 * Returns: int representing the size of a UArray2 element
 * Expected input: a valid UArray2
 * Success output: returns the size of an element of the UArray2
 * Failure output: if the UArray2 is null, a Hanson CRE is raised
 */
int UArray2_size(T uarray2);

/* UArray2_map_col_major
 * Purpose: Traverse a given UArray2 column by column starting from 
 *             the top left element, calling the apply function on each
 *             element and building up the closure value across iterations
 * Parameters: the UArray2, function pointer to the function to apply to
 *             each element, and void pointer to the closure value that is
 *             aggregated across the traversal
 * Returns: none
 * Expected input: a valid UArray2, a function pointer with matching
 *                 apply function parameters, and either a closure
 *                 pointer or a null closure parameter
 * Success output: none
 * Failure output: if either the UArray2 or the apply function are null, a
 *                 Hanson CRE is raised
 *           Note: The apply function parameters are ints representing the
 *                 column and row of the current UArray2 element, as well
 *                 as the current UArray2, a void pointer to the current
 *                 value, and the closure parameter as a void pointer
 */
void UArray2_map_col_major(T uarray2, void apply(int col, int row, T a,
                                        void *p1, void *p2), void *cl);

/* UArray2_map_row_major
 * Purpose: Traverse a given UArray2 row by row starting from 
 *             the top left element, calling the apply function on each
 *             element and building up the closure value across iterations
 * Parameters: the UArray2, function pointer to the function to apply to
 *             each element, and void pointer to the closure value that is
 *             aggregated across the traversal
 * Returns: none
 * Expected input: a valid UArray2, a function pointer with matching
 *                 apply function parameters, and either a closure
 *                 pointer or a null closure parameter
 * Success output: none
 * Failure output: if either the UArray2 or the apply function are null, a
 *                 Hanson CRE is raised
 *           Note: The apply function parameters are ints representing the
 *                 column and row of the current UArray2 element, as well
 *                 as the current UArray2, a void pointer to the current
 *                 value, and the closure parameter as a void pointer
 */
void UArray2_map_row_major(T uarray2, void apply(int col, int row, T a,
                                        void *p1, void *p2), void *cl);

/* UArray2_free
 * Purpose: Frees memory associated with a given UArray2, including the
 *          elements stored inside of it.
 * Parameters: a pointer to the UArray2 to free
 *    Returns: void
 * Expected input: non-null pointer to a valid UArray2
 * Success output: none
 * Failure output: if either the UArray2 pointer or the UArray2 itself
 *                 are null, a Hanson CRE is raised
 */
void UArray2_free(T *uarray2);

#undef T
#endif /* __UARRAY2_ */