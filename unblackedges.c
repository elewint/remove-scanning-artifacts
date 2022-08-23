/**************************************************************
 *
 *         unblackedges – remove the black edge pixels
 *                           of a given pbm file
 *
 *     Assignment: iii
 *     Authors:  Katie Yang (zyang11), Eli Intriligator (eintri01)
 *     Date:     Oct 4, 2021
 *
 *     Input:
 *       A valid pbm file with black edges via arg or stdin
 *
 *     Success output:
 *       The pbm without black edges is written to stdout
 *
 *     Failure output:
 *       A Hanson checked runtime exception is raised if
 *       there is a problem accessing or reading the input,
 *       or if the input pbm is invalid. In this case,
 *       no other input is written to stdout.
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include <bit2.h>
#include <seq.h>
#include <pnmrdr.h>

FILE * OpenFile(int argc, char *argv[]);

Bit2_T pbmread(FILE *fp);

void populate_bit2(Bit2_T bitmap, int line_length, short *line_data, 
                                                      int line_num);
void remove_black_edges(Bit2_T bitmap);
void restore_neighbors(Bit2_T bitmap, Seq_T neighbor_queue);
void find_neighbors(Bit2_T bitmap, Bit2_T queued_map, int col, int row);
void queue_neighbors(Bit2_T bitmap, Bit2_T queued_map, Seq_T neighbor_queue,
                                                int curr_col, int curr_row);
void queue_neighbor(Seq_T neighbor_queue, Bit2_T queued_map, int col, 
                                                            int row);

void pbmwrite(Bit2_T bitmap);

typedef struct BlackPixelData *BlackPixelData;

struct BlackPixelData {
    int col;
    int row;
};

int main(int argc, char *argv[])
{
    FILE *fp = OpenFile(argc, argv);
    
    Bit2_T bitmap = pbmread(fp);
    
    remove_black_edges(bitmap);
    
    pbmwrite(bitmap);

    Bit2_free(&bitmap);
    fclose(fp);

    return 0;
}

/* pbmread
 *    Purpose: Store information from a pbm file in a new bitmap
 * Parameters: a file pointer to the input stream (a file or stdin)
 *    Returns: a bitmap populated with information from the file stream
 *    Expected input: a pointer to a filestream
 *    Expected output: populated bitmap should be returned
 * Errors: Throws a CRE if the pbm's width or height are less than 1, or if
 *         the maxval is anything other than 1
 */
Bit2_T pbmread(FILE *fp)
{
    Pnmrdr_T rdr = Pnmrdr_new(fp);
    Pnmrdr_mapdata data = Pnmrdr_data(rdr);
    assert(data.type == 1);

    unsigned width = data.width;
    unsigned height = data.height;
    assert(width > 0 && height > 0 && data.denominator == 1);

    Bit2_T bitmap = Bit2_new(width, height);
    
    for (unsigned int i = 0; i < height; i++) {
        short *line_data;
        
        line_data = malloc(width * sizeof(short));
        assert(line_data != NULL);
        
        unsigned input_num;
        
        for (unsigned int j = 0; j < width; j++){
            input_num = Pnmrdr_get(rdr);
            assert(input_num == 0 || input_num == 1);

            line_data[j] = input_num;
        }
        
        populate_bit2(bitmap, width, line_data, i);
        free(line_data);
    }
    
    Pnmrdr_free(&rdr);

    return bitmap;
}

 /* populate_bit2
  *    Purpose: Populate a line of the bitmap with values from the input file
  * Parameters: the bitmap, the length of line, a short pointer for a buffer
  *             that stores the input data, and an int for the taget line
  *             number in the bitmap
  *    Returns: void
  *
  */
void populate_bit2(Bit2_T bitmap, int line_length, short *line_data, 
                                                       int line_num)
{
    for (int i = 0; i < line_length; i++) {
        short curr_num = line_data[i];
        if (curr_num == 1) {
            Bit2_put(bitmap, i, line_num, curr_num);
        }
    }
}

 /* remove_black_edges
  *    Purpose: Remove the black edges
  * Parameters: A bitmap with all the pixels
  *    Returns: void
  */
void remove_black_edges(Bit2_T bitmap)
{
    int width = Bit2_width(bitmap);
    int height = Bit2_height(bitmap);

    Bit2_T queued_map = Bit2_new(width, height);

    /* Loop along top edge */
    for (int i = 0; i < width; i++) {
        if (Bit2_get(bitmap, i, 0) == 1) {
            find_neighbors(bitmap, queued_map, i, 0);
        }
    }

    /* Loop along right edge */
    for (int i = 0; i < height; i++) {
        if (Bit2_get(bitmap, width - 1, i) == 1) {
            find_neighbors(bitmap, queued_map, width - 1, i);
        }
    }

    /* Loop along bottom edge */
    for (int i = 0; i < width; i++) {
        if (Bit2_get(bitmap, i, height - 1) == 1) {
            find_neighbors(bitmap, queued_map, i, height - 1);
        }
    }

    /* Loop along left edge */
    for (int i = 0; i < height; i++) {
        if (Bit2_get(bitmap, 0, i) == 1) {
            find_neighbors(bitmap, queued_map, 0, i);
        }
    }
    Bit2_free(&queued_map);
}

 /* find_neighbors
  *    Purpose: Find the neighbors of the current pixel
  * Parameters: A bitmap storing the pixels, a bitmap storing whether or not
  *             the pixels have been queued, an int for the column number,
  *             and an int for the row number
  *    Returns: void
  */
