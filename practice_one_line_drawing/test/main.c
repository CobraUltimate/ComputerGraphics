#include <stdio.h>
#include "ppm_processor.h"
#include "line_drawing.h"
#include "parallel_line_drawing.h"

void ppm_processor_test();
void line_drawing_test();
int paint_it_black(int width_index, int height_index);

int main(int argc, char *argv[])
{
    ppm_processor_canvas *canvas = ppm_processor_get_canvas(500, 500);
    ppm_processor_test(canvas);
    ppm_processor_canvas *line_drawing_naive_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(line_drawing_naive_canvas, line_drawing_draw_line_naive);
    ppm_processor_print_image(line_drawing_naive_canvas, "line_drawing_naive_test.ppm");
    ppm_processor_canvas *line_drawing_dda_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(line_drawing_dda_canvas, line_drawing_draw_line_dda);
    ppm_processor_print_image(line_drawing_dda_canvas, "line_drawing_dda_test.ppm");
    ppm_processor_canvas *line_drawing_bresenham_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(line_drawing_bresenham_canvas, line_drawing_draw_line_bresenham);
    ppm_processor_print_image(line_drawing_bresenham_canvas, "line_drawing_bresenham_test.ppm");
    ppm_processor_canvas *parallel_line_drawing_bresenham_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(parallel_line_drawing_bresenham_canvas, parallel_line_drawing_draw_line_bresenham);
    ppm_processor_print_image(parallel_line_drawing_bresenham_canvas, "parallel_line_drawing_bresenham_test.ppm");
    ppm_processor_canvas *parallel_line_drawing_dda_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(parallel_line_drawing_dda_canvas, parallel_line_drawing_draw_line_dda);
    ppm_processor_print_image(parallel_line_drawing_dda_canvas, "parallel_line_drawing_dda_test.ppm");
    ppm_processor_canvas *parallel_line_drawing_naive_canvas = ppm_processor_get_canvas(500, 500);
    line_drawing_test(parallel_line_drawing_naive_canvas, parallel_line_drawing_draw_line_naive);
    ppm_processor_print_image(parallel_line_drawing_naive_canvas, "parallel_line_drawing_naive_test.ppm");
    return 0;
}

void ppm_processor_test(ppm_processor_canvas *canvas){
    int height_index, width_index;
    for(height_index = 0; height_index < canvas->height; height_index++){
        for(width_index = 0; width_index < canvas->width; width_index++){
            if(paint_it_black(width_index, height_index)){
                ppm_processor_pixel *pixel = ppm_processor_get_pixel_absolute(canvas, width_index, height_index);
                pixel->red = 0;
                pixel->green = 0;
                pixel->blue = 0;
            }
        }
    }
    ppm_processor_print_image(canvas, "ppm_processor_test.ppm");
}

int paint_it_black(int width_index, int height_index){
    if (width_index < 50 || width_index >= 450) return 1;
    if (height_index < 50 || height_index >= 450) return 1;
    return 0;
}

void line_drawing_test(ppm_processor_canvas *canvas, void (*line_drawing_algorithm)(ppm_processor_canvas *canvas, line_drawing_point *start_point, line_drawing_point *end_point)){
    line_drawing_point start_point, end_point;
    start_point.x = -200;
    start_point.y = -200;
    end_point.x = 200;
    end_point.y = 200;
    line_drawing_algorithm(canvas, &start_point, &end_point);
    start_point.x = -200;
    start_point.y = 200;
    end_point.x = 200;
    end_point.y = -200;
    line_drawing_algorithm(canvas, &start_point, &end_point);
    start_point.x = -200;
    start_point.y = 0;
    end_point.x = 200;
    end_point.y = 0;
    line_drawing_algorithm(canvas, &start_point, &end_point);
    start_point.x = 0;
    start_point.y = -200;
    end_point.x = 0;
    end_point.y = 200;
    line_drawing_algorithm(canvas, &start_point, &end_point);
    start_point.x = -200;
    start_point.y = -200;
    end_point.x = 0;
    end_point.y = 200;
    line_drawing_algorithm(canvas, &start_point, &end_point);
    start_point.x = 200;
    start_point.y = -200;
    end_point.x = -200;
    end_point.y = 0;
    line_drawing_algorithm(canvas, &start_point, &end_point);
}