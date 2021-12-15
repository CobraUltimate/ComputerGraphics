#include "line_drawing.h"

void line_drawing_draw_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    double my = (double)(end_point->y - start_point->y); 
    double mx = (double)(end_point->x - start_point->x);
    if(mx == 0) {
        line_drawing_draw_vertical_line_naive(canvas, start_point, end_point);
        return;
    }
    double m = my/mx;
    double b = start_point->y - (m * start_point->x);
    int x;
    line_drawing_point *left_point = line_drawing_get_left_point(start_point, end_point);
    line_drawing_point *right_point = line_drawing_get_right_point(start_point, end_point);
    for(x = left_point->x; x <= right_point->x; x++){
        int y = m * x + b;
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

void line_drawing_draw_vertical_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    line_drawing_point *bottom_point = line_drawing_get_bottom_point(start_point, end_point);
    line_drawing_point *top_point = line_drawing_get_top_point(start_point, end_point);
    int y;
    for(y = bottom_point->y; y <= top_point->y; y++){
        int x = bottom_point->x;
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

void line_drawing_draw_horizontal_line_naive(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    line_drawing_point *left_point = line_drawing_get_left_point(start_point, end_point);
    line_drawing_point *right_point = line_drawing_get_right_point(start_point, end_point);
    int x;
    for(x = left_point->x; x <= right_point->x; x++){
        int y = left_point->y;
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

void line_drawing_draw_line_dda(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    float delta_x = end_point->x - start_point->x;
    float delta_y = end_point->y - start_point->y;
    float steps;
    if(abs(delta_x) > abs(delta_y)) steps = abs(delta_x);
    else steps = abs(delta_y);
    double increment_x = delta_x / steps;
    double increment_y = delta_y / steps;
    float x = start_point->x;
    float y = start_point->y;
    int step;
    for(step = 0; step < steps; step++){
        x += increment_x;
        y += increment_y;
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, (int)(x + 0.5), (int)(y + 0.5));
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

void line_drawing_draw_line_bresenham(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    int slope_x = end_point->x - start_point->x;
    if(slope_x == 0){
        line_drawing_draw_vertical_line_naive(canvas, start_point, end_point);
        return;
    }
    int slope_y = end_point->y - start_point->y;
    if(slope_y == 0){
        line_drawing_draw_horizontal_line_naive(canvas, start_point, end_point);
        return;
    }
    double slope = slope_y / slope_x;
    if(slope >= 1 || slope <= -1){
        line_drawing_draw_line_bresenham_independent_y(canvas, start_point, end_point);
        return;
    }
    line_drawing_draw_line_bresenham_independent_x(canvas, start_point, end_point);
}

void line_drawing_draw_line_bresenham_independent_x(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    int delta_x = abs(start_point->x - end_point->x);
    int delta_y = abs(start_point->y - end_point->y);
    int p = 2 * delta_y - delta_x;
    int two_delta_y = 2 * delta_y;
    int two_difference_delta_y_delta_x = 2 * (delta_y - delta_x);
    int x,y,end_x,end_y,increment_y;
    if(start_point->x > end_point->x){
        x = end_point->x;
        y = end_point->y;
        end_x = start_point->x;
        end_y = start_point->y;
    }
    else{
        x = start_point->x;
        y = start_point->y;
        end_x = end_point->x;
        end_y = end_point->y;
    }
    if(y < end_y) increment_y = 1;
    else increment_y = -1;
    ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
    ppm_processor_paint_pixel(pixel, 0, 0, 0);
    while(x < end_x){
        x++;
        if(p < 0){
            p += two_delta_y;
        }
        else{
            y += increment_y;
            p += two_difference_delta_y_delta_x;
        }
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

void line_drawing_draw_line_bresenham_independent_y(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point){
    int delta_x = abs(start_point->x - end_point->x);
    int delta_y = abs(start_point->y - end_point->y);
    int p = 2 * delta_x - delta_y;
    int two_delta_x = 2 * delta_x;
    int two_difference_delta_x_delta_y = 2 * (delta_x - delta_y);
    int x,y,end_y,end_x,increment_x;
    if(start_point->y > end_point->y){
        x = end_point->x;
        y = end_point->y;
        end_y = start_point->y;
        end_x = start_point->x;
    }
    else{
        x = start_point->x;
        y = start_point->y;
        end_y = end_point->y;
        end_x = end_point->x;
    }
    if(x < end_x) increment_x = 1;
    else increment_x = -1;
    ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
    ppm_processor_paint_pixel(pixel, 0, 0, 0);
    while(y < end_y){
        y++;
        if(p < 0){
            p += two_delta_x;
        }
        else{
            x += increment_x;
            p += two_difference_delta_x_delta_y;
        }
        ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, x, y);
        ppm_processor_paint_pixel(pixel, 0, 0, 0);
    }
}

line_drawing_point *line_drawing_get_left_point(line_drawing_point *point_one, line_drawing_point *point_two){
    if(point_two->x >= point_one->x) return point_one;
    else return point_two;
}

line_drawing_point *line_drawing_get_right_point(line_drawing_point *point_one, line_drawing_point *point_two){
    if(point_two->x >= point_one->x) return point_two;
    else return point_one;
}

line_drawing_point *line_drawing_get_bottom_point(line_drawing_point *point_one, line_drawing_point *point_two){
    if(point_two->y >= point_one->y) return point_one;
    else return point_two;
}

line_drawing_point *line_drawing_get_top_point(line_drawing_point *point_one, line_drawing_point *point_two){
    if(point_two->y >= point_one->y) return point_two;
    else return point_one;
}
