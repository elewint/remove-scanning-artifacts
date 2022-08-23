/**************************************************************
 *
 *                     bit2.c
 *
 *     Assignment: iii
 *     Authors:  Katie Yang (zyang11), Eli Intriligator (eintri01)
 *     Date:     Oct 4, 2021
 *
 *     Summary
 *       Implementation of the bit2 interface. Based on David Hanson's
 *       unboxed array, this file contains functions for making 2D
 *       bitmaps, getting its width and height, getting a bit at
 *       a given index, and putting a bit at a given index.
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <uarray.h>
#include <bit2.h>

#define T Bit2_T

struct T {
    UArray_T col_uarray;
    int width;
    int height;
};

/* Bit2_new
 * Purpose: Creates a new Bit2 of a given width and height
 * Parameters: integers representing width and height of the Bit2
 * Returns: the new Bit2
 * Expected input: a width and height that are both greater than 0
 * Success output: a new Bit2 bitmap is returned
 * Failure output: if the width or height are invalid, a Hanson CRE
 *                 is raised
 */
T Bit2_new(int width, int height){
    assert (width > 0 && height > 0);

    T new_bit2 = malloc(sizeof(struct T));
    new_bit2->width = width;
    new_bit2->height = height;

    UArray_T col_uarray = UArray_new(width, sizeof(Bit_T));
    assert (col_uarray != NULL);
    
    for (int i = 0; i < width; i++) {
        Bit_T *empty_column = UArray_at(col_uarray, i);
        Bit_T bit_column = Bit_new(height);
        *empty_column = bit_column;
    }
    
    new_bit2->col_uarray = col_uarray;

    return new_bit2;
}

/* Bit2_get
 * Purpose: Get the value stored in target location
 * Parameters: the Bit2 and two ints for the column and row of the
 *             target bit
 * Returns: an int representing the target bit's stored value
 * Expected input: a valid Bit2 and a column and row that are both within
 *                 the bounds of the Bit2
 * Success output: the value of the desired bit is returned
 * Failure output: if the Bit2 is null, or if the column and row are not
 *                 within the bounds of the Bit2, a Hanson CRE is raised
 */
int Bit2_get(T bit2, int col, int row){
    assert (bit2 != NULL);
    assert (col < bit2->width && col >= 0);
    assert (row < bit2->height && row >= 0);
    
    Bit_T *bit_column = UArray_at(bit2->col_uarray, col);

    return Bit_get(*bit_column, row);
}

/* Bit2_put
 * Purpose: Store value in target location in a Bit2
 * Parameters: the Bit2 and two ints for the column and row of where
 *             new bit will be placed, as well as the new bit value
 * Returns: the previous value stored in the target location
 * Expected input: a valid Bit2 and a column and row that are both within
 *                 the bounds of the Bit2, as well as a value of 0 or 1
 * Success output: returns the previous value stored at the target location
 * Failure output: if the Bit2 is null, or if the column and row are not
 *                 within the bounds of the Bit2, or if the value is not a
 *                 0 or a 1, a Hanson CRE is raised
 */
int Bit2_put(T bit2, int col, int row, int value){
    assert (bit2 != NULL);
    assert (col < bit2->width && col >= 0);
    assert (row < bit2->height && row >= 0);
    assert (value == 0 || value == 1);
    
    Bit_T *bit_column = UArray_at(bit2->col_uarray, col);

    return Bit_put(*bit_column, row, value);
}

/* Bit2_width
 * Purpose: Returns the width of a given Bit2
 * Parameters: the Bit2
 * Returns: the width of the Bit2 as an integer
 * Expected input: a valid Bit2
 * Success output: returns the width of the Bit2
 * Failure output: if the Bit2 is null, a Hanson CRE is raised
 */
int Bit2_width(T bit2)
{
    assert (bit2 != NULL);
    
    return bit2->width;
}

/* Bit2_height
 * Purpose: Returns the height of a given Bit2
 * Parameters: the Bit2
 * Returns: the height of the Bit2 as an integer
 * Expected input: a valid Bit2
 * Success output: returns the height of the Bit2
 * Failure output: if the Bit2 is null, a Hanson CRE is raised
 */
int Bit2_height(T bit2)
{
    assert (bit2 != NULL);
    
    return bit2->height;
}

/* Bit2_map_col_major
 * Purpose: Traverse a given Bit2 column by column starting from 
 *             the top left element, calling the apply function on each
 *             element and building up the closure value across iterations
 * Parameters: the Bit2, function pointer to the function to apply to
 *             each element, and void pointer to the closure value that is
 *             aggregated across the traversal
 * Returns: none
 * Expected input: a valid Bit2, a function pointer with matching
 *                 apply function parameters, and either a closure
 *                 pointer or a null closure parameter
 * Success output: none
 * Failure output: if either the Bit2 or the apply function are null, a
 *                 Hanson CRE is raised
 *           Note: The apply function parameters are ints representing the
 *                 column and row of the current bit, as well as the current
 *                 Bit2 bitmap, the current value as an integer, and the
 *                 closure parameter as a void pointer
 */
void Bit2_map_col_major(T bit2, void apply(int col, int row, T a,
                                        int b, void *p1), void *cl)
{
    assert (bit2 != NULL && apply != NULL);
    
    for (int col = 0; col < bit2->width; col++) {
        Bit_T *bit_column = UArray_at(bit2->col_uarray, col);
        
        for (int row = 0; row < bit2->height; row++) {
            int curr_bit = Bit_get(*bit_column, row);
            
            apply(col, row, bit2, curr_bit, cl);
        }
    } 
}

/* Bit2_map_row_major
 * Purpose: Traverse a given Bit2 row by row starting from 
 *             the top left element, calling the apply function on each
 *             element and building up the closure value across iterations
 * Parameters: the Bit2, function pointer to the function to apply to
 *             each element, and void pointer to the closure value that is
 *             aggregated across the traversal
 * Returns: none
 * Expected input: a valid Bit2, a function pointer with matching
 *                 apply function parameters, and either a closure
 *                 pointer or a null closure parameter
 * Success output: none
 * Failure output: if either the Bit2 or the apply function are null, a
 *                 Hanson CRE is raised
 *           Note: The apply function parameters are ints representing the
 *                 column and row of the current bit, as well as the current
 *                 Bit2 bitmap, the current value as an integer, and the
 *                 closure parameter as a void pointer
 */
void Bit2_map_row_major(T bit2, void apply(int col, int row, T a,
                                        int b, void *p1), void *cl)
{
    assert (bit2 != NULL && apply != NULL);
    
    for (int row = 0; row < bit2->height; row++) {
        for (int col = 0; col < bit2->width; col++) {
            Bit_T *bit_column = UArray_at(bit2->col_uarray, col);
            int curr_bit = Bit_get(*bit_column, row);
            
            apply(col, row, bit2, curr_bit, cl);
        }
    } 
}

/* Bit2_free
 * Purpose: Frees memory associated with a given Bit2, including the
 *          elements stored inside of it.
 * Parameters: a pointer to the Bit2 to free
 * Returns: void
 * Expected input: non-null pointer to a valid Bit2
 * Success output: none
 * Failure output: if either the Bit2 pointer or the Bit2 itself are null,
 *                 a Hanson CRE is raised
 */
void Bit2_free(T *bit2){
    assert (*bit2 != NULL && bit2 != NULL);

    for (int col = 0; col < (*bit2)->width; col++) {
        Bit_T *bit_column = UArray_at((*bit2)->col_uarray, col);
        Bit_free(bit_column);
    }
    
    UArray_free(&(*bit2)->col_uarray);
    free(*bit2);
}
