#include "obj_processor.h"

obj_processor_three_dimensions_model *obj_processor_read_obj_from_file(char *file_name){
    long vertex_count = obj_processor_count_vertices(file_name);
    long faces_count = obj_processor_count_faces(file_name);
    obj_processor_three_dimensions_model *model = obj_processor_get_three_dimensions_model(vertex_count, faces_count);
    obj_processor_prase_obj_file(file_name, model);
    return model;
}

obj_processor_three_dimensions_model *obj_processor_get_three_dimensions_model(long vertex_count, long faces_count){
    obj_processor_three_dimensions_model *model = (obj_processor_three_dimensions_model *)malloc(sizeof(obj_processor_three_dimensions_model));
    obj_processor_three_dimensions_model_vertex *vertices = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex) * vertex_count);
    obj_processor_three_dimensions_model_face *faces = (obj_processor_three_dimensions_model_face *)malloc(sizeof(obj_processor_three_dimensions_model_face) * faces_count);
    model->vertex_count = vertex_count;
    model->faces_count = faces_count;
    model->vertices = vertices;
    model->faces = faces;
    return model;
}

void obj_processor_prase_obj_file(char *file_name, obj_processor_three_dimensions_model *model){
    FILE *file = fopen(file_name, "r");
    char line[100];
    char *line_readed = fgets(line, 100, file);
    int vertex_index = 0;
    int face_index = 0;
    while(line_readed){
        if(line[0] == 'v') {
            obj_processor_parse_vertex_line(line, model, vertex_index);
            vertex_index++;
        }
        if(line[0] == 'f') {
            obj_processor_parse_face_line(line, model, face_index);
            face_index++;
        }
        line_readed = fgets(line, 100, file);
    }
}

void obj_processor_parse_vertex_line(char *line, obj_processor_three_dimensions_model *model, int vertex_index){
    obj_processor_three_dimensions_model_vertex *vertex = model->vertices + vertex_index;
    sscanf(line + 2, "%f %f %f", &vertex->x, &vertex->y, &vertex->z);
}

void obj_processor_parse_face_line(char *line, obj_processor_three_dimensions_model *model, int face_index){
    obj_processor_three_dimensions_model_face *face = model->faces + face_index;
    int i, spaces_count;
    for (i = 0, spaces_count = 0; *(line + i) != '\0'; i++)
        spaces_count += *(line + i) == ' ';
    int *vertices_indexes = (int *)malloc(sizeof(int) * spaces_count);
    char* token = strtok(line + 2, " ");
    int token_index = 0;
    while (token) {
        sscanf(token, "%d", vertices_indexes + token_index);
        token = strtok(NULL, " ");
        token_index++;
    }
    face->vertex_count = spaces_count;
    face->vertices_indexes = vertices_indexes;
}

long obj_processor_count_vertices(char *file_name){
    FILE *file = fopen(file_name, "r");
    int vertex_count = 0;
    char line[100];
    char *result = fgets(line, 100, file);
    while(result != NULL){
        if(line[0] == 'v') vertex_count++;
        result = fgets(line, 100, file);
    }
    return vertex_count;
}

long obj_processor_count_faces(char *file_name){
    FILE *file = fopen(file_name, "r");
    int vertex_count = 0;
    char line[100];
    char *result = fgets(line, 100, file);
    while(result != NULL){
        if(line[0] == 'f') vertex_count++;
        result = fgets(line, 100, file);
    }
    return vertex_count;
}