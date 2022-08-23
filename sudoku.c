/**************************************************************
 *
 *           sudoku – checks the valifity of a 
 *                    nine-by-nine sudoku solution
 *
 *     Assignment: iii
 *     Authors:  Katie Yang (zyang11), Eli Intriligator (eintri01)
 *     Date:     Oct 4, 2021
 *
 *     Summary
 *     This program checks the validity of a nine-by-nine sudoku 
 *     solution
 *
 *     Input:
 *       A pgm file containing the sudoku solution
 *
 *     Success output:
 *       exit with code 0 if the solution is valid, and 1 if otherwise
 *
 *     Failure output:
 *       A Hanson checked runtime exception is raised if
 *       there is a problem accessing or reading the input,
 *       if the input pgm is invalid, if the maximum pixel
 *       intensity is not nine, pixel has zero intensity.
 *
 *
 **************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <uarray2.h>
#include <pnmrdr.h>

FILE * OpenFile(int argc, char *argv[]);

int read_data(UArray2_T uarray2, FILE *fp);
int check_pixel_val(int pixel_val);

void populate_uarray2(UArray2_T uarray2, int line_length, int *line_data,
                                                            int line_num);
void init_empty_uarray2(UArray2_T uarray2, int line_length);

int check_numbers(UArray2_T uarray2);
void calc_frequencies(int i, int j, UArray2_T a, void *p1, void *p2);
int find_submap_index(int col, int row);

typedef struct FrequencyData *FrequencyData;
struct FrequencyData {
    UArray2_T frequency_array;
    int solved;
};

int main(int argc, char *argv[])
{
    FILE *fp = OpenFile(argc, argv);
    
    UArray2_T uarray2;
    uarray2 = UArray2_new(9, 9, sizeof(int));

    /* if file represents a wrong sudoku solution */
    if (read_data(uarray2, fp) == 1 || check_numbers(uarray2) == 1) {
        UArray2_free(&uarray2);
        fclose(fp);
        return 1;
    }

    /* if file represents a solved sudoku puzzle */
    UArray2_free(&uarray2);
    fclose(fp);

    return 0;
}

/* OpenFile
 *    Purpose: Attempts to open a file from the command line, throwing errors
 *             in the three cases described below.
 * Parameters: number of command line arguments as an int and the
 *             characters of each argument as a char array
 *    Returns: A file pointer for input stream
 * Expected input: number of command line arguments, and characters of each 
 *                 argument
 * Success output: a file pointer for a file or standard input
 * Failure output: if an argument is supplied and fails to open, a Hanson CRE
 *                 is raised
 *
 *       Note: Throws a checked runtime error if
 *             1. More than one argument is supplied
 *             2. The named input file cannot be opened
 *             3. An error is encountered reading from an input file
 */
FILE *OpenFile(int argc, char *argv[])
{
    assert(argc <= 2);

    /*
     * set file pointer to stdin to handle command line input in the case
     * that no file is supplied
    */
    FILE *fp;
    fp = stdin;

    /*
     * if a file is supplied as an argument, open the file and make fp 
     * point to the opened filestream
    */
    if (argc == 2) {
        fp = fopen(argv[1], "r");
    }
    
    assert(fp != NULL);
    
    return fp;
}

/* read_data
 *    Purpose: Read in the data with pnm reader to populate a uarray2.
 * Parameters: The uarray2 to store information in, and a file pointer for
 *             input stream.
 *    Returns: None
 * Expected input: a valid uarray2
 * Success output: an exit code for whether or not the sukudo is valid
 * Failure output: if width, height, or mavel isn't 9, less than 81 pixels
 *                 are stored in the pbm, and the filestream is not a valid 
 *                 pgm (P2 or P5), a Hanson CRE is raised
 */
int read_data(UArray2_T uarray2, FILE *fp){
    Pnmrdr_T rdr = Pnmrdr_new(fp);
    Pnmrdr_mapdata data = Pnmrdr_data(rdr);
    assert(data.type == 2);

    unsigned width = data.width;
    unsigned height = data.height;
    assert(width == 9 && height == 9 && data.denominator == 9);
    
    int invalid_digit = 0;

    for (int i = 0; i < 9; i++) {
        int *line_data;
        
        line_data = malloc(9 * sizeof(int));
        assert(line_data != NULL);
        
        unsigned digit;
        
        for (int j = 0; j < 9; j++){
            digit = Pnmrdr_get(rdr);
            line_data[j] = digit;
            if (check_pixel_val(digit) == 1) {
                invalid_digit = 1;
            }
        }
        
        populate_uarray2(uarray2, 9, line_data, i);
        free(line_data);
    }
    
    Pnmrdr_free(&rdr);

    if (invalid_digit) {
        return 1;
    } else {
        return 0;
    }
}

/* check_pixel_val
 *    Purpose: Check if the pixel value is in scope
 * Parameters: an int for the pixel value
 *    Returns: an int for whether or not the current pixel value is in sope
 * Expected input: a pixel's intensity
 * Success output: an exit code for whether or not it is a valid sudoku
 * Failure output: none
 *
 *       Note: In scope means its value is 0 to 9, inclusive
 */
