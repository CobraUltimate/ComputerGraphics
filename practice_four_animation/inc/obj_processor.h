#ifndef OBJ_PROCESSOR_H
#define OBJ_PROCESSOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef double obj_processor_transform_matrix;

typedef struct obj_processor_three_dimensions_model_vertex{
    double x;
    double y;
    double z;
    double w;
} obj_processor_three_dimensions_model_vertex;

typedef struct obj_processor_three_dimensions_model_face{
    int vertex_count;
    int *vertices_indexes;
} obj_processor_three_dimensions_model_face;

typedef struct obj_processor_three_dimensions_model{
    int vertex_count;
    int faces_count;
    obj_processor_three_dimensions_model_vertex *vertices;
    obj_processor_three_dimensions_model_face *faces;
} obj_processor_three_dimensions_model;

obj_processor_three_dimensions_model *obj_processor_read_obj_from_file(char *file_name);
char *obj_processor_load_obj_file_to_memory(char *file_name, long *file_lenght);
obj_processor_three_dimensions_model *obj_processor_get_three_dimensions_model(long vertex_count, long faces_count);
void obj_processor_set_three_dimensions_model_vertex(obj_processor_three_dimensions_model_vertex *vertex, double x, double y, double z, double w);
void obj_processor_parse_obj_file(char *memory_file, long file_lenght, obj_processor_three_dimensions_model *model, long vertex_count, long faces_count);
void obj_processor_parse_vertex_line(char *line, obj_processor_three_dimensions_model *model, int vertex_index);
void obj_processor_parse_face_line(char *line, obj_processor_three_dimensions_model *model, int face_index);
void obj_processor_count_faces_and_vertices(char *memory_file, long file_lenght, long *vertex_count, long *faces_count);
obj_processor_three_dimensions_model_vertex *obj_processor_get_two_dimensions_projection(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *projection_center, double projection_plane_z);
obj_processor_transform_matrix *obj_processor_create_transform_matrix();
void obj_processor_set_transform_matrix_cell(obj_processor_transform_matrix *transform_matrix, int row, int column, double value);
double obj_processor_get_transform_matrix_cell(obj_processor_transform_matrix *transform_matrix, int row, int column);
void obj_processor_set_transform_matrix_row(obj_processor_transform_matrix *transform_matrix, int row, double value_one, double value_two, double value_three, double value_four);
obj_processor_three_dimensions_model_vertex *obj_processor_apply_transform_matrix_to_vertex(obj_processor_three_dimensions_model_vertex *vertex, obj_processor_transform_matrix *transform_matrix);
obj_processor_three_dimensions_model_vertex *obj_processor_apply_transform_matrix_to_model(obj_processor_three_dimensions_model *model, obj_processor_transform_matrix *transform_matrix);
double obj_processor_apply_transform_matrix_calculate_column(obj_processor_three_dimensions_model_vertex *vertex, obj_processor_transform_matrix *transform_matrix, int column);
obj_processor_transform_matrix *obj_processor_get_two_dimensions_projection_matrix(obj_processor_three_dimensions_model_vertex *projection_center, double projection_plane_z);
obj_processor_transform_matrix *obj_processor_get_translation_matrix(obj_processor_three_dimensions_model_vertex *translation);
obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_x(double angle);
obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_y(double angle);
obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_z(double angle);
obj_processor_transform_matrix *obj_processor_get_scale_relative_to_point_matrix(obj_processor_three_dimensions_model_vertex *scale_center, obj_processor_three_dimensions_model_vertex *scale);
obj_processor_transform_matrix *obj_processor_combine_transform_matrices(obj_processor_transform_matrix *left_matrix, obj_processor_transform_matrix *right_matrix);

#endif