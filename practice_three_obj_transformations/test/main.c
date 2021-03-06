#include <stdio.h>
#include "obj_processor.h"
#include "ppm_processor.h"
#include "line_drawing.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex);
void draw_model(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices, ppm_processor_canvas *canvas);

int main(int argc, char *argv[])
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
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
    obj_processor_three_dimensions_model_vertex *projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
    printf("Projected vertices\n");
    printf("Ploting model\n");
    ppm_processor_canvas *canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
    draw_model(model, projected_vertices, canvas);
    printf("Printing image\n");
    ppm_processor_print_image(canvas, "obj_processor_test.ppm");
    printf("Success\n");


    obj_processor_three_dimensions_model_vertex *translation = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(translation, 0, y_translation, 0, 0);
    printf("Ploting model\n");
    obj_processor_transform_matrix *translation_matrix = obj_processor_get_translation_matrix(translation);
    obj_processor_three_dimensions_model_vertex *translated_vertices = obj_processor_apply_transform_matrix_to_model(model, translation_matrix);
    model->vertices = translated_vertices;
    projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
    canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
    draw_model(model, projected_vertices, canvas);
    printf("Printing image\n");
    ppm_processor_print_image(canvas, "obj_processor_test_translated.ppm");
    printf("Success\n");


    double rotation_angle = (1. / 2.0) * M_PI; 
    printf("Ploting model\n");
    obj_processor_transform_matrix *rotation_matrix = obj_processor_get_rotation_matrix_origin_z(rotation_angle);
    obj_processor_three_dimensions_model_vertex *rotated_vertices = obj_processor_apply_transform_matrix_to_model(model, rotation_matrix);
    model->vertices = rotated_vertices;
    projected_vertices = obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
    canvas = ppm_processor_get_canvas(canvas_width, canvas_height);
    draw_model(model, projected_vertices, canvas);
    printf("Printing image\n");
    ppm_processor_print_image(canvas, "obj_processor_test_rotated.ppm");
    printf("Success\n");
}

void draw_model(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex* projected_vertices, ppm_processor_canvas *canvas){
    int face_index, vertex_index,start_vertex_index, end_vertex_index;
    for(face_index = 0; face_index < model->faces_count; face_index++){
        obj_processor_three_dimensions_model_face *face = model->faces + face_index;
        for(vertex_index = 0; vertex_index < face->vertex_count - 1; vertex_index++){
            start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
            end_vertex_index = *(face->vertices_indexes + vertex_index + 1) - 1;
            obj_processor_three_dimensions_model_vertex* start_vertex = projected_vertices + start_vertex_index;
            obj_processor_three_dimensions_model_vertex* end_vertex = projected_vertices + end_vertex_index;
            draw_model_line(canvas, start_vertex, end_vertex);
        }
        start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
        end_vertex_index = *(face->vertices_indexes) - 1;
        obj_processor_three_dimensions_model_vertex* start_vertex = projected_vertices + start_vertex_index;
        obj_processor_three_dimensions_model_vertex* end_vertex = projected_vertices + end_vertex_index;
        draw_model_line(canvas, start_vertex, end_vertex);
    }
}

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex){
    line_drawing_point start_point, end_point;
    start_point.x = (int)((start_vertex->x * 100) + 0.5);
    start_point.y = (int)((start_vertex->y * 100) + 0.5);
    end_point.x = (int)((end_vertex->x * 100) + 0.5);
    end_point.y = (int)((end_vertex->y * 100) + 0.5);
    if(start_point.x > 499 || start_point.x < -499) return;
    if(start_point.y > 499 || start_point.y < -499) return;
    if(end_point.x > 499 || end_point.x < -499) return;
    if(end_point.y > 499 || end_point.y < -499) return;
    line_drawing_draw_line_bresenham(canvas, &start_point, &end_point);
}