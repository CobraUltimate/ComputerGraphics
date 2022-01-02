#include "obj_processor_face_hidding.h"

void obj_processor_hide_back_faces(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *projected_vertices){
    int face_index;
    for(face_index = 0; face_index < model->faces_count; face_index++){
        obj_processor_three_dimensions_model_face *face = model->faces + face_index;
        obj_processor_three_dimensions_model_vertex *normal_vector = obj_processor_get_normal_vector(face, projected_vertices);
        if(normal_vector->z <= 0) face->back_face = 0;
        free(normal_vector);
    }
}

obj_processor_three_dimensions_model_vertex *obj_processor_get_normal_vector(obj_processor_three_dimensions_model_face *face, obj_processor_three_dimensions_model_vertex *projected_vertices){
    obj_processor_three_dimensions_model_vertex* a_vertex = projected_vertices +  *face->vertices_indexes;
    obj_processor_three_dimensions_model_vertex* b_vertex = projected_vertices + *(face->vertices_indexes + 1);
    obj_processor_three_dimensions_model_vertex* c_vertex = projected_vertices + *(face->vertices_indexes + 2);
    obj_processor_three_dimensions_model_vertex* ab_side = obj_processor_vector_substraction(b_vertex, a_vertex);
    obj_processor_three_dimensions_model_vertex* ac_side = obj_processor_vector_substraction(c_vertex, a_vertex);
    free(ab_side);
    free(ac_side);
    return obj_processor_cross_product(ab_side, ac_side);
}

obj_processor_three_dimensions_model_vertex *obj_processor_vector_substraction(obj_processor_three_dimensions_model_vertex *minuend, obj_processor_three_dimensions_model_vertex *substranting){
    obj_processor_three_dimensions_model_vertex *difference = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    difference->x = minuend->x - substranting->x;
    difference->y = minuend->y - substranting->y;
    difference->z = minuend->z - substranting->z;
    return difference;
}

obj_processor_three_dimensions_model_vertex *obj_processor_cross_product(obj_processor_three_dimensions_model_vertex *vector_a, obj_processor_three_dimensions_model_vertex *vector_b){
    obj_processor_three_dimensions_model_vertex *product = (obj_processor_three_dimensions_model_vertex *)malloc(sizeof(obj_processor_three_dimensions_model_vertex));
    product->x = (vector_a->y * vector_b->z) - (vector_a->z * vector_b->y);
    product->y = (vector_a->z * vector_b->x) - (vector_a->x * vector_b->z);
    product->z = (vector_a->x * vector_b->y) - (vector_a->y * vector_b->x);
    return product;
}

