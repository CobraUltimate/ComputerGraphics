#include "obj_processor_scan_line.h"

void obj_processor_paint_surfaces_scan_line_parallel(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template){
    int number_of_processors = (int)sysconf(_SC_NPROCESSORS_ONLN);
    int processor_index;
    pthread_t threads[number_of_processors];
    obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *arguments[number_of_processors];
    int scan_line_worker_height = (int)((double)canvas->height - 2.0) / (double)number_of_processors;
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        //printf("thread started: %d\n", processor_index);
        int scan_line_start_y = (canvas->height / 2 * -1) + (scan_line_worker_height * processor_index);
        int scan_line_end_y = (canvas->height / 2 * -1) + (scan_line_worker_height * (processor_index + 1));
        obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *thread_arguments = obj_processor_get_paint_surfaces_scan_line_parallel_worker_arguments(model, image_space_vertices, canvas, background_pixel_template, scan_line_start_y, scan_line_end_y);
        arguments[processor_index] = thread_arguments;
        //printf("procesando de la linea de escaneo %d a %d\n", thread_arguments->scan_line_start_y, thread_arguments->scan_line_finish_y);
        pthread_create(&threads[processor_index], NULL, obj_processor_paint_surfaces_scan_line_parallel_worker, thread_arguments);
        //printf("thread finished: %d\n", processor_index);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        pthread_join(threads[processor_index], NULL);
    }
    for(processor_index = 0; processor_index < number_of_processors; processor_index++){
        free(arguments[processor_index]);
    }
}

void *obj_processor_paint_surfaces_scan_line_parallel_worker(void *thread_arguments){
    obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *arguments = (obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *)thread_arguments;
    int scan_line_index_y, scan_line_index_x;
    for(scan_line_index_y = arguments->scan_line_start_y; scan_line_index_y < arguments->scan_line_finish_y; scan_line_index_y++){
        int active_sides_count, active_surfaces_count, active_sides_index;
        obj_processor_scan_line_side *active_sides;
        obj_processor_scan_line_surface *active_surfaces;
        obj_processor_scan_line_get_scan_line_active_surfaces_and_sides(arguments->model, arguments->image_space_vertices, scan_line_index_y, &active_sides_count, &active_surfaces_count, &active_sides, &active_surfaces);
        if(active_surfaces_count > 0) obj_processor_scan_line_sort_active_sides(active_sides, active_sides_count);
        for(scan_line_index_x = (arguments->canvas->width / 2) * -1, active_sides_index = 0; scan_line_index_x < arguments->canvas->width / 2; scan_line_index_x++){
            obj_processor_scan_line_side *next_side = active_sides + active_sides_index;
            if(active_sides_index < active_sides_count && scan_line_index_x == next_side->scan_line_intersection_x) {
                for(; active_sides_index < active_sides_count; active_sides_index++){
                    next_side = active_sides + active_sides_index;
                    if(next_side->scan_line_intersection_x > scan_line_index_x) break;
                    if(next_side->surface->surface_on && next_side->surface->surface_on_x == scan_line_index_x){
                        next_side->surface->surface_on = 0;
                        next_side->surface->vertex_on = 1;
                    }
                    else if(next_side->surface->surface_on && next_side->surface->surface_on_x != scan_line_index_x){
                        next_side->surface->surface_on = 0;
                    }
                    else if(!next_side->surface->surface_on && !next_side->surface->has_intersected_the_scan_line){
                        next_side->surface->surface_on = 1;
                        next_side->surface->surface_on_x = scan_line_index_x;
                        next_side->surface->has_intersected_the_scan_line = 1;
                    }
                }
            }
            obj_processor_scan_line_surface *paint_surface = obj_processor_scan_line_get_surface_to_paint(active_surfaces, active_surfaces_count);
            ppm_processor_pixel *surface_pixel_template = arguments->background_pixel_template;
            if(paint_surface != NULL) {
                surface_pixel_template = obj_processor_get_pixel_paint_color(paint_surface, arguments->image_space_vertices, arguments->model->vertices_attributes);
            }
            ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(arguments->canvas, scan_line_index_x, scan_line_index_y);
            pixel->red = surface_pixel_template->red;
            pixel->green = surface_pixel_template->green;
            pixel->blue = surface_pixel_template->blue;
            if(paint_surface != NULL) free(surface_pixel_template);
            obj_processor_scan_line_finish_scan_line_processing(active_surfaces, active_surfaces_count);
        }
        int surface_index;
        for(surface_index = 0; surface_index < active_surfaces_count; surface_index++){
            obj_processor_scan_line_surface *surface = active_surfaces + surface_index;
            free(surface->sides);
        }
        free(active_sides);
        free(active_surfaces);
    }
    return NULL;
}

obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *obj_processor_get_paint_surfaces_scan_line_parallel_worker_arguments(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template, int scan_line_start_y, int scan_line_finish_y){
    obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *arguments = (obj_processor_paint_surfaces_scan_line_parallel_worker_arguments *)malloc(sizeof(obj_processor_paint_surfaces_scan_line_parallel_worker_arguments));
    arguments->model = model;
    arguments->image_space_vertices = image_space_vertices;
    arguments->canvas = canvas;
    arguments->background_pixel_template = background_pixel_template;
    arguments->scan_line_start_y = scan_line_start_y;
    arguments->scan_line_finish_y = scan_line_finish_y;
    return arguments;
}

void obj_processor_paint_surfaces_scan_line(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, ppm_processor_canvas *canvas, ppm_processor_pixel *background_pixel_template){
    int scan_line_index_y, scan_line_index_x;
    for(scan_line_index_y = (canvas->height / 2) * -1; scan_line_index_y < canvas->height / 2 - 2; scan_line_index_y++){
    //for(scan_line_index_y = (canvas->height / 2) * -1; scan_line_index_y < -300; scan_line_index_y++){
        printf("------------> Processing scan line %d de %d\n", scan_line_index_y + (canvas->height / 2) + 1, canvas->height);
        int active_sides_count, active_surfaces_count, active_sides_index;
        obj_processor_scan_line_side *active_sides;
        obj_processor_scan_line_surface *active_surfaces;
        obj_processor_scan_line_get_scan_line_active_surfaces_and_sides(model, image_space_vertices, scan_line_index_y, &active_sides_count, &active_surfaces_count, &active_sides, &active_surfaces);
        if(active_surfaces_count > 0) obj_processor_scan_line_sort_active_sides(active_sides, active_sides_count);
        //printf("Superficies encontradas, lados activos: %d, superficies activas: %d\n", active_sides_count, active_surfaces_count);
        for(scan_line_index_x = (canvas->width / 2) * -1, active_sides_index = 0; scan_line_index_x < canvas->width / 2 - 2; scan_line_index_x++){
            obj_processor_scan_line_side *next_side = active_sides + active_sides_index;
            /*if(active_sides_index < active_sides_count){
                if(next_side->scan_line_intersection_x == 0) printf("Calcule mal la interseccion\n");
                else printf("Interseccion valida %d, indice siguiente lado: %d\n", next_side->scan_line_intersection_x, active_sides_index);
            }*/
            if(active_sides_index < active_sides_count && scan_line_index_x == next_side->scan_line_intersection_x) {
                //printf("Interseccion encontrada: %d\n", scan_line_index_x);
                for(; active_sides_index < active_sides_count; active_sides_index++){
                    next_side = active_sides + active_sides_index;
                    //printf("Procesando lado %d con interseccion en %d\n", active_sides_index, next_side->scan_line_intersection_x);
                    if(next_side->scan_line_intersection_x > scan_line_index_x) break;
                    if(next_side->surface->surface_on && next_side->surface->surface_on_x == scan_line_index_x){
                        //printf("switch surface\n");
                        next_side->surface->surface_on = 0;
                        next_side->surface->vertex_on = 1;
                    }
                    else if(next_side->surface->surface_on && next_side->surface->surface_on_x != scan_line_index_x){
                        //printf("switch surface\n");
                        next_side->surface->surface_on = 0;
                    }
                    else if(!next_side->surface->surface_on && !next_side->surface->has_intersected_the_scan_line){
                        //printf("switch surface\n");
                        next_side->surface->surface_on = 1;
                        next_side->surface->surface_on_x = scan_line_index_x;
                        next_side->surface->has_intersected_the_scan_line = 1;
                    }
                }
                //printf("Interseccion procesada: %d\n", scan_line_index_x);
            }
            obj_processor_scan_line_surface *paint_surface = obj_processor_scan_line_get_surface_to_paint(active_surfaces, active_surfaces_count);
            ppm_processor_pixel *surface_pixel_template = background_pixel_template;
            if(paint_surface != NULL) {
                surface_pixel_template = obj_processor_get_pixel_paint_color(paint_surface, image_space_vertices, model->vertices_attributes);
                //printf("La superficia a pintar no es nula\n");
            }
            ppm_processor_pixel *pixel = ppm_processor_get_pixel_relative(canvas, scan_line_index_x, scan_line_index_y);
            pixel->red = surface_pixel_template->red;
            pixel->green = surface_pixel_template->green;
            pixel->blue = surface_pixel_template->blue;
            if(paint_surface != NULL) free(surface_pixel_template);
            obj_processor_scan_line_finish_scan_line_processing(active_surfaces, active_surfaces_count);
            /*if(scan_line_index_x == canvas->width / 2 - 1 && paint_surface != NULL){
                int surface_index;
                for(surface_index = 0; surface_index < active_surfaces_count; surface_index++){
                    obj_processor_scan_line_surface *surface = active_surfaces + surface_index;
                    if(!surface->surface_on) continue;
                    printf("---------------> Surface %d %d\n", surface_index, scan_line_index_y);
                    int surface_side_index;
                    for(surface_side_index = 0; surface_side_index < 3; surface_side_index++){
                        obj_processor_scan_line_side *side = surface->sides + surface_side_index;
                        printf("Side %d de (%f,%f) a (%f,%f)\n", surface_side_index, side->start_point->x, side->start_point->y, side->end_point->x, side->end_point->y );
                    }
                    printf("---------------> End surface %d\n", surface_index);
                }
            }*/
        }
        free(active_sides);
        free(active_surfaces);
    }
}

