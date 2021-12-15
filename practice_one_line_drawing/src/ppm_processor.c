#include "ppm_processor.h"

ppm_processor_canvas *ppm_processor_get_canvas(int width, int height){
    ppm_processor_canvas *canvas = (ppm_processor_canvas *)malloc(sizeof(ppm_processor_canvas));
    ppm_processor_pixel **pixels = (ppm_processor_pixel **)malloc(sizeof(ppm_processor_pixel *) * height);
    int height_index,width_index;
    for(height_index = 0; height_index < height; height_index++){
        ppm_processor_pixel *pixels_row = (ppm_processor_pixel *)malloc(sizeof(ppm_processor_pixel) * width);
        for(width_index = 0; width_index < width; width_index++){
            ppm_processor_pixel *pixel = pixels_row + width_index;
            pixel->red = 255;
            pixel->green = 255;
            pixel->blue = 255;
        }
        *(pixels + height_index) = pixels_row;
    }
    canvas->pixels = pixels;
    canvas->height = height;
    canvas->width = width;
    return canvas;
}

ppm_processor_pixel *ppm_processor_get_pixel_relative(ppm_processor_canvas *canvas, int x, int y){
    int width_index = ((canvas->width / 2) - 1) + x;
    int height_index = ((canvas->height / 2) - 1) - y;
    return ppm_processor_get_pixel_absolute(canvas, width_index, height_index);
}

ppm_processor_pixel *ppm_processor_get_pixel_absolute(ppm_processor_canvas *canvas, int width_index, int height_index){
    ppm_processor_pixel *pixels_row = *(canvas->pixels + height_index);
    return pixels_row + width_index;
}

void ppm_processor_print_image(ppm_processor_canvas *canvas, const char *file_name){
    FILE *output = fopen(file_name,"w");
    fprintf(output,"P3\n");
    fprintf(output,"%d %d\n", canvas->width, canvas->height);
    fprintf(output,"255\n");
    int height_index, width_index;
    for(height_index = 0; height_index < canvas->height; height_index++){
        for(width_index = 0; width_index < canvas->width; width_index++){
            ppm_processor_pixel *pixel = ppm_processor_get_pixel_absolute(canvas, width_index, height_index);
            fprintf(output,"%d %d %d\n", pixel->red, pixel->green, pixel->blue);
        }
    }
}

void ppm_processor_paint_pixel(ppm_processor_pixel *pixel, int red, int green, int blue){
    pixel->red = red;
    pixel->green = green;
    pixel->blue = blue;
}
