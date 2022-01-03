#include <stdio.h>
#include "obj_processor.h"
#include "obj_processor_face_hidding.h"
#include "obj_processor_scan_line.h"
#include "ppm_processor.h"
#include "line_drawing.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex);
void draw_model(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices, ppm_processor_canvas *canvas);
obj_processor_three_dimensions_model_vertex *get_image_space_vertices(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices);

int main(int argc, char *argv[])
{
    int canvas_width = 1000;
    int canvas_height = 1000;
    double projection_center_z, projection_plane_z, scale, y_translation, y_scale_center;
    obj_processor_three_dimensions_model *model = obj_processor_read_obj_from_file(argv[1]);
    sscanf(argv[2], "%lf", &projection_plane_z);
    sscanf(argv[3], "%lf", &projection_center_z);
    sscanf(argv[4], "%lf", &scale);
    sscanf(argv[5], "%lf", &y_translation);
    sscanf(argv[6], "%lf", &y_scale_center);
    printf("Model loaded\n");

    int vertex_index;
    double minimum_y = model->vertices->y;
    double maximum_y = model->vertices->y;
    for(vertex_index = 0; vertex_index < model->vertex_count; vertex_index++){
        obj_processor_three_dimensions_model_vertex *vertex = model->vertices + vertex_index;
        if(minimum_y > vertex->y) minimum_y = vertex->y;
        if(maximum_y < vertex->y) maximum_y = vertex->y;
    }
    double color_factor_y = 255.0 / (maximum_y - minimum_y);
    for(vertex_index = 0; vertex_index < model->vertex_count; vertex_index++){
        obj_processor_three_dimensions_model_vertex *vertex = model->vertices + vertex_index;
        obj_processor_three_dimensions_model_vertex_attributes *vertex_attributes = model->vertices_attributes + vertex_index;
        int color_offset = (int)((vertex->y - minimum_y) * color_factor_y);
        vertex_attributes->red = 255 - color_offset;
        vertex_attributes->green = 255 - color_offset; 
        vertex_attributes->blue = 122;
    }
    printf("Color loaded\n");

    obj_processor_three_dimensions_model_vertex *scale_center = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(scale_center, -4, y_scale_center, 0, 0);
    obj_processor_three_dimensions_model_vertex *scale_details = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(scale_details, scale, scale, scale, 0);
    obj_processor_transform_matrix *scale_matrix = obj_processor_get_scale_relative_to_point_matrix(scale_center, scale_details);
    obj_processor_three_dimensions_model_vertex *scaled_vertices = obj_processor_apply_transform_matrix_to_model(model, scale_matrix);
    model->vertices = scaled_vertices;

    obj_processor_three_dimensions_model_vertex *projection_center = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(projection_center, 0, 0, projection_center_z, 0);
    obj_processor_transform_matrix *projection_matrix = obj_processor_get_two_dimensions_projection_matrix(projection_center, projection_plane_z);

/* One frame
    obj_processor_three_dimensions_model_vertex *translation = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(translation, 0, y_translation, 0, 0);
    obj_processor_transform_matrix *translation_matrix = obj_processor_get_translation_matrix(translation);
    obj_processor_three_dimensions_model_vertex *translated_vertices = obj_processor_apply_transform_matrix_to_model(model, translation_matrix);
    free(model->vertices);
    model->vertices = translated_vertices;
    
    obj_processor_three_dimensions_model_vertex *projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
    ppm_processor_canvas *canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
    draw_model(model, projected_vertices, canvas);
    ppm_processor_print_image(canvas, "image.ppm");
    free(projected_vertices);
    free(canvas->pixels);
    free(canvas);
*/
// Animation

    obj_processor_three_dimensions_model_vertex *translation = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(translation, 0, y_translation / 60, 0, 0);
    obj_processor_transform_matrix *translation_matrix = obj_processor_get_translation_matrix(translation);
    int frame_index;
    for(frame_index = 0; frame_index < 60; frame_index++){
        obj_processor_three_dimensions_model_vertex *translated_vertices = obj_processor_apply_transform_matrix_to_model(model, translation_matrix);
        free(model->vertices);
        model->vertices = translated_vertices;
        obj_processor_three_dimensions_model_vertex *projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
        ppm_processor_canvas *canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
        draw_model(model, projected_vertices, canvas);
        char file_name[100];
        sprintf(file_name, "animation_%d.ppm", frame_index);
        ppm_processor_print_image(canvas, file_name);
        printf("Frame %d de %d generado\n", frame_index + 1, 120 );
        free(projected_vertices);
        free(canvas->pixels);
        free(canvas);
    }

    double frame_translation = 20.0 / 60.0;
    obj_processor_set_three_dimensions_model_vertex(translation, frame_translation, 0, 0, 0);
    translation_matrix = obj_processor_get_translation_matrix(translation);
    double frame_rotation = (2 * M_PI) / 60;
    obj_processor_transform_matrix *rotation_matrix = obj_processor_get_rotation_matrix_origin_z(frame_rotation );
    obj_processor_transform_matrix *translation_and_rotation_matrix = obj_processor_combine_transform_matrices(translation_matrix, rotation_matrix);
    for(; frame_index < 120; frame_index++){
        obj_processor_three_dimensions_model_vertex *transformed_vertices = obj_processor_apply_transform_matrix_to_model(model, translation_and_rotation_matrix);
        free(model->vertices);
        model->vertices = transformed_vertices;
        obj_processor_three_dimensions_model_vertex *projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
        ppm_processor_canvas *canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
        draw_model(model, projected_vertices, canvas);
        char file_name[100];
        sprintf(file_name, "animation_%d.ppm", frame_index);
        ppm_processor_print_image(canvas, file_name);
        printf("Frame %d de %d generado\n", frame_index + 1, 120);
        free(projected_vertices);
        free(canvas->pixels);
        free(canvas);
    }

}

