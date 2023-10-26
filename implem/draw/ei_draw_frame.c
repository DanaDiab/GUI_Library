#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_draw_utils.h"

/*
 * Linked list of points
 */
typedef struct ei_linked_cell_point_t 
{
	ei_point_t point;
        double angle;

	struct ei_linked_cell_point_t* next;

} ei_linked_cell_point_t;




void linked_point_append_left(ei_linked_cell_point_t** ptr_list, ei_point_t point, double angle)
{
	ei_linked_cell_point_t* cell = malloc(sizeof(ei_linked_cell_point_t));
        assert( cell != NULL );

        cell->point.x = point.x;
        cell->point.y = point.y;
        cell->angle = angle;

        // Ajout en tête
	cell->next = *ptr_list;

        *ptr_list = cell;
}




void linked_point_free(ei_linked_cell_point_t** ptr_list)
{
	ei_linked_cell_point_t *current_cell = *ptr_list,
	                       *   next_cell;

        // Itère sur la liste en libèrant ses éléments
	while ( current_cell != NULL )
	{
		next_cell = current_cell->next;
		free(current_cell);

		current_cell = next_cell;
	}

	*ptr_list = NULL;
}




void get_quadrant(ei_linked_cell_point_t** point_list, uint32_t* list_length, uint32_t radius)
{       
        *list_length = 0;
        *point_list  = NULL;

        if ( radius == 0 )
        {
                (*list_length)++;
                // Angle droit
                linked_point_append_left(point_list, ei_point(0, 0), M_PI / 2);

                return;
        }


        ei_point_t current = ei_point(radius, 0);
        double error_limit = pow(radius + .5, 2);

        // 0 to PI/2
        while ( current.x >= 0 )
        {
                (*list_length)++;
                // Ajout des points dans la liste représentant les coordonnées et angles des points dans un quadrant
                linked_point_append_left(point_list, current, atan( (double)(current.y) / (double)(current.x)));

                // Build point
                current.y++;

                while ( current.x >= 0 && pow(current.x, 2) + pow(current.y, 2) > error_limit )
                {
                        // Diminuer la coordonnée x jusqu'à ce que la distance au carré au point courant dépasse la limite d'erreur
                        current.x--;
                }
        }
}




void get_shape(ei_point_t** result_array, 
               size_t* array_size, 
               ei_linked_cell_point_t** point_list, 
               ei_point_t* h_point_left , 
               ei_point_t* h_point_right,
               ei_point_t* NW_point,
               ei_point_t* NE_point,
               ei_point_t* SE_point,
               ei_point_t* SW_point,
               int radius,
               bool bottom)
{
        ei_linked_cell_point_t* current = *point_list;
        size_t i = 0, 
               j = *array_size - 1;

        int dir;
        ei_point_t *plain_corner, *cut_corner_1, *cut_corner_2;

        if ( bottom )
        {
                dir = 1;

                plain_corner = SE_point;
                cut_corner_1 = SW_point;
                cut_corner_2 = NE_point;
        }
        else
        {
                dir = -1;

                plain_corner = NW_point;
                cut_corner_1 = NE_point;
                cut_corner_2 = SW_point;
        }

        // First part of quadrant
        while ( current != NULL && current->angle >= M_PI / 4 )
        {
                // Plain corner
                (*result_array)[i].x = dir * current->point.x + plain_corner->x;
                (*result_array)[i].y = dir * current->point.y + plain_corner->y;

                // Half corner
                (*result_array)[j].x = dir * -current->point.x + cut_corner_1->x;
                (*result_array)[j].y = dir *  current->point.y + cut_corner_1->y;

                // Next
                current = current->next;
                i++; j--;
        }

        // Middle
        (*result_array)[j].x = dir * -(int)(cos(M_PI / 4) * radius) + cut_corner_1->x;
        (*result_array)[j].y = dir *  (int)(sin(M_PI / 4) * radius) + cut_corner_1->y;
        j--;

        (*result_array)[j] = *h_point_left;
        j--;

        (*result_array)[j] = *h_point_right;
        j--;

        (*result_array)[j].x = dir *  (int)(cos(M_PI / 4) * radius) + cut_corner_2->x;
        (*result_array)[j].y = dir * -(int)(sin(M_PI / 4) * radius) + cut_corner_2->y;
        j--;

        // Second part of quadrant
        while ( current != NULL )
        {
                // Plain corner
                (*result_array)[i].x = dir * current->point.x + plain_corner->x;
                (*result_array)[i].y = dir * current->point.y + plain_corner->y;

                // Half corner
                (*result_array)[j].x = dir *  current->point.x + cut_corner_2->x;
                (*result_array)[j].y = dir * -current->point.y + cut_corner_2->y;

                // Next
                current = current->next;
                i++; j--;
        }
}