void find_neighbors(Bit2_T bitmap, Bit2_T queued_map, int col, int row)
{
    Seq_T neighbor_queue = Seq_new(100);
    assert (neighbor_queue != NULL);

    BlackPixelData bpd = malloc(sizeof(struct BlackPixelData));
    assert (bpd != NULL);

    bpd->col = col;
    bpd->row = row;

    Seq_addhi(neighbor_queue, bpd);
    
    while (Seq_length(neighbor_queue) != 0) {
        BlackPixelData curr_pixel = Seq_remlo(neighbor_queue);;
        int col = curr_pixel->col;
        int row = curr_pixel->row;
        free(curr_pixel);
        Bit2_put(bitmap, col, row, 0);
        queue_neighbors(bitmap, queued_map, neighbor_queue, col, row);
    }
    
    Seq_free(&neighbor_queue);
}

/* queue_neighbors
  *    Purpose: Queue a given pixel's black neighbors
  * Parameters: A bitmap storing the pixels, a bitmap storing whether or not
  *             the pixels have been queued, a sequence storing a queue of 
  *             pixels to process, an int for the column number, and an
  *             int for the row number
  *    Returns: void
  */
void queue_neighbors(Bit2_T bitmap, Bit2_T queued_map, Seq_T neighbor_queue,
                                               int curr_col, int curr_row)
{
    /* Above neighbor */
    if (curr_row != 0 && Bit2_get(bitmap, curr_col, curr_row - 1) == 1 &&
                        Bit2_get(queued_map, curr_col, curr_row - 1) == 0) {
        queue_neighbor(neighbor_queue, queued_map, curr_col, curr_row - 1);
    }

    /* Right neighbor */
    if (curr_col != Bit2_width(bitmap) - 1
                        && Bit2_get(bitmap, curr_col + 1, curr_row) == 1
                        && Bit2_get(queued_map, curr_col + 1, curr_row) == 0) {
        queue_neighbor(neighbor_queue, queued_map, curr_col + 1, curr_row);
    }

    /* Below neighbor */
    if (curr_row != Bit2_height(bitmap) - 1
                        && Bit2_get(bitmap, curr_col, curr_row + 1) == 1
                        && Bit2_get(queued_map, curr_col, curr_row + 1) == 0) {
        queue_neighbor(neighbor_queue, queued_map, curr_col, curr_row + 1);
    }

    /* Left neighbor */
    if (curr_col != 0 && Bit2_get(bitmap, curr_col - 1, curr_row) == 1
                      && Bit2_get(queued_map, curr_col - 1, curr_row) == 0) {
        queue_neighbor(neighbor_queue, queued_map, curr_col - 1, curr_row);
    }
}

/* queue_neighbor
  *    Purpose: Enqueue a given black pixel to the neighbor queue
  * Parameters: the sequence of pixels where the neighbor should be added,
  *             a bitmap storing whether or not the pixels have been queued,
  *             an int for the column number, and an int for the row number
  *    Returns: void
  */
void queue_neighbor(Seq_T neighbor_queue, Bit2_T queued_map, int col, int row)
{
    BlackPixelData neighbor_data = malloc(sizeof(struct BlackPixelData));
    assert(neighbor_data != NULL);

    neighbor_data->col = col;
    neighbor_data->row = row;
    Seq_addhi(neighbor_queue, neighbor_data);
    Bit2_put(queued_map, col, row, 1);
}

/* pbmwrite
  *    Purpose: print the restored portable bitmap image to standard output 
  *             with proper header
  * Parameters: A bitmap storing the pixels
  *    Returns: void
  */
void pbmwrite(Bit2_T bitmap)
{
    printf("P1\n");
    printf("%d %d\n", Bit2_width(bitmap), Bit2_height(bitmap));
    printf("1\n");

    for (int i = 0; i < Bit2_height(bitmap); i++) {
        for (int j = 0; j < Bit2_width(bitmap); j++) {
            char output_char = Bit2_get(bitmap, j, i) + '0';
            putchar(output_char);

            if (Bit2_width(bitmap) > 70 && j % 70 == 69){
                putchar('\n');
            }
        }

        putchar('\n');
    }
}

/* OpenFile
 *    Purpose: Attempts to open a file from the command line, throwing errors
 *             in the three cases described below.
 * Parameters: number of command line arguments as an int and the
 *             characters of each argument as a char array
 *    Returns: A file pointer for the file.
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
     * Set file pointer to stdin to handle command line input in the case
     * that no file is supplied
     */
    FILE *fp;
    fp = stdin;

    /*
     * If a file is supplied as an argument, open the file and make fp 
     * point to the opened filestream
     */
    if (argc == 2) {
        fp = fopen(argv[1], "r");
    }
    
    assert(fp != NULL);
    
    return fp;
}