void obj_processor_scan_line_get_scan_line_active_surfaces_and_sides(obj_processor_three_dimensions_model *model, obj_processor_three_dimensions_model_vertex *image_space_vertices, int scan_line_index_y, int *active_sides_count, int *active_surfaces_count, obj_processor_scan_line_side **active_sides, obj_processor_scan_line_surface **active_surfaces){
    *active_sides = (obj_processor_scan_line_side *)malloc(sizeof(obj_processor_scan_line_side) * model->faces_count * 3);
    *active_surfaces = (obj_processor_scan_line_surface *)malloc(sizeof(obj_processor_scan_line_surface) * model->faces_count);
    *active_sides_count = 0;
    *active_surfaces_count = 0;
    int face_index;
    for(face_index = 0; face_index < model->faces_count; face_index++){
        obj_processor_three_dimensions_model_face *face = model->faces + face_index;
        obj_processor_scan_line_surface *surface = *active_surfaces + *active_surfaces_count;
        if(face->back_face) continue;
        if(!obj_processor_scan_line_is_surface_active(face, scan_line_index_y, image_space_vertices)) continue;
        //printf("---------------> Surface %d %d\n", face_index, scan_line_index_y);
        obj_processor_map_face_to_surface(face, surface, image_space_vertices);
        int surface_side_index;
        for(surface_side_index = 0; surface_side_index < 3; surface_side_index++){
            obj_processor_scan_line_side *side = surface->sides + surface_side_index;
            //printf("Side %d de (%f,%f) a (%f,%f)\n", surface_side_index, side->start_point->x, side->start_point->y, side->end_point->x, side->end_point->y );
            if(!obj_processor_scan_line_is_side_crossing_scan_line(side, scan_line_index_y)) continue;
            obj_processor_scan_line_copy_side(*active_sides + *active_sides_count, side);
            (*active_sides_count)++;
        }
        //printf("---------------> End surface %d\n", face_index);
        (*active_surfaces_count)++;
    }
}

int obj_processor_scan_line_is_surface_active(obj_processor_three_dimensions_model_face *face, int scan_line_index_y, obj_processor_three_dimensions_model_vertex *image_space_vertices){
    int vertex_index, start_vertex_index, end_vertex_index;
    obj_processor_scan_line_side side;
    for(vertex_index = 0; vertex_index < face->vertex_count - 1; vertex_index++){
        start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
        end_vertex_index = *(face->vertices_indexes + vertex_index + 1) - 1;
        obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
        obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
        side.start_point = start_vertex;
        side.end_point = end_vertex;
        if(obj_processor_scan_line_is_side_crossing_scan_line(&side, scan_line_index_y)) return 1;
    }
    start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
    end_vertex_index = *(face->vertices_indexes) - 1;
    obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
    obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
    side.start_point = start_vertex;
    side.end_point = end_vertex;
    return obj_processor_scan_line_is_side_crossing_scan_line(&side, scan_line_index_y);
}

int obj_processor_scan_line_is_side_crossing_scan_line(obj_processor_scan_line_side *side, int scan_line_index_y){
    if(side->start_point->y > scan_line_index_y && side->end_point->y > scan_line_index_y) return 0;
    if(side->start_point->y < scan_line_index_y && side->end_point->y < scan_line_index_y) return 0;
    if(side->start_point->y == side->end_point->y) return 0;
    double mx = (double)(side->end_point->x - side->start_point->x);
    double my = (double)(side->end_point->y - side->start_point->y); 
    double m = mx/my;
    double b = side->start_point->x - (m * side->start_point->y);
    side->scan_line_intersection_x = (int)(m * scan_line_index_y + b + 0.5);
    //printf("Interseccion en %d", side->scan_line_intersection_x);
    //getc(stdin);
    return 1;
}