void get_plain_shape(ei_point_t** result_array, 
                     ei_linked_cell_point_t** point_list, 
                     uint32_t* list_length, 
                     ei_point_t* NW_point,
                     ei_point_t* NE_point,
                     ei_point_t* SE_point,
                     ei_point_t* SW_point)
{
        size_t i = 0, id;
        ei_linked_cell_point_t* current = *point_list;

        // Transformer les points afin de les positionner correctement dans le cadre global de la forme
        while ( current != NULL )
        {
                // North-Est
                id = (*list_length) - 1 - i;
                (*result_array)[id].x =  current->point.x + NE_point->x;
                (*result_array)[id].y = -current->point.y + NE_point->y;

                // North-West
                id = (*list_length) + i;
                (*result_array)[id].x = -current->point.x + NW_point->x;
                (*result_array)[id].y = -current->point.y + NW_point->y;

                // South-West
                id = 3 * (*list_length) - 1 - i;
                (*result_array)[id].x = -current->point.x + SW_point->x;
                (*result_array)[id].y =  current->point.y + SW_point->y;

                // South-Est
                id = 3 * (*list_length) + i;
                (*result_array)[id].x =  current->point.x + SE_point->x;
                (*result_array)[id].y =  current->point.y + SE_point->y;

                // Next
                current = current->next;
                i++;
        }
}




void draw_frame(ei_surface_t* surface, ei_rect_t location, ei_color_t color, int border_width, int corner_radius, ei_relief_t relief, ei_rect_t* clipper)
{
        ei_point_t* point_array;
        size_t array_size;

        uint32_t list_length;
        ei_linked_cell_point_t* point_list = NULL;

        // Calcul middle points
        int h;
        ei_point_t h_point_right, h_point_left;
        
        if ( location.size.height < location.size.width )
        {
                h = location.size.height / 2;
                
                h_point_left = ei_point(location.top_left.x + h, location.top_left.y + h);

                h_point_right.x = location.top_left.x + location.size.width - h;
                h_point_right.y = location.top_left.y + h;
        }
        else
        {
                h = location.size.width / 2;

                h_point_right = ei_point(location.top_left.x + h, location.top_left.y + h);

                h_point_left.x = location.top_left.x +  h;
                h_point_left.y = location.top_left.y + location.size.height - h;
        }

        // Arc limit
        if ( corner_radius > h )
        {
                corner_radius = h;
        }

        // Get cardinals points
        ei_point_t NW_point = ei_point(location.top_left.x                       + corner_radius, location.top_left.y                        + corner_radius),
                   NE_point = ei_point(location.top_left.x + location.size.width - corner_radius, location.top_left.y                        + corner_radius),
                   SW_point = ei_point(location.top_left.x                       + corner_radius, location.top_left.y + location.size.height - corner_radius),
                   SE_point = ei_point(location.top_left.x + location.size.width - corner_radius, location.top_left.y + location.size.height - corner_radius);

        if ( relief != ei_relief_none )
        {
                // Calcul colors
                ei_color_t color_top    = color,
                           color_bottom = color;

                if ( relief == ei_relief_raised )
                {
                        add_at_color(&color_top   , 10);
                        sub_at_color(&color_bottom, 10);
                }
                else
                {
                        sub_at_color(&color_top   , 10);
                        add_at_color(&color_bottom, 10);
                }

                // Get quadrant
                get_quadrant(&point_list, &list_length, corner_radius);

                array_size = 2 * list_length + 4;
                point_array = calloc(array_size, sizeof(ei_point_t));

                get_shape(&point_array, &array_size, &point_list, &h_point_right, &h_point_left, &NW_point, &NE_point, &SE_point, &SW_point, corner_radius, false);
                ei_draw_polygon(surface, point_array, array_size, color_top, clipper);

                get_shape(&point_array, &array_size, &point_list, &h_point_left, &h_point_right, &NW_point, &NE_point, &SE_point, &SW_point, corner_radius, true );
                ei_draw_polygon(surface, point_array, array_size, color_bottom, clipper);

                linked_point_free(&point_list);
                free(point_array);

                // Update points
                location.top_left.x += border_width;
                location.top_left.y += border_width;

                location.size.width  -= 2 * border_width;
                location.size.height -= 2 * border_width;

                h -= border_width;
                if ( corner_radius > h )
                {
                        corner_radius = h;
                }

                NW_point = ei_point(location.top_left.x                       + corner_radius, location.top_left.y                        + corner_radius),
                NE_point = ei_point(location.top_left.x + location.size.width - corner_radius, location.top_left.y                        + corner_radius),
                SW_point = ei_point(location.top_left.x                       + corner_radius, location.top_left.y + location.size.height - corner_radius),
                SE_point = ei_point(location.top_left.x + location.size.width - corner_radius, location.top_left.y + location.size.height - corner_radius);
        }
        
        if ( corner_radius == 0 )
        {
                location = ei_rect_intersect(&location, ( clipper != NULL ) ? clipper : &location);
                ei_fill(surface, &color, &location);
        }
        else
        {
                get_quadrant(&point_list, &list_length, corner_radius);

                // Full draw shape
                array_size = 4 * list_length;
                point_array = calloc(array_size, sizeof(ei_point_t));

                get_plain_shape(&point_array, &point_list, &list_length, &NW_point, &NE_point, &SE_point, &SW_point);
                ei_draw_polygon(surface, point_array, array_size, color, clipper);

                linked_point_free(&point_list);
                free(point_array);
        }
}