#ifndef OBJ_PROCESSOR_SCAN_LINE_H
#define OBJ_PROCESSOR_SCAN_LINE_H

#include "obj_processor.h"
#include "ppm_processor.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

typedef struct obj_processor_scan_line_side obj_processor_scan_line_side;

typedef struct obj_processor_scan_line_surface{
    obj_processor_three_dimensions_model_face *face;
    obj_processor_scan_line_side *sides;
    int surface_on;
    int surface_on_x;
    int vertex_on;
    int has_intersected_the_scan_line;
} obj_processor_scan_line_surface;

typedef struct obj_processor_scan_line_side{
    obj_processor_three_dimensions_model_vertex *start_point;
    obj_processor_three_dimensions_model_vertex *end_point;
    obj_processor_scan_line_surface *surface;
    int scan_line_intersection_x;
} obj_processor_scan_line_side;

typedef struct obj_processor_paint_surfaces_scan_line_parallel_worker_arguments{
    obj_processor_three_dimensions_model *model;
    obj_processor_three_dimensions_model_vertex *image_space_vertices;
    ppm_processor_canvas *canvas;
    ppm_processor_pixel *background_pixel_template;
    int scan_line_start_y;
    int scan_line_finish_y;
} obj_processor_paint_surfaces_scan_line_parallel_worker_arguments;

#include "obj_processor_scan_line.h"

void obj_processor_paint_surfaces_scan_line_parallel(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template);
void *obj_processor_paint_surfaces_scan_line_parallel_worker(void *arguments);
obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *obj_processor_get_paint_surfaces_scan_line_parallel_worker_arguments(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template, int scan_line_start_y, int scan_line_finish_y);
void obj_processor_paint_surfaces_scan_line(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template);
void obj_processor_scan_line_get_scan_line_active_surfaces_and_sides(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, int scan_line_index_y, int *active_sides_count, int *active_surfaces_count, obj_processor_scan_line_side **active_sides, obj_processor_scan_line_surface **active_surfaces);
int obj_processor_scan_line_is_surface_active(obj_processor_three_dimensions_model_face *face, int scan_line_index_y, obj_processor_three_dimensions_model_vertex *image_space_vertices);
int obj_processor_scan_line_is_side_crossing_scan_line(obj_processor_scan_line_side *side, int scan_line_index_y);
void obj_processor_map_face_to_surface(obj_processor_three_dimensions_model_face *face, obj_processor_scan_line_surface *surface, obj_processor_three_dimensions_model_vertex *image_space_vertices);
void obj_processor_scan_line_sort_active_sides(obj_processor_scan_line_side *active_sides, int active_sides_count);
void obj_processor_scan_line_copy_side(obj_processor_scan_line_side *target, obj_processor_scan_line_side *source);
obj_processor_scan_line_surface *obj_processor_scan_line_get_surface_to_paint(obj_processor_scan_line_surface *active_surfaces, int active_surfaces_count);
ppm_processor_pixel *obj_processor_get_pixel_paint_color(obj_processor_scan_line_surface *paint_surface, obj_processor_three_dimensions_model_vertex *image_space_vertices, obj_processor_three_dimensions_model_vertex_attributes *vertices_attributes);
void obj_processor_scan_line_finish_scan_line_processing(obj_processor_scan_line_surface *active_surfaces, int active_surfaces_count);

#endif