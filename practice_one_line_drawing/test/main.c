#include <stdio.h>
#include "ppm_processor.h"

void test_ppm_processor();
int paint_it_black(int width_index, int height_index;

int main(int argc, char *argv[])
{
    test_ppm_processor();

    return 0;
}

void test_ppm_processor(){
    ppm_processor_canvas *canvas = ppm_processor_get_canvas(500, 500);
    int height_index, width_index;
    for(height_index = 0; height_index < canvas->height; height_index++){
        for(width_index = 0; width_index < canvas->width; width_index++){
            if(paint_it_black(width_index, height_index)){
                ppm_processor_pixel *pixel = get_pixel(canvas, width_index, height_index);
                pixel->red = 0;
                pixel->green = 0;
                pixel->blue = 0;
            }
        }
    }
    ppm_processor_print_image(canvas, "output.ppm");
}

int paint_it_black(int width_index, int height_index){
    if (width_index < 50 || width_index >= 450) return 1;
    if (height_index < 50 || height_index >= 450) return 1;
    return 0;
}

void line_drawing_naive(){
    
}