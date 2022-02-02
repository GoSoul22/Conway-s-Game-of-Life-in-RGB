/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				RIXIAO ZHANG
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

#define SCNu8 "hhu"


//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
    FILE *fp = fopen(filename, "r");
    if (NULL == fp) {
        printf("No such file.");
        exit(EXIT_FAILURE);
    }
    
    Image *fp_image = (Image *) malloc (sizeof(Image));
    if (NULL == fp_image) {
        printf("malloc fails to allocate memory for Image struct in imageloader.");
        exit(-1);
    }
    
    int scale = 0;
    char string[10];
    fscanf(fp, "%s", string);    // Format. e.g. P3
    fscanf(fp, "%d %d", &(fp_image -> cols), &(fp_image -> rows));
    fscanf(fp, "%d", &scale);    // Color scale. e.g. 0-255
    
    if (strcmp(string, "P3") || scale < 0 || scale > 255){
        printf("Invalid Input.");
        free(fp_image);
        exit(-1);
    }
    
    
    int num_of_colors = (fp_image -> cols) * (fp_image -> rows);
    Color **handle = (Color **) malloc (num_of_colors * sizeof(Color *));
    if (NULL == handle) {
        printf("malloc fails to allocate memory for handle in imageloader.");
        free(fp_image);
        exit(-1);
    }
    
    int counter;
    for (counter = 0; counter < num_of_colors; counter++) {
        Color *curr = (Color *) malloc (sizeof(Color));
        if (NULL == curr) {
            printf("malloc fails to allocate memory for %d th Color struct in imageloader.", counter);
            freeColors(handle, counter);
            free(handle);
            free(fp_image);
            exit(-1);
        }
        fscanf(fp, "%hhu %hhu %hhu", &(curr->R), &(curr->G), &(curr->B));
        handle[counter] = curr;
    }
    fp_image->image = handle;
    fclose(fp);
    return fp_image;

}

// Given a handle, which points to a list of pointers to Color struct, delete Colors.
void freeColors(Color **handle, int counter){
    for (int i = 0; i <= counter; i++) {
        Color *curr = handle[i];
        free(curr);
    }
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
    printf("P3\n");
    printf("%d %d\n", image->cols, image->rows);
    printf("255\n");
    
    Color **handle = image->image;
    int width_of_cols = 3;
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Color *curr = handle[(image->cols) * i + j];
            printf("%3d %3d %3d", curr->R, curr->G, curr->B);
            if (j < (image->cols - 1)) {
                printf("%*c", width_of_cols, ' ');
            }
        }
        printf("\n");
    }
  
}

//Frees an image
void freeImage(Image *image)
{
    if (NULL != image) {
        int num = image->cols * image->rows;
        if (image->image != NULL && num > 0){
            freeColors(image->image, num-1);
        }
        free(image->image);
        free(image);
    }
}