void draw_model(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices, ppm_processor_canvas *canvas){
    obj_processor_hide_back_faces(model, projected_vertices);
    obj_processor_three_dimensions_model_vertex *image_space_vertices = get_image_space_vertices(model, projected_vertices);
    ppm_processor_pixel background_pixel_template;
    ppm_processor_paint_pixel(&background_pixel_template, 0, 0, 128);
    obj_processor_paint_surfaces_scan_line_parallel(model, image_space_vertices, canvas, &background_pixel_template);
/*  int face_index, vertex_index,start_vertex_index, end_vertex_index;
    for(face_index = 0; face_index < model->faces_count; face_index++){
        obj_processor_three_dimensions_model_face *face = model->faces + face_index;
        if(face->back_face) continue;
        for(vertex_index = 0; vertex_index < face->vertex_count - 1; vertex_index++){
            start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
            end_vertex_index = *(face->vertices_indexes + vertex_index + 1) - 1;
            obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
            obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
            draw_model_line(canvas, start_vertex, end_vertex);
        }
        start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
        end_vertex_index = *(face->vertices_indexes) - 1;
        obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
        obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
        draw_model_line(canvas, start_vertex, end_vertex);
    }*/
    free(image_space_vertices);
}

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex){
    line_drawing_point start_point, end_point;
    if(start_vertex->x > 499 || start_vertex->x < -499) return;
    if(start_vertex->y > 499 || start_vertex->y < -499) return;
    if(end_vertex->x > 499 || end_vertex->x < -499) return;
    if(end_vertex->y > 499 || end_vertex->y < -499) return;
    start_point.x = start_vertex->x;
    start_point.y = start_vertex->y;
    end_point.x = end_vertex->x;
    end_point.y = end_vertex->y;
    line_drawing_draw_line_bresenham(canvas, &start_point, &end_point);
}

obj_processor_three_dimensions_model_vertex *get_image_space_vertices(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices){
    obj_processor_three_dimensions_model_vertex *image_space_vertices = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex) * model->vertex_count);
    int vertex_index;
    for(vertex_index = 0; vertex_index < model->vertex_count; vertex_index++){
        obj_processor_three_dimensions_model_vertex *projected_vertex = projected_vertices + vertex_index;
        obj_processor_three_dimensions_model_vertex *image_space_vertex = image_space_vertices + vertex_index;
        image_space_vertex->x = (int)((projected_vertex->x * 100) + 0.5);
        image_space_vertex->y = (int)((projected_vertex->y * 100) + 0.5);
    }
    return image_space_vertices;
}
