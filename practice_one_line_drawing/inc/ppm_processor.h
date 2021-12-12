#ifndef PPM_PROCESSOR_H
#define PPM_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>

typedef struct ppm_processor_pixel{
    int red;
    int green;
    int blue;
} ppm_processor_pixel;
 
typedef struct ppm_processor_canvas{
    ppm_processor_pixel **pixels;
    int height;
    int width;
} ppm_processor_canvas;

ppm_processor_canvas *ppm_processor_get_canvas(int width, int height);
ppm_processor_pixel *get_pixel(ppm_processor_canvas *canvas, int width, int height);
void ppm_processor_print_image(ppm_processor_canvas *canvas, const char *file_name);

#endif