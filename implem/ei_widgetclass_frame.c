#include "draw/ei_draw_utils.h"
#include "draw/ei_draw_image.h"
#include "draw/ei_draw_frame.h"
#include "ei_widget_configure.h"
#include "ei_widget_attributes.h"
#include "ei_widgetclass_frame.h"
#include "ei_application.h"
#include "ei_event.h"




ei_widget_t allocfunc_frame()
{
        return (ei_widget_t)calloc(1, sizeof(ei_impl_frame_t)); 
}




void releasefunc_frame(ei_widget_t widget)
{
        ei_frame_t frame = (ei_frame_t)widget;
        
        // Libèrer le Text
        if ( frame->text != NULL )
        {
                free(frame->text);
                frame->text = NULL;
        }

        // Libèrer la surface de l''image
        if ( frame->img != NULL )
        {
                hw_surface_free(frame->img);
                frame->img = NULL;
        }
}




void drawfunc_frame(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
        ei_frame_t frame = (ei_frame_t)widget;

        const ei_rect_t* location     = ei_widget_get_screen_location(widget);
        const ei_rect_t* content_rect = ei_widget_get_content_rect   (widget);

        ei_rect_t childs_clipper = ei_rect_intersect(clipper, content_rect);

        // Draw frame à partir des caracteristiques du widget en paramètre
        draw_frame(
                surface, 
                *location, 
                frame->color, 
                frame->border_width, 
                0, 
                frame->relief, 
                clipper
        );

        if ( frame->text != NULL )
        {
                // Get text size
                ei_size_t content_size;
                hw_text_compute_size(frame->text, frame->text_font, &(content_size.width), &(content_size.height));

                // Get text location 
                ei_point_t content_location;
                ei_get_anchored_inner_position(&content_location, frame->text_anchor, content_rect->size, content_size);

                content_location.x += content_rect->top_left.x;
                content_location.y += content_rect->top_left.y;

                // Draw text
                ei_draw_text(
                        surface          , 
                        &content_location, 
                        frame->text      , 
                        frame->text_font , 
                        frame->text_color, 
                        &childs_clipper
                );
        }
        else if ( frame->img != NULL )
        {
                ei_rect_t img_rect = ( frame->img_rect != NULL ) ? *(frame->img_rect) : hw_surface_get_rect(frame->img);

                // Get image size
                ei_size_t content_size = img_rect.size;

                // Get image location
                ei_point_t content_location;
                ei_get_anchored_inner_position(&content_location, frame->img_anchor, content_rect->size, content_size);

                content_location.x += content_rect->top_left.x;
                content_location.y += content_rect->top_left.y;

                // Draw img
                ei_draw_image(
                        surface, 
                        &content_location,
                        frame->img,
                        &img_rect,
                        &childs_clipper
                );
        }

	// Draw the frame on the picking surface
	draw_frame(pick_surface, *location, *(ei_widget_get_pick_color(widget)), 0, 0, ei_relief_none, clipper);

        // Childs
        ei_impl_widget_draw_children(widget, surface, pick_surface, &childs_clipper);
}




void setdefaultsfunc_frame(ei_widget_t widget)
{
        ei_frame_t frame = (ei_frame_t)widget;
        ei_color_t default_font_color_copy = ei_font_default_color;

        frame->has_natural_size = true;

        // Configuration du frame avec les valeurs par default
        ei_frame_configure(
                widget,
		NULL,
		&ei_default_background_color,
                &(int          ) { 0                     },
                &(ei_relief_t  ) { ei_relief_none        },
	        &(ei_string_t  ) { NULL                  },
		&(ei_font_t    ) { ei_default_font       },
		&default_font_color_copy,
		&(ei_anchor_t  ) { ei_anc_center         },
		&(ei_surface_t ) { NULL                  },
		&(ei_rect_ptr_t) { NULL                  },
	        &(ei_anchor_t  ) { ei_anc_center         }
        );
}




void geomnotifyfunc_frame(ei_widget_t widget)
{
        ei_frame_t frame = (ei_frame_t)widget;
        ei_rect_t content_rect = *ei_widget_get_content_rect(widget);

        // Le contenu du frame est correctement positionné à l'intérieur des bordures
        content_rect.top_left.x += frame->border_width;
        content_rect.top_left.y += frame->border_width;

        // Effet visuel d'un relief
        if ( frame->relief == ei_relief_sunken )
        {
                content_rect.top_left.x += 1;
                content_rect.top_left.y += 1;
        }
        
        // Cet ajustement garantit que la zone de contenu ne se superpose pas avec la bordure 
        content_rect.size.width  -= 2 * frame->border_width;
        content_rect.size.height -= 2 * frame->border_width;

        ei_widget_set_content_rect(widget, &content_rect);
}



// L'event passé en paramètre n'est pas traité ici.
bool handlefunc_frame(ei_widget_t widget, struct ei_event_t* event) { return false; }
