#ifndef OBJ_PROCESSOR_H
#define OBJ_PROCESSOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct obj_processor_three_dimensions_model_vertex{
    float x;
    float y;
    float z;
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
obj_processor_three_dimensions_model *obj_processor_get_three_dimensions_model(long vertex_count, long faces_count);
void obj_processor_prase_obj_file(char *file_name, obj_processor_three_dimensions_model *model);
void obj_processor_parse_vertex_line(char *line, obj_processor_three_dimensions_model *model, int vertex_index);
void obj_processor_parse_face_line(char *line, obj_processor_three_dimensions_model *model, int face_index);
long obj_processor_count_vertices(char *file_name);
long obj_processor_count_faces(char *file_name);
obj_processor_three_dimensions_model_vertex * obj_processor_get_two_dimen

#endif