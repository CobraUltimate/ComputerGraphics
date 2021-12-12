#include "line_drawing.h"

void line_drawing_draw_line_naive(line_drawing_point *start_point, line_drawing_point *end_point, ppm_processor_canvas *canvas){
    double my = (double)(end_point->y - start_point->y); 
    double mx = (double)(end_point->x - start_point->x);
    double m = my/mx;
    double b = start_point->y - (m * start_point->x);
    int x;
    for(x = start_point->x; x <= end_point->x; x++){
        int y = m * x + b;
        ppm_processor_pixel *pixel = get_pixel(canvas, x, y);
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
    }
}