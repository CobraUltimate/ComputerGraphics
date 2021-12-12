#ifndef LINE_DRAWING_H
#define LINE_DRAWING_H

#include "ppm_processor.h"

typedef struct line_drawing_point
{
    int x;
    int y;
} line_drawing_point;


void line_drawing_draw_line_naive(line_drawing_point *start_point, line_drawing_point *end_point, ppm_processor_canvas *canvas);

#endif