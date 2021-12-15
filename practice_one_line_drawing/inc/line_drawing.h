#ifndef LINE_DRAWING_H
#define LINE_DRAWING_H

#include "ppm_processor.h"
#include "math.h"
#include <pthread.h>

typedef struct line_drawing_point
{
    int x;
    int y;
} line_drawing_point;

void line_drawing_draw_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_vertical_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_horizontal_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_line_dda(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_line_bresenham(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_line_bresenham_independent_x(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void line_drawing_draw_line_bresenham_independent_y(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
line_drawing_point *line_drawing_get_left_point(line_drawing_point *point_one, line_drawing_point *point_two);
line_drawing_point *line_drawing_get_right_point(line_drawing_point *point_one, line_drawing_point *point_two);
line_drawing_point *line_drawing_get_bottom_point(line_drawing_point *point_one, line_drawing_point *point_two);
line_drawing_point *line_drawing_get_top_point(line_drawing_point *point_one, line_drawing_point *point_two);

#endif