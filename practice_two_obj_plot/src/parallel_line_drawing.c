#include "parallel_line_drawing.h"

void parallel_line_drawing_draw_line_bresenham(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    parallel_line_drawing_draw_line(canvas, start_point,end_point, line_drawing_draw_line_bresenham_independent_x, line_drawing_draw_line_bresenham_independent_y);
}

void parallel_line_drawing_draw_line_dda(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    parallel_line_drawing_draw_line(canvas, start_point,end_point, line_drawing_draw_line_dda, line_drawing_draw_line_dda);
}

void parallel_line_drawing_draw_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    parallel_line_drawing_draw_line(canvas, start_point,end_point, line_drawing_draw_line_naive, line_drawing_draw_line_naive);
}

void parallel_line_drawing_draw_line(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm_independent_x)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point), void (*line_drawing_algorithm_independent_y)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point)){
    int slope_x = end_point->x - start_point->x;
    if(slope_x == 0){
        parallel_line_drawing_draw_vertical_line_naive(canvas, start_point, end_point);
        return;
    }
    int slope_y = end_point->y - start_point->y;
    if(slope_y == 0){
        parallel_line_drawing_draw_horizontal_line_naive(canvas, start_point, end_point);
        return;
    }
    double slope = slope_y / slope_x;
    if(slope >= 1 || slope <= -1){
        parallel_line_drawing_draw_line_independent_y(canvas, start_point, end_point, line_drawing_algorithm_independent_y);
        return;
    }
    parallel_line_drawing_draw_line_independent_x(canvas, start_point, end_point, line_drawing_algorithm_independent_x);
}

void parallel_line_drawing_draw_horizontal_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    int number_of_processors = (int)sysconf(_SC_NPROCESSORS_ONLN);
    int partition_width = parallel_line_drawing_get_partition_width(start_point, end_point, number_of_processors);
    line_drawing_point *left_point = line_drawing_get_left_point(start_point, end_point);
    line_drawing_point *right_point = line_drawing_get_right_point(start_point, end_point);
    int processor_index;
    pthread_t threads[number_of_processors];
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        line_drawing_point line_segment_start_point, line_segment_end_point;
        line_segment_start_point.x = left_point->x + partition_width * processor_index;
        line_segment_start_point.y = left_point->y;
        line_segment_end_point.x = left_point->x + partition_width * (processor_index + 1);
        line_segment_end_point.y = left_point->y;
        if(line_segment_end_point.x > right_point->x) line_segment_end_point.x = right_point->x;
        parallel_line_drawing_draw_line_thread_arguments *thread_arguments = parallel_line_drawing_get_draw_line_thread_arguments(canvas, &line_segment_start_point, &line_segment_end_point, line_drawing_draw_horizontal_line_naive);
        pthread_create(&threads[processor_index], NULL, parallel_line_drawing_draw_line_thread_wrapper, thread_arguments);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        pthread_join(threads[processor_index], NULL);
    }
}

void parallel_line_drawing_draw_vertical_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    int number_of_processors = (int)sysconf(_SC_NPROCESSORS_ONLN);
    int partition_height = parallel_line_drawing_get_partition_height(start_point, end_point, number_of_processors);
    line_drawing_point *bottom_point = line_drawing_get_bottom_point(start_point, end_point);
    line_drawing_point *top_point = line_drawing_get_top_point(start_point, end_point);
    int processor_index;
    pthread_t threads[number_of_processors];
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        line_drawing_point line_segment_start_point, line_segment_end_point;
        line_segment_start_point.x = bottom_point->x;
        line_segment_start_point.y = bottom_point->y + partition_height * processor_index;
        line_segment_end_point.x = bottom_point->x;
        line_segment_end_point.y = bottom_point->y + partition_height * (processor_index + 1);
        if(line_segment_end_point.y > top_point->y) line_segment_end_point.y = top_point->y;
        parallel_line_drawing_draw_line_thread_arguments *thread_arguments = parallel_line_drawing_get_draw_line_thread_arguments(canvas, &line_segment_start_point, &line_segment_end_point, line_drawing_draw_vertical_line_naive);
        pthread_create(&threads[processor_index], NULL, parallel_line_drawing_draw_line_thread_wrapper, thread_arguments);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        pthread_join(threads[processor_index], NULL);
    }
}