void obj_processor_map_face_to_surface(obj_processor_three_dimensions_model_face *face, obj_processor_scan_line_surface *surface, obj_processor_three_dimensions_model_vertex *image_space_vertices){
    surface->face = face;
    surface->sides = (obj_processor_scan_line_side *)malloc(sizeof(obj_processor_scan_line_side) * 3);
    int vertex_index, start_vertex_index, end_vertex_index;
    for(vertex_index = 0; vertex_index < face->vertex_count - 1; vertex_index++){
        start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
        end_vertex_index = *(face->vertices_indexes + vertex_index + 1) - 1;
        obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
        obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
        obj_processor_scan_line_side *side = surface->sides + vertex_index;
        side->start_point = start_vertex;
        side->end_point = end_vertex;
        side->surface = surface;
        side->scan_line_intersection_x = 0;
    }
    start_vertex_index = *(face->vertices_indexes + vertex_index) - 1;
    end_vertex_index = *(face->vertices_indexes) - 1;
    obj_processor_three_dimensions_model_vertex* start_vertex = image_space_vertices + start_vertex_index;
    obj_processor_three_dimensions_model_vertex* end_vertex = image_space_vertices + end_vertex_index;
    obj_processor_scan_line_side *side = surface->sides + vertex_index;
    side->start_point = start_vertex;
    side->end_point = end_vertex;
    side->surface = surface;
    side->scan_line_intersection_x = 0;
}

void obj_processor_scan_line_sort_active_sides(obj_processor_scan_line_side *active_sides, int active_sides_count){
    int bubble_sort_outer_index, bubble_sort_inner_index;
    for(bubble_sort_outer_index = 0; bubble_sort_outer_index < active_sides_count; bubble_sort_outer_index++){
        obj_processor_scan_line_side *sorting_side = active_sides + bubble_sort_outer_index;
        obj_processor_scan_line_side *minimum_intersection_x_side = sorting_side;
        for(bubble_sort_inner_index = bubble_sort_outer_index; bubble_sort_inner_index < active_sides_count; bubble_sort_inner_index ++){
            obj_processor_scan_line_side *side = active_sides + bubble_sort_inner_index;
            if(minimum_intersection_x_side->scan_line_intersection_x > side->scan_line_intersection_x) minimum_intersection_x_side = side;
        }
        obj_processor_scan_line_side auxiliar_side;
        obj_processor_scan_line_copy_side(&auxiliar_side, sorting_side);
        obj_processor_scan_line_copy_side(sorting_side, minimum_intersection_x_side);
        obj_processor_scan_line_copy_side(minimum_intersection_x_side, &auxiliar_side);
    }
}

void obj_processor_scan_line_copy_side(obj_processor_scan_line_side *target, obj_processor_scan_line_side *source){
    target->start_point = source->start_point;
    target->end_point = source->end_point;
    target->surface = source->surface;
    target->scan_line_intersection_x = source->scan_line_intersection_x;
}

obj_processor_scan_line_surface *obj_processor_scan_line_get_surface_to_paint(obj_processor_scan_line_surface *active_surfaces, int active_surfaces_count){
    int active_surfaces_index;
    for(active_surfaces_index = 0; active_surfaces_index < active_surfaces_count; active_surfaces_index++){
        obj_processor_scan_line_surface *active_surface = active_surfaces + active_surfaces_index;
        //printf("start vertex %f %f\n", active_surface->sides->start_point->x, active_surface->sides->start_point->y);
        //getc(stdin);
        if(active_surface->surface_on || active_surface->vertex_on){
            return active_surface;
        }
    }
    return NULL;
}

ppm_processor_pixel *obj_processor_get_pixel_paint_color(obj_processor_scan_line_surface *paint_surface, obj_processor_three_dimensions_model_vertex *image_space_vertices, obj_processor_three_dimensions_model_vertex_attributes *vertices_attributes){
    //int vertex_index = paint_surface->sides->start_point - image_space_vertices;
    ppm_processor_pixel *pixel_template = (ppm_processor_pixel *)malloc(sizeof(ppm_processor_pixel));
    ppm_processor_paint_pixel(pixel_template, 128, 0, 0);
    return pixel_template;
}

void obj_processor_scan_line_finish_scan_line_processing(obj_processor_scan_line_surface *active_surfaces, int active_surfaces_count){
    int surface_index;
    for(surface_index = 0; surface_index < active_surfaces_count; surface_index++){
        obj_processor_scan_line_surface *surface = active_surfaces + surface_index;
        surface->vertex_on = 0;
    }
}