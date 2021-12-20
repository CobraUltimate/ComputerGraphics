#include "obj_processor.h"

obj_processor_three_dimensions_model *obj_processor_read_obj_from_file(char *file_name){
    long vertex_count, faces_count, file_lenght;
    char *memory_file = obj_processor_load_obj_file_to_memory(file_name, &file_lenght);
    obj_processor_count_faces_and_vertices(memory_file, file_lenght, &vertex_count, &faces_count);
    obj_processor_three_dimensions_model *model = obj_processor_get_three_dimensions_model(vertex_count, faces_count);
    obj_processor_parse_obj_file(memory_file, file_lenght, model, vertex_count, faces_count);
    free(memory_file);
    return model;
}

char *obj_processor_load_obj_file_to_memory(char *file_name, long *file_lenght){
    FILE *fileptr;
    char *buffer;
    fileptr = fopen(file_name, "rb");
    fseek(fileptr, 0, SEEK_END);
    *file_lenght = ftell(fileptr);
    rewind(fileptr);
    buffer = (char *)malloc(*file_lenght * sizeof(char));
    fread(buffer, *file_lenght, 1, fileptr);
    fclose(fileptr);
    return buffer;
}

obj_processor_three_dimensions_model *obj_processor_get_three_dimensions_model(long vertex_count, long faces_count){
    obj_processor_three_dimensions_model *model = (obj_processor_three_dimensions_model *)malloc(sizeof(obj_processor_three_dimensions_model));
    obj_processor_three_dimensions_model_vertex *vertices = (obj_processor_three_dimensions_model_vertex *)calloc(vertex_count, sizeof(obj_processor_three_dimensions_model_vertex));
    obj_processor_three_dimensions_model_face *faces = (obj_processor_three_dimensions_model_face *)calloc(faces_count, sizeof(obj_processor_three_dimensions_model_face));
    model->vertex_count = vertex_count;
    model->faces_count = faces_count;
    model->vertices = vertices;
    model->faces = faces;
    return model;
}

void obj_processor_set_three_dimensions_model_vertex(obj_processor_three_dimensions_model_vertex *vertex, double x, double y, double z, double w){
    vertex->x = x;
    vertex->y = y;
    vertex->z = z;
    vertex->w = w;
}

