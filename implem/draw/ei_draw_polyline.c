#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ei_draw_utils.h"
#include "ei_draw.h"
#include "ei_utils.h"

/**
 * \brief	Draws a line that can be made of many line segments.
 *
 * @param	surface 	Where to draw the line. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polyline. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or it can have a single point, or more.
 *				If the last point is the same as the first point, then this pixel is
 *				drawn only once.
 * @param	point_array_size The number of points in the point_array. Can be 0.
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_polyline(ei_surface_t surface, ei_point_t* point_array, size_t point_array_size, ei_color_t color, const ei_rect_t* clipper)
{
        // Cas de base
        if ( point_array_size < 2 || point_array == NULL ) { return; }

        // Initialisation
        ei_size_t surface_size = hw_surface_get_size(surface);
        ei_rect_t surface_rect = ei_surface_rect(&surface_size, clipper);
        
        if ( surface_rect.size.width == 0 || surface_rect.size.height == 0 ) { return; }

        
        uint8_t *first_pixel = hw_surface_get_buffer(surface),
                *pixel, 
                *last ;

        int id_red, id_green, id_blue, id_alpha;

        hw_surface_get_channel_indices(
                surface  , 
                &id_red  , 
                &id_green, 
                &id_blue , 
                &id_alpha
        );

        // Geometry
        int delta_x, 
            delta_y, 
            error;
 
        uint32_t abs_delta_x,
                 abs_delta_y;

        int32_t primary_axe_dir  , 
                primary_axe_error, 
                secondary_axe_dir  , 
                secondary_axe_error;
 
        // Clip
        unsigned char clipping_code_origin, 
                      clipping_code_end   ;

        // Points
        ei_point_t origin, end, tmp, cut_origin, cut_end;

        double dbl_cut_origin_x, 
               dbl_cut_origin_y, 
               dbl_cut_end_x, 
               dbl_cut_end_y;

        int limit_coord;

        // Traitements
        for (size_t i = 0; i < point_array_size - 1; i++) 
        {
                // Select
                origin = point_array[i    ];
                end    = point_array[i + 1];
                
                delta_x = end.x - origin.x;
                delta_y = end.y - origin.y;

                // Init cut
                cut_origin.x = origin.x;
                cut_origin.y = origin.y;
                cut_end.x = end.x;
                cut_end.y = end.y;

                dbl_cut_origin_x = cut_origin.x;
                dbl_cut_origin_y = cut_origin.y;
                dbl_cut_end_x = cut_end.x;
                dbl_cut_end_y = cut_end.y;

                // Initialisation du clipper du point d'arrivé
                ei_rect_t end_surface_rect = surface_rect;
                end_surface_rect.top_left.x -= 1;
                end_surface_rect.top_left.y -= 1;
                end_surface_rect.size.width  += 1;
                end_surface_rect.size.height += 1;

                clipping_code_origin = ei_clipping_code(&    surface_rect, &cut_origin);
                clipping_code_end    = ei_clipping_code(&end_surface_rect, &cut_end   );

                // Si l'on ne peut accepté ou refusé trivialement
                if ( ( clipping_code_origin | clipping_code_end ) && !( clipping_code_origin & clipping_code_end ) )
                {
                        // Coupe du point d'origine
                        if ( (clipping_code_origin & 12) && delta_y != 0 ) // Dessous ou dessus le surface_rect
                        {
                                limit_coord = surface_rect.top_left.y + ( (clipping_code_origin & 8) ? surface_rect.size.height -1 : 0 );

                                dbl_cut_origin_x += (double)( limit_coord - origin.y ) * ((double)( end.x - origin.x )) / ((double)( end.y - origin.y ));
                                dbl_cut_origin_y  = limit_coord;
                        }
                        else if ( clipping_code_origin & 3 ) // Avant ou après le surface_rect
                        {
                                limit_coord = surface_rect.top_left.x + ( (clipping_code_origin & 2) ? surface_rect.size.width -1 : 0 );

                                dbl_cut_origin_y += ((double)( limit_coord - origin.x ) * ((double)( end.y - origin.y )) / ((double)( end.x - origin.x )));
                                dbl_cut_origin_x  = limit_coord;
                        }

                        // Coupe du point d'arrivé
                        if ( (clipping_code_end & 12) && delta_y != 0 ) // Dessous ou dessus le surface_rect
                        {
                                limit_coord = end_surface_rect.top_left.y + ((clipping_code_end & 8) != 0) * end_surface_rect.size.height;

                                dbl_cut_end_x += (double)( limit_coord - end.y ) * ((double)( end.x - origin.x )) / ((double)( end.y - origin.y ));
                                dbl_cut_end_y  = limit_coord;
                        }
                        else if ( clipping_code_end & 3 ) // Avant ou après le surface_rect
                        {
                                limit_coord = end_surface_rect.top_left.x + ((clipping_code_end & 2) != 0) * end_surface_rect.size.width;

                                dbl_cut_end_y += ((double)( limit_coord - end.x ) * ((double)( end.y - origin.y )) / ((double)( end.x - origin.x )));
                                dbl_cut_end_x  = limit_coord;
                        }
                        
                        // Sauvegarde des coupes
                        cut_origin.x = round(dbl_cut_origin_x);
                        cut_origin.y = round(dbl_cut_origin_y);
                        cut_end.x = round(dbl_cut_end_x);
                        cut_end.y = round(dbl_cut_end_y);
                                
                        clipping_code_origin = ei_clipping_code(&    surface_rect, &cut_origin);
                        clipping_code_end    = ei_clipping_code(&end_surface_rect, &cut_end   );
                }

                // Refus de la line
                if ( clipping_code_origin | clipping_code_end )
                {
                        continue;
                }


                // Calcul des valeurs permettant de diriger les différentes variables lors du dessin
                // Ces directeurs sont les différentes quantités à ajouter ou soustraire au pixel et/ou à l'erreur

                abs_delta_x = abs(delta_x);
                abs_delta_y = abs(delta_y);

                // Dirigé par Y
                if ( abs_delta_x == 0 || abs_delta_y > abs_delta_x )
                {
                        primary_axe_dir   = (( delta_y < 0 ) ? -4 : 4) * surface_size.width;
                        secondary_axe_dir = (( delta_x < 0 ) ? -4 : 4);

                        primary_axe_error   = abs_delta_y;
                        secondary_axe_error = abs_delta_x;

                        error = secondary_axe_error * abs( origin.y - cut_origin.y );
                }
                // Dirigé par X
                else
                {
                        primary_axe_dir   =  ( delta_x < 0 ) ? -4 : 4;
                        secondary_axe_dir = (( delta_y < 0 ) ? -4 : 4) * surface_size.width;

                        primary_axe_error   = abs_delta_x;
                        secondary_axe_error = abs_delta_y;

                        error = secondary_axe_error * abs( origin.x - cut_origin.x );
                }

                // Calcul des pixels de début et de fin
                pixel = first_pixel + 4 * ( cut_origin.y * surface_size.width + cut_origin.x );
                last  = first_pixel + 4 * ( cut_end   .y * surface_size.width + cut_end   .x );

                // Réduction de l'erreur d'origine du point d'origine
                while ( 2 * error > primary_axe_error )
                {
                        // pixel += secondary_axe_dir;
                        error -= primary_axe_error;
                }
                
                // Dessin du pixel de fin au pixel de début
                if ( delta_y < 0 )
                {
                        while ( pixel > last )
                        {
                                pixel[id_red  ] = (color.alpha * color.red   + (255 - color.alpha) * pixel[id_red  ]) / 255;
                                pixel[id_green] = (color.alpha * color.green + (255 - color.alpha) * pixel[id_green]) / 255;
                                pixel[id_blue ] = (color.alpha * color.blue  + (255 - color.alpha) * pixel[id_blue ]) / 255;
                                pixel[id_alpha] =  color.alpha;

                                pixel += primary_axe_dir;
                                error += secondary_axe_error;
                                
                                if ( 2 * error > primary_axe_error ) 
                                {
                                        pixel += secondary_axe_dir;
                                        error -= primary_axe_error;
                                }
                        }
                }
                // Dessin du pixel du début au pixel de fin
                else
                {
                        while ( pixel < last )
                        {
                                pixel[id_red  ] = (color.alpha * color.red   + (255 - color.alpha) * pixel[id_red  ]) / 255;
                                pixel[id_green] = (color.alpha * color.green + (255 - color.alpha) * pixel[id_green]) / 255;
                                pixel[id_blue ] = (color.alpha * color.blue  + (255 - color.alpha) * pixel[id_blue ]) / 255;
                                pixel[id_alpha] =  color.alpha;

                                pixel += primary_axe_dir;
                                error += secondary_axe_error;
                                
                                if ( 2 * error > primary_axe_error ) 
                                {
                                        pixel += secondary_axe_dir;
                                        error -= primary_axe_error;
                                }
                        }
                }
        }
}