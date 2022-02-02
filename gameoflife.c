/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				Rixiao Zhang
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Return the nth bit of x.
unsigned get_bit(uint8_t x,
                 uint8_t n){
    return (x >> n) & 1;
}

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
    Color *old_color =  (image->image)[image->cols * row + col];
    Color *new_color = (Color *) malloc (sizeof(Color));
    
    
    if (NULL == new_color) {
        return NULL;
    }
    
    new_color->R = 0;
    new_color->G = 0;
    new_color->B = 0;
    
    uint32_t curr_row_index;
    uint32_t curr_col_index;
    uint32_t num_row = image->rows;
    uint32_t num_col = image->cols;
    Color **handle = image->image;
    int red_live[8]   = {0,0,0,0,0,0,0,0};
    int green_live[8] = {0,0,0,0,0,0,0,0};
    int blue_live[8]  = {0,0,0,0,0,0,0,0};


    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if ( i != 0 || j != 0) {
                curr_row_index = (row + i + num_row) % num_row;
                curr_col_index = (col + j + num_col) % num_col;
                Color *neighbor_color = handle[num_col * curr_row_index + curr_col_index];
                uint8_t neighbor_R = neighbor_color -> R;
                uint8_t neighbor_G = neighbor_color -> G;
                uint8_t neighbor_B = neighbor_color -> B;
                for (uint8_t k = 0; k < 8; k++) {
                    red_live[k]   += get_bit(neighbor_R,k);
                    green_live[k] += get_bit(neighbor_G,k);
                    blue_live[k]  += get_bit(neighbor_B,k);
                }
            }
        }
    }
    
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t curr_state_R = get_bit(old_color->R, i);
        uint8_t curr_state_G = get_bit(old_color->G, i);
        uint8_t curr_state_B = get_bit(old_color->B, i);
        
        uint32_t offset_R = curr_state_R == 0 ? red_live[i]   : red_live[i]   + 9;
        uint32_t offset_G = curr_state_G == 0 ? green_live[i] : green_live[i] + 9;
        uint32_t offset_B = curr_state_B == 0 ? blue_live[i]  : blue_live[i]  + 9;
        
        uint8_t state_R = (rule & (1 << offset_R)) >> offset_R;
        uint8_t state_G = (rule & (1 << offset_G)) >> offset_G;
        uint8_t state_B = (rule & (1 << offset_B)) >> offset_B;

        new_color->R += (state_R << i);
        new_color->G += (state_G << i);
        new_color->B += (state_B << i);
    }
    
    return new_color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
    Image *new_image = (Image *) malloc (sizeof(Image));
    if (NULL == new_image) {
        printf("malloc fails to allocate memory for Image struct in gamelife.\n");
        free(image);
        exit(-1);
    }
    
    new_image->rows = image->rows;
    new_image->cols = image->cols;
    int num_pixel = image->rows * image->cols;
    
    Color **handel = (Color **) malloc (num_pixel * sizeof(Color *));
    if (NULL == handel) {
        printf("malloc fails to allocate memory for handle in gamelife.\n");
        free(image);
        free(new_image);
        exit(-1);
    }
    
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Color *curr = evaluateOneCell(image, i, j, rule);
            if (NULL == curr) {
                printf("malloc fails to allocate memory for %d th Color struct in gamelife.\n", image->cols * i + j);
                freeColors(handel, image->cols * i + j - 1);
                free(image);
                free(handel);
                free(new_image);
                exit(-1);
            }
            handel[image->cols * i + j] = curr;
        }
    }
    
    new_image->image = handel;
    return new_image;
}

void processCLI(int argc, char **argv, char **filename, uint32_t *rule)
{
    // $ ./gameOfLife blinkerH.ppm 0x1808 > blinkerV.ppm
    // argc is greater than 3 in this case. or do we need to care it?
    if (argc < 3) {
        printf("usage: %s filename rule\n",argv[0]);
        printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
        printf("rule is a hex number beginning with 0x; Life is 0x1808.");
        exit(-1);
    }
    
    *filename = argv[1];
    *rule = (uint32_t) strtol(argv[2], NULL, 16);
}


/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
    char *filename;
    uint32_t rule = 0;
    uint32_t *rulePtr = &rule;
    processCLI(argc, argv, &filename, rulePtr);
    Image *old_image = readData(filename);
    Image *new_image = life(old_image, *rulePtr);
    writeData(new_image);
    freeImage(old_image);
    freeImage(new_image);
    return 0;
}
