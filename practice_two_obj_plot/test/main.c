#include <stdio.h>
#include "obj_processor.h"
#include "ppm_processor.h"
#include "line_drawing.h"

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex, double scale);

int main(int argc, char *argv[])
{
    double projection_center_z, projection_plane_z, scale;
    obj_processor_three_dimensions_model *model = obj_processor_read_obj_from_file(argv[1]);
    sscanf(argv[2], "%lf", &projection_plane_z);
    sscanf(argv[3], "%lf", &projection_center_z);
    sscanf(argv[4], "%lf", &scale);
    printf("Model loaded\n");
    obj_processor_three_dimensions_model_vertex *projection_center = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_set_three_dimensions_model_vertex(projection_center, 0, 0, projection_center_z, 0);
    obj_processor_three_dimensions_model_vertex *projected_vertices = obj_processor_get_two_dimensions_projection(model, projection_center, projection_plane_z);
    printf("Projected vertices\n");
    int vertex_index;
    printf("Ploting model\n");
    ppm_processor_canvas *canvas = ppm_processor_get_canvas(4000, 4000);
    int face_index, start_vertex_index, end_vertex_index;
    for(face_index = 0; face_index < model->faces_count; face_index++){
        obj_processor_three_dimensions_model_face *face = model->faces + face_index;
        for(vertex_index = 0; vertex_index < face->vertex_count - 1; vertex_index++){
            start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
            end_vertex_index = *(face->vertices_indexes + vertex_index + 1) - 1;
            obj_processor_three_dimensions_model_vertex* start_vertex = projected_vertices + start_vertex_index;
            obj_processor_three_dimensions_model_vertex* end_vertex = projected_vertices + end_vertex_index;
            draw_model_line(canvas, start_vertex, end_vertex, scale);
        }
        start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
        end_vertex_index = *(face->vertices_indexes) - 1;
        obj_processor_three_dimensions_model_vertex* start_vertex = projected_vertices + start_vertex_index;
        obj_processor_three_dimensions_model_vertex* end_vertex = projected_vertices + end_vertex_index;
        draw_model_line(canvas, start_vertex, end_vertex, scale);
    }
    printf("Printing image\n");
    ppm_processor_print_image(canvas, "obj_processor_test.ppm");
    printf("Success\n");
}

void draw_model_line(ppm_processor_canvas *canvas, obj_processor_three_dimensions_model_vertex* start_vertex, obj_processor_three_dimensions_model_vertex* end_vertex, double scale){
    line_drawing_point start_point, end_point;
    start_point.x = (int)(start_vertex->x * scale + 0.5);
    start_point.y = (int)(start_vertex->y * scale + 0.5);
    end_point.x = (int)(end_vertex->x * scale + 0.5);
    end_point.y = (int)(end_vertex->y * scale + 0.5);
    line_drawing_draw_line_bresenham(canvas, &start_point, &end_point);
}