int check_pixel_val(int pixel_val)
{
    if (pixel_val < 1 || pixel_val > 9) {
        return 1;
    } else {
        return 0;
    }
}

/* populate_uarray2
 *    Purpose: Populate a line in the uarray2
 * Parameters: the uarray2, the length of line, an int pointer for a buffer
 *             that stores the information, and a int for the taget line
 *             number in the uarray2
 *    Returns: void
 * Expected input: a valid uarray2 , the length of a line, all the stored 
 *                 informaton, and the number of elments in a line
 * Success output: none
 * Failure output: none
 *
 */
 void populate_uarray2(UArray2_T uarray2, int line_length, int *line_data,
                                                            int line_num)
{
    for (int i = 0; i < line_length; i++) {
        int curr_num = line_data[i];
        *((int *)UArray2_at(uarray2, i, line_num)) = curr_num;
    }
}

/* init_empty_uarray2
 *    Purpose: Populate an empty uarray2
 * Parameters: an uarray2, an int for the line length
 *    Returns: void
 * Expected input: a valid uarray2 and the length of a line
 * Success output: none
 * Failure output: if memory allocation failed, a Hanson CRE is raised
 */
void init_empty_uarray2(UArray2_T uarray2, int line_length)
{
    // initialize everything in frequency_array to zero
    int *line_data = malloc(27 * sizeof(int));
    assert(line_data != NULL);
        
    for (int j = 0; j < line_length; j++){
        line_data[j] = 0;
    }

    for (int i = 0; i < UArray2_height(uarray2); i++) {
        populate_uarray2(uarray2, line_length, line_data, i);
    }

    free(line_data);
}

/* check_numbers
 *    Purpose: Check if the nine-by-nine submaps  sudoku is valid
 * Parameters: an uarray2 that stores the information
 *    Returns: an int for whether or not the sudoky is valid
 * Expected input: a valid uarray2 storing the information
 * Success output: an exit code for whether or not it is a valid sudoku
 * Failure output: none
 *
 *       Note: A sudoku is valid when
 *             1. No pixels have the same number in each row.
 *             2. No pixels have the same number in each column.
 *             3. In each of three-by-three submaps, no two pixels have 
 *                the same number.
 */
int check_numbers(UArray2_T uarray2){
    FrequencyData freq_data = malloc(sizeof(struct FrequencyData));

    /* Assume sudoku is solved until proven wrong */
    freq_data->solved = 1;
    freq_data->frequency_array = UArray2_new(9, 27, sizeof(int));

    init_empty_uarray2(freq_data->frequency_array, 9);

    UArray2_map_col_major(uarray2, calc_frequencies, freq_data);

    printf("SOLVED IS %d\n", freq_data->solved);
    int solved = freq_data->solved;

    /* Free frequency data on the heap */
    UArray2_free(&freq_data->frequency_array);
    free(freq_data);

    return solved;
}

/* calc_frequencies
 *    Purpose: Calculate the frequency of a number's apperance in 
 *             column, row, and submap using folding
 * Parameters: an int for the column number, and int for the row number,
 *             an urray2, a void pointer for the current element that
 *             needs to be checked, and a void pointer for the closure
 *             (which will store a FrequencyTable)
 *    Returns: void
 * Expected input: column number, row number, a valid array, the current
 *                 element, and FrequencyData containing the freq_uarray
 * Success output: an exit code for whether or not it is a valid sudoku
 * Failure output: none
 *
 */
void calc_frequencies(int i, int j, UArray2_T a, void *p1, void *p2)
{
    assert(p1 == UArray2_at(a, i, j));
    int *curr_num = p1;
    
    UArray2_T freq_uarray = ((FrequencyData)p2)->frequency_array;

    int *num_col_freq = (int *)UArray2_at(freq_uarray, *curr_num - 1, i);
    *num_col_freq += 1;

    int *num_row_freq = (int *)UArray2_at(freq_uarray, *curr_num - 1, j + 9);
    *num_row_freq += 1;

    int submap = find_submap_index(i, j);
    int *num_submap_freq = (int *)UArray2_at(freq_uarray, *curr_num - 1, 
                                                            submap + 18);
    *num_submap_freq += 1;

    if (*num_submap_freq == 2 || *num_col_freq == 2 || *num_row_freq == 2) {
        ((FrequencyData)p2)->solved = 0;
        printf("map_solved is %d\n", ((FrequencyData)p2)->solved);
    }
}

/* find_submap_index
 *    Purpose: Calculate the element's submap index for frequency_array
 * Parameters: an int for the column number and an int for the row column
 *    Returns: an int for the index
 * Expected input: column number and row number
 * Success output: the submap index
 * Failure output: none
 */
int find_submap_index(int col, int row)
{
    if (col <= 2 && row <= 2) {
        return 0;
    } else if (col <= 5 && row <= 2) {
        return 1;
    } else if (col <= 8 && row <= 2) {
        return 2;
    } else if (col <= 2 && row <= 5) {
        return 3;
    } else if (col <= 5 && row <= 5) {
        return 4;
    } else if (col <= 8 && row <= 5) {
        return 5;
    } else if (col <= 2 && row <= 8) {
        return 6;
    } else if (col <= 5 && row <= 8) {
        return 7;
    } else {
        return 8;
    }
}
