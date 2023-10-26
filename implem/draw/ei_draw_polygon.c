#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "ei_draw_utils.h"
#include "ei_draw.h"
#include "ei_utils.h"
#include "ei_draw_types.h"


ei_circular_linked_cell_point_t* ei_get_point_by_cut_x_line(ei_circular_linked_cell_point_t* point_list, int limit_coord, char limit_dir)
{
        // Init
        ei_circular_linked_cell_point_t *result = NULL,
                                      *current = point_list;

        double x_origin, x_end;
        int    y_origin, y_end;

        // Si la liste n'est pas vide
        if ( current != NULL )
        {
                bool origin_is_out, end_is_out;

                // Tant que l'on a pas parcouru toute la liste
                do
                {
                        // Calcul de la sortie possible des points
                        x_origin = current->x;
                        y_origin = current->y;

                        x_end = current->next->x;
                        y_end = current->next->y;

                        origin_is_out = limit_dir * x_origin < limit_dir * limit_coord;
                        end_is_out    = limit_dir * x_end    < limit_dir * limit_coord;

                        // Twice out
                        if ( !(origin_is_out && end_is_out) )
                        {
                                // Origin out only
                                if ( origin_is_out && !end_is_out )
                                {
                                        y_origin += ((double)( limit_coord - x_origin ) * ((double)( y_end - y_origin )) / ((double)( x_end - x_origin )));
                                        x_origin  = limit_coord;

                                        circular_linked_point_append_left(&result, x_origin, y_origin);
                                }
                                // End out only
                                else if ( end_is_out && !origin_is_out )
                                {
                                        y_end += ((double)( limit_coord - x_end ) * ((double)( y_end - y_origin )) / ((double)( x_end - x_origin )));
                                        x_end  = limit_coord;
                                }

                                circular_linked_point_append_left(&result, x_end, y_end);
                        }

                        current = current->next;
                
                } while ( !(current->head) );

                // Libération de la liste
                circular_linked_point_free(&point_list);
        }

        return result;
}
ei_circular_linked_cell_point_t* ei_get_point_by_cut_y_line(ei_circular_linked_cell_point_t* point_list, int limit_coord, char limit_dir)
{
        // Init
        ei_circular_linked_cell_point_t *result = NULL,
                                      *current = point_list;

        double x_origin, x_end;
        int    y_origin, y_end;

        // Si la liste n'est pas vide
        if ( current != NULL )
        {
                bool origin_is_out, end_is_out;

                // Tant que l'on a pas parcouru toute la liste
                do
                {
                        // Calcul de la sortie possible des points
                        x_origin = current->x;
                        y_origin = current->y;

                        x_end = current->next->x;
                        y_end = current->next->y;

                        origin_is_out = limit_dir * y_origin < limit_dir * limit_coord;
                        end_is_out    = limit_dir * y_end    < limit_dir * limit_coord;

                        // Twice out
                        if ( !(origin_is_out && end_is_out) )
                        {
                                // Origin out only
                                if ( origin_is_out && !end_is_out )
                                {
                                        x_origin += (double)( limit_coord - y_origin ) * ((double)( x_end - x_origin )) / ((double)( y_end - y_origin ));
                                        y_origin  = limit_coord;

                                        circular_linked_point_append_left(&result, x_origin, y_origin);
                                }
                                // End out only
                                else if ( end_is_out && !origin_is_out )
                                {
                                        x_end += (double)( limit_coord - y_end ) * ((double)( x_end - x_origin )) / ((double)( y_end - y_origin ));
                                        y_end  = limit_coord;
                                }

                                circular_linked_point_append_left(&result, x_end, y_end);
                        }

                        current = current->next;
                
                } while ( !(current->head) );

                circular_linked_point_free(&point_list);
        }

        return result;
}

ei_side_table_t ei_create_side_table(int* ptr_y, int height, ei_point_t * point_array, size_t point_array_size, const ei_rect_t* clipper)
{
        // Initialisation
        *ptr_y = height;

        int min_y;
        ei_side_table_t side_table = calloc(height, sizeof(ei_side_list_t*));

        ei_point_t origin,
                   end   ;

        char clipping_code_origin, clipping_code_end;

        ei_circular_linked_cell_point_t* point_list = NULL;
        
        bool origin_is_out, end_is_out;

        // Add point in list
        for ( size_t i = 0 ; i < point_array_size ; i++ )
        {
                circular_linked_point_append_left(&point_list, point_array[i].x, point_array[i].y);
        }

        // Cuts
        point_list = ei_get_point_by_cut_x_line(point_list, clipper->top_left.x, 1);
        point_list = ei_get_point_by_cut_y_line(point_list, clipper->top_left.y, 1);
        point_list = ei_get_point_by_cut_y_line(point_list, clipper->top_left.y + clipper->size.height, -1);
        point_list = ei_get_point_by_cut_x_line(point_list, clipper->top_left.x + clipper->size.width , -1);

        // Add points in sides table
        if ( point_list != NULL )
        {
                ei_circular_linked_cell_point_t* current = point_list;

                // Parcours de la liste des points
                do
                {
                        // Copie des points
                        origin = ei_point(round(current      ->x), current      ->y);
                        end    = ei_point(round(current->next->x), current->next->y);

                        // Si la différence d'ordonnée n'est pas nulle
                        if ( origin.y != end.y )
                        {
                                // Ajout dans la table
                                min_y = ei_side_table_append(side_table, height, &origin, &end);

                                // Mise à jour de la scanline
                                if ( min_y < *ptr_y )
                                {
                                        *ptr_y = min_y;
                                }
                        }

                        current = current->next;

                } while ( !(current->head) );

                // Libération de la liste des points
                circular_linked_point_free(&point_list);
        }

        return side_table;
}


