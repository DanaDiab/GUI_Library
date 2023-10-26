#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ei_draw.h"
#include "ei_utils.h"
#include "ei_draw_utils.h"




void ei_fill(ei_surface_t surface, const ei_color_t* color, const ei_rect_t* clipper)
{
        // Valeur par défaut
	if ( color == NULL ) 
        {
		color = &((ei_color_t){ 0, 0, 0, 255 }); // Noir opaque
        }

        // Initialisation
	ei_size_t surface_size = hw_surface_get_size(surface);
        ei_rect_t surface_rect = ei_surface_rect(&surface_size, clipper);

        int id_red, id_green, id_blue, id_alpha;

        hw_surface_get_channel_indices(
                surface  , 
                &id_red  , 
                &id_green, 
                &id_blue , 
                &id_alpha
        );

        uint8_t* pixel = hw_surface_get_buffer(surface);

        // Nombre d'octet pour le saut en fin du rectangle de destination et retour à la ligne
        uint32_t line_skip = ( surface_size.width - surface_rect.size.width ) << 2; 

        // Traitements
        // Saut des pixels jusqu'au premier point à modifier
        pixel += ( surface_rect.top_left.y * surface_size.width +  surface_rect.top_left.x ) << 2; 

        if ( color->alpha != 255 )
        {
                uint32_t   red_proportion = color->alpha * color->red  ,
                        green_proportion = color->alpha * color->green,
                        blue_proportion = color->alpha * color->blue ;

                uint8_t reversed_alpha = 255 - color->alpha;

                for ( int y = 0 ; y < surface_rect.size.height ; y++ )
                {
                        for ( int x = 0 ; x < surface_rect.size.width ; x++ )
                        {
                                pixel[id_red  ] = (  red_proportion + reversed_alpha * pixel[id_red  ]) / 255;
                                pixel[id_green] = (green_proportion + reversed_alpha * pixel[id_green]) / 255;
                                pixel[id_blue ] = ( blue_proportion + reversed_alpha * pixel[id_blue ]) / 255;
                                pixel[id_alpha] = color->alpha;
                                
                                pixel += 4;
                        }
                        
                        pixel += line_skip;
                }
        }
        else
        {
                for ( int y = 0 ; y < surface_rect.size.height ; y++ )
                {
                        for ( int x = 0 ; x < surface_rect.size.width ; x++ )
                        {
                                pixel[id_red  ] = color->red  ;
                                pixel[id_green] = color->green;
                                pixel[id_blue ] = color->blue ;
                                pixel[id_alpha] = color->alpha;
                                
                                pixel += 4;
                        }
                        
                        pixel += line_skip;
                }
        }
}




void ei_draw_text(ei_surface_t surface, const ei_point_t* where, ei_const_string_t text, ei_font_t font, ei_color_t color, const ei_rect_t* clipper)
{	
        if ( font == NULL ) { font = ei_default_font; }

        // Initilisation
        ei_surface_t text_surface = hw_text_create_surface(text, font, color);

        ei_size_t surface_size = hw_surface_get_size(     surface);
        ei_size_t    text_size = hw_surface_get_size(text_surface);

        ei_rect_t surface_rect = ei_surface_rect(&surface_size, clipper);
        ei_rect_t    text_rect = ei_surface_rect(&   text_size, clipper);

        ei_rect_t rel_text_rect = ei_rect(*where, text_size);
        
        // Intersect
        surface_rect = ei_rect_intersect(&surface_rect, &rel_text_rect);
        text_rect = ei_rect(ei_point_sub(surface_rect.top_left, *where), surface_rect.size);

        if ( surface_rect.size.width == 0 || text_rect.size.width == 0 ) { return; }

        // Copy
        hw_surface_lock(text_surface);
        ei_copy_surface(surface, &surface_rect, text_surface, &text_rect, true);
        hw_surface_unlock(text_surface);

        hw_surface_free(text_surface);
}