void parallel_line_drawing_draw_line_independent_x(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point)){
    int number_of_processors = (int)sysconf(_SC_NPROCESSORS_ONLN);
    int partition_width = parallel_line_drawing_get_partition_width(start_point, end_point, number_of_processors);
    line_drawing_point *left_point = line_drawing_get_left_point(start_point, end_point);
    line_drawing_point *right_point = line_drawing_get_right_point(start_point, end_point);
    int slope_x = end_point->x - start_point->x;
    int slope_y = end_point->y - start_point->y;
    double slope = (double)slope_y / slope_x;
    int processor_index;
    pthread_t threads[number_of_processors];
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        line_drawing_point line_segment_start_point, line_segment_end_point;
        int start_point_offset_y = (int)(partition_width * processor_index * slope + 0.5);
        int end_point_offset_y = (int)(partition_width * (processor_index + 1) * slope + 0.5);
        line_segment_start_point.x = left_point->x + partition_width * processor_index;
        line_segment_start_point.y = left_point->y + start_point_offset_y;
        line_segment_end_point.x = left_point->x + partition_width * (processor_index + 1);
        line_segment_end_point.y = left_point->y + end_point_offset_y;
        if(line_segment_end_point.x > right_point->x) line_segment_end_point.x = right_point->x;
        if(slope > 0 && line_segment_end_point.y > right_point->y) line_segment_end_point.y = right_point->y;
        if(slope < 0 && line_segment_end_point.y < right_point->y) line_segment_end_point.y = right_point->y;
        parallel_line_drawing_draw_line_thread_arguments *thread_arguments = parallel_line_drawing_get_draw_line_thread_arguments(canvas, &line_segment_start_point, &line_segment_end_point, line_drawing_algorithm);
        pthread_create(&threads[processor_index], NULL, parallel_line_drawing_draw_line_thread_wrapper, thread_arguments);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        pthread_join(threads[processor_index], NULL);
    }
}

void parallel_line_drawing_draw_line_independent_y(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point)){
    int number_of_processors = (int)sysconf(_SC_NPROCESSORS_ONLN);
    int partition_height = parallel_line_drawing_get_partition_height(start_point, end_point, number_of_processors);
    line_drawing_point *bottom_point = line_drawing_get_bottom_point(start_point, end_point);
    line_drawing_point *top_point = line_drawing_get_top_point(start_point, end_point);
    int slope_x = top_point->x - bottom_point->x;
    int slope_y = top_point->y - bottom_point->y;
    double slope = (double)slope_x / slope_y;
    int processor_index;
    pthread_t threads[number_of_processors];
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        line_drawing_point line_segment_start_point, line_segment_end_point;
        int start_point_offset_x = (int)(partition_height * processor_index * slope + 0.5);
        int end_point_offset_x = (int)(partition_height * (processor_index + 1) * slope + 0.5);
        line_segment_start_point.x = bottom_point->x + start_point_offset_x;
        line_segment_start_point.y = bottom_point->y + partition_height * processor_index;
        line_segment_end_point.x = bottom_point->x + end_point_offset_x;
        line_segment_end_point.y = bottom_point->y + partition_height * (processor_index + 1);
        if(line_segment_end_point.y > top_point->y) line_segment_end_point.y = top_point->y;
        if(slope > 0 && line_segment_end_point.x > top_point->x) line_segment_end_point.x = top_point->x;
        if(slope < 0 && line_segment_end_point.x < top_point->x) line_segment_end_point.x = top_point->x;
        parallel_line_drawing_draw_line_thread_arguments *thread_arguments = parallel_line_drawing_get_draw_line_thread_arguments(canvas, &line_segment_start_point, &line_segment_end_point, line_drawing_algorithm);
        pthread_create(&threads[processor_index], NULL, parallel_line_drawing_draw_line_thread_wrapper, thread_arguments);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        pthread_join(threads[processor_index], NULL);
    }
}

int parallel_line_drawing_get_partition_width(line_drawing_point *start_point, line_drawing_point *end_point, int number_of_processors){
    int line_width = abs(start_point->x - end_point->x);
    int partition_width = (int)((line_width + number_of_processors - 1) / number_of_processors);
    return partition_width;
}

int parallel_line_drawing_get_partition_height(line_drawing_point *start_point, line_drawing_point *end_point, int number_of_processors){
    int line_height = abs(start_point->y - end_point->y);
    int partition_height = (int)((line_height + number_of_processors - 1) / number_of_processors);
    return partition_height;
}

void *parallel_line_drawing_draw_line_thread_wrapper(void *thread_arguments){
    parallel_line_drawing_draw_line_thread_arguments *arguments = (parallel_line_drawing_draw_line_thread_arguments *)thread_arguments;
    arguments->line_drawing_algorithm(arguments->canvas, arguments->start_point, arguments->end_point);
    return NULL;
}

parallel_line_drawing_draw_line_thread_arguments *parallel_line_drawing_get_draw_line_thread_arguments(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point)){
    parallel_line_drawing_draw_line_thread_arguments *thread_arguments = (parallel_line_drawing_draw_line_thread_arguments *)malloc(sizeof(parallel_line_drawing_draw_line_thread_arguments));
    line_drawing_point *thread_arguments_start_point = (line_drawing_point *)malloc(sizeof(line_drawing_point));
    line_drawing_point *thread_arguments_end_point = (line_drawing_point *)malloc(sizeof(line_drawing_point));
    thread_arguments_start_point->x = start_point->x;
    thread_arguments_start_point->y = start_point->y;
    thread_arguments_end_point->x = end_point->x;
    thread_arguments_end_point->y = end_point->y;
    thread_arguments->canvas = canvas;
    thread_arguments->start_point = thread_arguments_start_point;
    thread_arguments->end_point = thread_arguments_end_point;
    thread_arguments->line_drawing_algorithm = line_drawing_algorithm;
    return thread_arguments;
}