/**
 * \brief	Draws a filled polygon.
 *
 * @param	surface 	Where to draw the polygon. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polygon. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or else it must have more than 2 points.
 * @param	point_array_size The number of points in the point_array. Must be 0 or more than 2.
 * @param	color		The color used to draw the polygon. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_polygon(ei_surface_t surface, ei_point_t * point_array, size_t point_array_size, ei_color_t color, const ei_rect_t * clipper)
{
        if ( point_array_size < 2 || point_array == NULL ) { return; }

        // Initialisation
	ei_size_t surface_size = hw_surface_get_size(surface);
        ei_rect_t surface_rect = ei_surface_rect(&surface_size, clipper);

        uint8_t* first_pixel = hw_surface_get_buffer(surface);

        int id_red, id_green, id_blue, id_alpha;

        hw_surface_get_channel_indices(
                surface  , 
                &id_red  , 
                &id_green, 
                &id_blue , 
                &id_alpha
        );

        int x, y;

        // Construction de la table des côtés
        ei_linked_side_cell_t *current, *next;

        // Création de la table des côtés
        ei_side_table_t side_table = ei_create_side_table(&y, surface_size.height, point_array, point_array_size, &surface_rect);
        ei_side_list_t active_side_list = NULL;

	if ( y < surface_size.height )
	{
		uint8_t *pixel;
		uint8_t *y_linear_position = first_pixel + 4 * surface_size.width * ( y - 1 ); // On se place une ligne avant le début

		do 
                {
                        // Parcours de la liste des côtés actifs
			current = active_side_list;
			while (current != NULL) 
                        {
				next = current->next;

				if (y == current->y_max)
                                {
					// Suppression des côtés ne devant plus être actifs
					ei_side_list_pop(&active_side_list, current);
				} 
                                else 
                                {
					// Retri la liste des côtés actifs (dans le cas où deux côtés se croisent)
					ei_side_list_resort(&active_side_list, current);
				}

				current = next;
			}

			// Ajout des côtés devant devenir actifs à la liste côtés actifs
			ei_side_list_extend(&active_side_list, side_table[y]);

			if (active_side_list != NULL)
                        {
				// Recalcul des pixels
				y_linear_position += 4 * surface_size.width;
				pixel = y_linear_position + 4 * (int) (active_side_list->x_y_min);
				x = (int) (active_side_list->x_y_min);

				// Mise à jour du premier côté actif
				active_side_list->x_y_min += active_side_list->gradiant;

                                // Pour tous les côtés actifs à partir du second
				current = active_side_list->next;
				while (current != NULL) 
                                {
                                        // Tant que l'on est en mode d'écriture
                                        while ( pixel < y_linear_position + 4 * (int) (current->x_y_min) )
                                        {
                                                pixel[id_red  ] = (color.alpha * color.red   + (255 - color.alpha) * pixel[id_red  ]) / 255;
                                                pixel[id_green] = (color.alpha * color.green + (255 - color.alpha) * pixel[id_green]) / 255;
                                                pixel[id_blue ] = (color.alpha * color.blue  + (255 - color.alpha) * pixel[id_blue ]) / 255;
                                                pixel[id_alpha] =  color.alpha;

                                                pixel += 4;
                                                x++;
                                        }

                                        // Mise à jour du côté actif courant
                                        current->x_y_min += current->gradiant;

					// Saut d'un côté vide
                                        if (current->next != NULL) 
                                        {
                                                pixel += 4 * (int) (current->next->x_y_min - current->x_y_min);
                                                x += (int) (current->next->x_y_min - current->x_y_min);

                                                // Passage au côté suivant
                                                current = current->next;

                                                // Mise à jour du côté actif courant
                                                current->x_y_min += current->gradiant;
                                        } 

                                        // Passage au côté suivant
                                        current = current->next;
				}
			}

			y++;

		} while ( active_side_list != NULL );
	}

        // Libération de la table des côtés
	free(side_table);
}