int ei_copy_surface(ei_surface_t destination, const ei_rect_t* dst_rect, ei_surface_t source, const ei_rect_t* src_rect, bool alpha) 
{       
        // Initialisation
        ei_size_t dst_surface_size = hw_surface_get_size(destination);
        ei_size_t src_surface_size = hw_surface_get_size(source     );

        ei_rect_t dst_surface_rect = ei_surface_rect(&dst_surface_size, dst_rect);
        ei_rect_t src_surface_rect = ei_surface_rect(&src_surface_size, src_rect);

        // Intersect
        ei_rect_t rel_src_rect = ei_rect(dst_rect->top_left, src_surface_rect.size);
        dst_surface_rect = ei_rect_intersect(&dst_surface_rect, &rel_src_rect);

        ei_rect_t rel_dst_rect = ei_rect(ei_point_add(ei_point_sub(dst_surface_rect.top_left, dst_rect->top_left), src_rect->top_left), dst_surface_rect.size);
        src_surface_rect = ei_rect_intersect(&src_surface_rect, &rel_dst_rect);

        if ( dst_surface_rect.size.width == 0 || dst_surface_rect.size.width != src_surface_rect.size.width || dst_surface_rect.size.height != src_surface_rect.size.height )
        {
                return 1;
        }

        // Draw
        uint8_t *dst_first = hw_surface_get_buffer(destination) + 4 * (dst_surface_size.width * dst_surface_rect.top_left.y + dst_rect->top_left.x ),
                *src_first = hw_surface_get_buffer(source     ) + 4 * (src_surface_size.width * src_surface_rect.top_left.y + src_rect->top_left.x );

        if ( alpha )
        {
                uint8_t *dst_pixel = dst_first, 
                        *src_pixel = src_first;

                int id_red, id_green, id_blue, id_alpha;

                hw_surface_get_channel_indices(
                        source   , 
                        &id_red  , 
                        &id_green, 
                        &id_blue , 
                        &id_alpha
                );

                //Iteration sur les coordonnées des pixels
                for ( int y = 0 ; y < dst_surface_rect.size.height ; y++ )
                {
                        for ( int x = 0 ; x < dst_surface_rect.size.width ; x++ )
                        {
                                dst_pixel[id_red  ] = (src_pixel[id_alpha] * src_pixel[id_red  ] + (255 - src_pixel[id_alpha]) * dst_pixel[id_red  ]) / 255;
                                dst_pixel[id_green] = (src_pixel[id_alpha] * src_pixel[id_green] + (255 - src_pixel[id_alpha]) * dst_pixel[id_green]) / 255;
                                dst_pixel[id_blue ] = (src_pixel[id_alpha] * src_pixel[id_blue ] + (255 - src_pixel[id_alpha]) * dst_pixel[id_blue ]) / 255;
                                dst_pixel[id_alpha] = 255;

                                dst_pixel += 4;
                                src_pixel += 4;
                        }

                        //Skip les pixels du début de ligne en dehors des rectangles
                        dst_pixel += 4 * ( dst_surface_size.width - dst_surface_rect.size.width );
                        src_pixel += 4 * ( src_surface_size.width - src_surface_rect.size.width );
                }
        }
        else
        {
                uint32_t *dst_pixel = (uint32_t*)dst_first, 
                         *src_pixel = (uint32_t*)src_first;

                //Iteration sur les coordonnées des pixels
                for ( int y = 0 ; y < dst_surface_rect.size.height ; y++ )
                {
                        for ( int x = 0 ; x < dst_surface_rect.size.width ; x++ )
                        {
                                dst_pixel = src_pixel;

                                dst_pixel++;
                                src_pixel++;
                        }

                        //Skip les pixels du début de ligne en dehors des rectangles
                        dst_pixel += dst_surface_size.width - dst_surface_rect.size.width;
                        src_pixel += src_surface_size.width - src_surface_rect.size.width;
                }
        }

        return 0;

}

