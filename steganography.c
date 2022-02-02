/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				Rixiao Zhang
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

char *strchr(const char *s, int c);
//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
    Color *old_color =  (image->image)[image->cols * row + col];
    Color *new_color = (Color *) malloc (sizeof(Color));

    if (NULL == new_color) {
        return NULL;
    }
    
    uint8_t value = (old_color->B & 1) ? 255 : 0;
    new_color->R = value;
    new_color->G = value;
    new_color->B = value;
    return new_color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
    Image *new_image = (Image *) malloc (sizeof(Image));
    if (NULL == new_image) {
        printf("malloc fails to allocate memory for Image struct in steganography.\n");
        free(image);
        exit(-1);
    }
    
    new_image->rows = image->rows;
    new_image->cols = image->cols;
    int num_pixel = image->rows * image->cols;
    
    Color **handel = (Color **) malloc (num_pixel * sizeof(Color *));
    if (NULL == handel) {
        printf("malloc fails to allocate memory for handle in steganography.\n");
        free(image);
        free(new_image);
        exit(-1);
    }
    
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Color *curr = evaluateOnePixel(image, i, j);
            if (NULL == curr) {
                printf("malloc fails to allocate memory for %d th Color struct in steganography.\n", image->cols * i + j);
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

/*
Loads a .ppm from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/

void processCLI(int argc, char **argv, char **filename)
{
    if (argc != 2) {
        printf("usage: %s filename\n",argv[0]);
        printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
        exit(-1);
    }
    
    *filename = argv[1];
}

int main(int argc, char **argv)
{
    Image *image;
    char *filename;
    processCLI(argc, argv, &filename);
    image = readData(filename);
    Image *new_image = steganography(image);
    writeData(new_image);
    freeImage(new_image);
    freeImage(image);
    return 0;
}
