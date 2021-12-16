#include <stdio.h>
#include "obj_processor.h"

int main(int argc, char *argv[])
{
    obj_processor_three_dimensions_model *model = obj_processor_read_obj_from_file("test/cube_solid_triangle_faces.obj");
    printf("Vertices: %d, Faces: %d\n", model->vertex_count, model->faces_count);
    int i;
    for(i = 0; i < model->vertex_count; i++){
        obj_processor_three_dimensions_model_vertex* vertex = model->vertices + i;
        printf("%f %f %f\n", vertex->x, vertex->y, vertex->z);
    }
    for(i = 0; i < model->faces_count; i++){
        obj_processor_three_dimensions_model_face* face = model->faces + i;
        int j; 
        for(j = 0; j < face->vertex_count - 1; j++){
            printf("%d ", *(face->vertices_indexes + j));
        }
        printf("%d\n", *(face->vertices_indexes + j));
    }
}