void obj_processor_parse_obj_file(char *memory_file, long file_lenght, obj_processor_three_dimensions_model *model, long vertex_count, long faces_count){
    FILE *file = fmemopen(memory_file, file_lenght, "r");
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
    sscanf(line + 2, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);
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

void obj_processor_count_faces_and_vertices(char *memory_file, long file_lenght, long *vertex_count, long *faces_count){
    FILE *file = fmemopen(memory_file, file_lenght, "r");
    *vertex_count = 0;
    *faces_count = 0;
    char line[100];
    char *result = fgets(line, 100, file);
    while(result != NULL){
        if(line[0] == 'v') *vertex_count += 1;
        else if(line[0] == 'f') *faces_count += 1;
        result = fgets(line, 100, file);
    }
}

obj_processor_three_dimensions_model_vertex *obj_processor_get_two_dimensions_projection(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *projection_center, double projection_plane_z){
    obj_processor_transform_matrix *projection_matrix = obj_processor_get_two_dimensions_projection_matrix(projection_center, projection_plane_z);
    return obj_processor_apply_transform_matrix_to_model(model, projection_matrix);
}

obj_processor_transform_matrix *obj_processor_create_transform_matrix(){
    return (obj_processor_transform_matrix *)malloc(sizeof(obj_processor_transform_matrix) * 16);
}

void obj_processor_set_transform_matrix_cell(obj_processor_transform_matrix *transform_matrix, int row, int column, double value){
    *((transform_matrix + row * 4) + column) = value;
}

double obj_processor_get_transform_matrix_cell(obj_processor_transform_matrix *transform_matrix, int row, int column){
    return *((transform_matrix + row * 4) + column);
}

void obj_processor_set_transform_matrix_row(obj_processor_transform_matrix *transform_matrix, int row, double value_one, double value_two, double value_three, double value_four){
    *(transform_matrix + row * 4) = value_one;
    *((transform_matrix + row * 4) + 1) = value_two;
    *((transform_matrix + row * 4) + 2) = value_three;
    *((transform_matrix + row * 4) + 3) = value_four;
}

obj_processor_three_dimensions_model_vertex *obj_processor_apply_transform_matrix_to_vertex(obj_processor_three_dimensions_model_vertex *vertex, obj_processor_transform_matrix *transform_matrix){
    obj_processor_three_dimensions_model_vertex *transformed_vertex = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    vertex->w = 1;
    transformed_vertex->x = obj_processor_apply_transform_matrix_calculate_column(vertex, transform_matrix, 0);
    transformed_vertex->y = obj_processor_apply_transform_matrix_calculate_column(vertex, transform_matrix, 1);
    transformed_vertex->z = obj_processor_apply_transform_matrix_calculate_column(vertex, transform_matrix, 2);
    transformed_vertex->w = obj_processor_apply_transform_matrix_calculate_column(vertex, transform_matrix, 3);
    transformed_vertex->x /= transformed_vertex->w;
    transformed_vertex->y /= transformed_vertex->w;
    transformed_vertex->z /= transformed_vertex->w;
    transformed_vertex->w /= transformed_vertex->w;
    return transformed_vertex;
}

obj_processor_three_dimensions_model_vertex *obj_processor_apply_transform_matrix_to_model(obj_processor_three_dimensions_model *model, obj_processor_transform_matrix *transform_matrix){
    obj_processor_three_dimensions_model_vertex *projected_vertices = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex) * model->vertex_count);
    int vertex_index;
    for(vertex_index = 0; vertex_index < model->vertex_count; vertex_index++){
        obj_processor_three_dimensions_model_vertex* vertex = model->vertices + vertex_index;
        obj_processor_three_dimensions_model_vertex *projected_vertex = obj_processor_apply_transform_matrix_to_vertex(vertex, transform_matrix);
        obj_processor_set_three_dimensions_model_vertex(projected_vertices + vertex_index, projected_vertex->x, projected_vertex->y, projected_vertex->z, projected_vertex->w);
        free(projected_vertex);
    }
    return projected_vertices;
}

double obj_processor_apply_transform_matrix_calculate_column(obj_processor_three_dimensions_model_vertex *vertex, obj_processor_transform_matrix *transform_matrix, int column){
    double column_value = 0;
    column_value += vertex->x * obj_processor_get_transform_matrix_cell(transform_matrix, 0, column);
    column_value += vertex->y * obj_processor_get_transform_matrix_cell(transform_matrix, 1, column);
    column_value += vertex->z * obj_processor_get_transform_matrix_cell(transform_matrix, 2, column);
    column_value += vertex->w * obj_processor_get_transform_matrix_cell(transform_matrix, 3, column);
    return column_value;
}

obj_processor_transform_matrix *obj_processor_get_two_dimensions_projection_matrix(obj_processor_three_dimensions_model_vertex *projection_center, double projection_plane_z){
    double d = projection_center->z - projection_plane_z;
    obj_processor_transform_matrix *projection_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(projection_matrix, 0, 1, 0, 0, 0);
    obj_processor_set_transform_matrix_row(projection_matrix, 1, 0, 1, 0, 0);
    obj_processor_set_transform_matrix_row(projection_matrix, 2, 0, 0, (projection_plane_z * -1) / d, -1 / d);
    obj_processor_set_transform_matrix_row(projection_matrix, 3, 0, 0, projection_plane_z * (projection_center->z / d), projection_center->z / d);
    return projection_matrix;
}

obj_processor_transform_matrix *obj_processor_get_translation_matrix(obj_processor_three_dimensions_model_vertex *translation){
    obj_processor_transform_matrix *translation_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(translation_matrix, 0, 1, 0, 0, 0);
    obj_processor_set_transform_matrix_row(translation_matrix, 1, 0, 1, 0, 0);
    obj_processor_set_transform_matrix_row(translation_matrix, 2, 0, 0, 1, 0);
    obj_processor_set_transform_matrix_row(translation_matrix, 3, translation->x, translation->y, translation->z, 1);
    return translation_matrix;
}

obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_x(double angle){
    obj_processor_transform_matrix *rotation_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(rotation_matrix, 0, 1, 0, 0, 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 1, 0, cos(angle), sin(angle), 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 2, 0, -sin(angle), cos(angle), 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 3, 0, 0, 0, 1);
    return rotation_matrix;
}

obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_y(double angle){
    obj_processor_transform_matrix *rotation_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(rotation_matrix, 0, cos(angle), 0, -sin(angle), 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 1, 0, 1, 0, 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 2, sin(angle), 0, 1, cos(angle));
    obj_processor_set_transform_matrix_row(rotation_matrix, 3, 0, 0, 0 , 1);
    return rotation_matrix;
}

obj_processor_transform_matrix *obj_processor_get_rotation_matrix_origin_z(double angle){
    obj_processor_transform_matrix *rotation_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(rotation_matrix, 0, cos(angle), sin(angle), 0, 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 1, -sin(angle), cos(angle), 0, 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 2, 0, 0, 1, 0);
    obj_processor_set_transform_matrix_row(rotation_matrix, 3, 0, 0, 0, 1);
    return rotation_matrix;
}

obj_processor_transform_matrix *obj_processor_get_scale_relative_to_point_matrix(obj_processor_three_dimensions_model_vertex *scale_center, obj_processor_three_dimensions_model_vertex *scale){
    obj_processor_three_dimensions_model_vertex *scale_center_negative = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    scale_center_negative->x = scale_center->x * -1;
    scale_center_negative->y = scale_center->y * -1;
    scale_center_negative->z = scale_center->z * -1;
    scale_center_negative->w = 1;
    obj_processor_transform_matrix *translation_matrix = obj_processor_get_translation_matrix(scale_center_negative);
    obj_processor_transform_matrix *scale_matrix = obj_processor_create_transform_matrix();
    obj_processor_set_transform_matrix_row(scale_matrix, 0, scale->x, 0, 0, 0);
    obj_processor_set_transform_matrix_row(scale_matrix, 1, 0, scale->y, 0, 0);
    obj_processor_set_transform_matrix_row(scale_matrix, 2, 0, 0, scale->z, 0);
    obj_processor_set_transform_matrix_row(scale_matrix, 3, 0, 0, 0, 1);
    obj_processor_transform_matrix *return_translation_matrix = obj_processor_get_translation_matrix(scale_center);
    obj_processor_transform_matrix *center_scale_matrix = obj_processor_combine_transform_matrices(translation_matrix, scale_matrix);
    obj_processor_transform_matrix *scale_relative_to_point_matrix = obj_processor_combine_transform_matrices(center_scale_matrix, return_translation_matrix);
    free(scale_center_negative);
    free(translation_matrix);
    free(scale_matrix);
    free(return_translation_matrix);
    free(center_scale_matrix);
    return scale_relative_to_point_matrix;
}

obj_processor_transform_matrix *obj_processor_combine_transform_matrices(obj_processor_transform_matrix *left_matrix, obj_processor_transform_matrix *right_matrix){
    obj_processor_transform_matrix *combined_matrix = obj_processor_create_transform_matrix();
    int outer_index, innner_index, multiplication_index;
    for(outer_index = 0; outer_index < 4; outer_index++){
        for(innner_index = 0; innner_index < 4; innner_index++){
            double cell_value = 0;
            for(multiplication_index = 0; multiplication_index < 4; multiplication_index++){
                double left_matrix_value = obj_processor_get_transform_matrix_cell(left_matrix, outer_index, multiplication_index);
                double right_matrix_value = obj_processor_get_transform_matrix_cell(right_matrix, multiplication_index, innner_index);
                cell_value += left_matrix_value * right_matrix_value;
            }
            obj_processor_set_transform_matrix_cell(combined_matrix, outer_index, innner_index, cell_value);
        }
    }
    return combined_matrix;
}