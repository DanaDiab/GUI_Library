#include "draw/ei_draw_utils.h"
#include "draw/ei_draw_frame.h"
#include "draw/ei_draw_image.h"
#include "ei_widget_attributes.h"
#include "ei_widget_configure.h"
#include "ei_widgetclass_button.h"
#include "ei_application.h"
#include "ei_event.h"




ei_widget_t allocfunc_button()
{
        return (ei_widget_t)calloc(1, sizeof(ei_impl_button_t)); 
}




void releasefunc_button(ei_widget_t widget)
{
        ei_button_t button = (ei_button_t)widget;
        
        // Libèrer le Text
        if ( button->text != NULL )
        {
                free(button->text);
                button->text = NULL;
        }

        // Libèrer la surface de l''image
        if ( button->img != NULL )
        {
                hw_surface_free(button->img);
                button->img = NULL;
        }
}




void drawfunc_button(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
        ei_button_t button = (ei_button_t)widget;

        const ei_rect_t* location     = ei_widget_get_screen_location(widget);
        const ei_rect_t* content_rect = ei_widget_get_content_rect   (widget);

        ei_rect_t childs_clipper = ei_rect_intersect(clipper, content_rect);
        

        // Draw button en tant que frame ayant des arcs
        draw_frame(
                surface, 
                *location, 
                button->color, 
                button->border_width, 
                button->corner_radius, 
                button->relief, 
                clipper
        );

        if ( button->text != NULL )
        {
                // Get content size
                ei_size_t content_size;
                hw_text_compute_size(button->text, button->text_font, &(content_size.width), &(content_size.height));

                // Get content location
                ei_point_t content_location;
                ei_get_anchored_inner_position(&content_location, button->text_anchor, content_rect->size, content_size);

                content_location.x += content_rect->top_left.x;
                content_location.y += content_rect->top_left.y;

                // Draw text
                ei_draw_text(
                        surface          , 
                        &content_location, 
                        button->text      , 
                        button->text_font , 
                        button->text_color, 
                        &childs_clipper
                );
        }
        if ( button->img != NULL )
        {
                ei_rect_t img_rect = ( button->img_rect != NULL ) ? *(button->img_rect) : hw_surface_get_rect(button->img);

                // Get content size
                ei_size_t content_size = img_rect.size;

                // Get content location
                ei_point_t content_location;
                ei_get_anchored_inner_position(&content_location, button->img_anchor, content_rect->size, content_size);

                // Pour que l'image soit à l'interieur des bordures
                content_location.x += content_rect->top_left.x;
                content_location.y += content_rect->top_left.y;

                // Draw img
                ei_draw_image(
                        surface, 
                        &content_location,
                        button->img,
                        &img_rect,
                        &childs_clipper
                );
        }

	// Dessin du widget sur la picking surface
	draw_frame(pick_surface, *location, *(ei_widget_get_pick_color(widget)), 0, button->corner_radius, ei_relief_none, clipper);

        // Dessin des child sur la picking surface
        ei_impl_widget_draw_children(widget, surface, pick_surface, &childs_clipper);
}




void setdefaultsfunc_button(ei_widget_t widget)
{
        ei_button_t button = (ei_button_t)widget;

        // Les valeurs par défault de certains attributs 
        ei_color_t default_font_color_copy = ei_font_default_color;
        int default_border_width_copy  = k_default_button_border_width ,
            default_corner_radius_copy = k_default_button_corner_radius;

        button->has_natural_size = true;

        // Configuration du button avec les valeurs par default
        ei_button_configure(
                widget,
		NULL,
		&ei_default_background_color,
                &default_border_width_copy,
                &default_corner_radius_copy,
                &(ei_relief_t    ) { ei_relief_raised      },
	        &(ei_string_t    ) { NULL                  },
		&(ei_font_t      ) { ei_default_font       },
		&default_font_color_copy,
		&(ei_anchor_t    ) { ei_anc_center         },
		&(ei_surface_t   ) { NULL                  },
		&(ei_rect_ptr_t  ) { NULL                  },
	        &(ei_anchor_t    ) { ei_anc_center         },
                &(ei_callback_t  ) { NULL                  },
                &(ei_user_param_t) { NULL                  }
        );
}




void geomnotifyfunc_button(ei_widget_t widget)
{
        ei_button_t button = (ei_button_t)widget;
        ei_rect_t content_rect = *ei_widget_get_content_rect(widget);

        // Le contenu du bouton est correctement positionné à l'intérieur des bordures
        content_rect.top_left.x += button->border_width;
        content_rect.top_left.y += button->border_width;

        // Effet visuel d'un bouton cliqué
        if ( button->relief == ei_relief_sunken )
        {
                content_rect.top_left.x += 1;
                content_rect.top_left.y += 1;
        }

        // Cet ajustement garantit que la zone de contenu ne se superpose pas avec la bordure
        content_rect.size.width  -= 2 * button->border_width;
        content_rect.size.height -= 2 * button->border_width;

        ei_widget_set_content_rect(widget, &content_rect);
}




bool handlefunc_button(ei_widget_t widget, struct ei_event_t* event)
{
        ei_button_t button = (ei_button_t)widget; 
        
        switch( event->type )
        {
                case ei_ev_mouse_buttondown:

                        if ( button->relief != ei_relief_none )
                        {
                                // Changement du relief et activation du button, le button est cliqué 
                                ei_button_configure(widget, NULL, NULL, NULL, NULL, &(ei_relief_t){ ei_relief_sunken }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                                ei_event_set_active_widget(widget);

                                return true;
                        }

                        return false;

                case ei_ev_mouse_buttonup:

                        if ( ei_event_get_active_widget() == widget )
                        {
                                ei_point_t mouse_position = event->param.mouse.where;
                                ei_widget_t picked = ei_widget_pick( &mouse_position );

                                if ( picked == widget )
                                {
                                        if ( button->relief != ei_relief_none )
                                        {
                                                // Changement de relief et inactivation du widget
                                                ei_button_configure(widget, NULL, NULL, NULL, NULL, &(ei_relief_t){ ei_relief_raised }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                                        }

                                        if ( button->callback != NULL )
                                        {
                                                // Appel du callback par défault
                                                button->callback(widget, event, button->user_param);
                                        }
                                }

                                ei_event_set_active_widget(NULL);

                                //Event traité
                                return true;
                        }
                        
                        return false;

                case ei_ev_mouse_move:

                        // Lors du clique d'un bouton sans relachement, le relief du bouton n'apparait pas en "sunken" 
                        // que lorsque la souris est positionné sur le bouton
                        
                        if ( ( button->relief != ei_relief_none ) && ( ei_event_get_active_widget() == widget ) )
                        {
                                ei_point_t mouse_position = event->param.mouse.where;
                                ei_widget_t picked = ei_widget_pick( &mouse_position );

                                if ( picked == widget )
                                {
                                        ei_button_configure(widget, NULL, NULL, NULL, NULL, &(ei_relief_t){ ei_relief_sunken }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                                }
                                else
                                {
                                        ei_button_configure(widget, NULL, NULL, NULL, NULL, &(ei_relief_t){ ei_relief_raised }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                                }

                                return true;
                        }

                        return false;

                default:
                        // Event non traité
                        return false; 
        }
}