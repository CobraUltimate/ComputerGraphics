#ifndef PARALLEL_LINE_DRAWING_H
#define PARALLEL_LINE_DRAWING_H

#include "line_drawing.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

typedef struct parallel_line_drawing_draw_line_thread_arguments {
    ppm_processor_canvas *canvas;
    line_drawing_point *start_point;
    line_drawing_point *end_point;
    void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
} parallel_line_drawing_draw_line_thread_arguments;

void parallel_line_drawing_draw_line_bresenham(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void parallel_line_drawing_draw_line_dda(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void parallel_line_drawing_draw_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void parallel_line_drawing_draw_horizontal_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void parallel_line_drawing_draw_vertical_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point);
void parallel_line_drawing_draw_line(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm_independent_x)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point), void (*line_drawing_algorithm_independent_y)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point));
void parallel_line_drawing_draw_line_independent_x(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point));
void parallel_line_drawing_draw_line_independent_y(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point));
int parallel_line_drawing_get_partition_width(line_drawing_point *start_point, line_drawing_point *end_point, int number_of_processors);
int parallel_line_drawing_get_partition_height(line_drawing_point *start_point, line_drawing_point *end_point, int number_of_processors);
void *parallel_line_drawing_draw_line_thread_wrapper(void *thread_arguments);
parallel_line_drawing_draw_line_thread_arguments *parallel_line_drawing_get_draw_line_thread_arguments(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point));

#endif