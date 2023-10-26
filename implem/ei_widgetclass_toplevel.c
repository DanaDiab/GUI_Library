#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_implementation.h"
#include "ei_draw.h"
#include "draw/ei_draw_utils.h"
#include "draw/ei_draw_frame.h"
#include "ei_widget_configure.h"
#include "ei_widget_attributes.h"
#include "ei_widgetclass.h"
#include "ei_widgetclass_button.h"
#include "ei_widgetclass_toplevel.h"
#include "ei_event.h"
#include "ei_application.h"




void put_in_front_callback(ei_widget_t widget)
{
        // On ne fait rien si le widget est le dernier dessiné entre ses frères
        if ( widget->parent->children_tail == widget ){ return ;}

        ei_widget_t current = widget->parent->children_head;
        ei_widget_t previous = NULL;

        // Enlever le widget de sa place dans la liste et l'ajouter à la queue
        while ( current != NULL && current != widget)
        {
                previous  = current; 
                current = current->next_sibling; 
        }

        if ( current == widget )
        {
                if ( previous != NULL )
                {
                        previous->next_sibling = current->next_sibling;
                        widget->parent->children_tail->next_sibling = current;
                        widget->parent->children_tail = current;
                        current->next_sibling = NULL;
                }
                else
                {
                        widget->parent->children_head = current->next_sibling;
                        widget->parent->children_tail->next_sibling = current;
                        widget->parent->children_tail = current; 
                        current->next_sibling = NULL;

                }
        }

        ei_event_set_active_widget(widget);

        ei_app_invalidate_rect( &(widget->screen_location) );
}




ei_widget_t allocfunc_toplevel()
{
        return (ei_widget_t)calloc(1, sizeof(ei_impl_toplevel_t));
}




void releasefunc_toplevel(ei_widget_t widget)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;
        
        // Libèrer le title
        if ( toplevel->title != NULL )
        {
                free(toplevel->title);
        }
}




void drawfunc_toplevel(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;

        const ei_rect_t* location = ei_widget_get_screen_location(widget);
        const ei_rect_t* content_rect = ei_widget_get_content_rect(widget);

        ei_rect_t childs_clipper = ei_rect_intersect(clipper, content_rect);

        // Draw le top level qui prend la forme d'une frame 
        draw_frame(
                surface        , 
                *location      , 
                toplevel->color, 
                0              , 
                0              , 
                ei_relief_none , 
                clipper
        );


        ei_color_t secondary_color = toplevel->color;
        sub_at_color(&secondary_color, 64);
        secondary_color.alpha = 255;

        // Draw de la barre de titre de taille fixe, sous la forme de frame ( n'est pas un widget )
        ei_rect_t bar_location = *location;
        bar_location.size.height = 32;

        draw_frame(
                surface        , 
                bar_location   , 
                secondary_color, 
                0              , 
                0              , 
                ei_relief_none , 
                clipper
        );

        if ( toplevel->resizable != ei_axis_none )
        {
                // Draw du boutton de redimensionnement de taille fixe, de la forme de frame (n'est pas un widget)
                ei_rect_t resize_location;
                resize_location.size.width  = 16;
                resize_location.size.height = 16;
                resize_location.top_left.x = location->top_left.x + location->size.width  - resize_location.size.width ;
                resize_location.top_left.y = location->top_left.y + location->size.height - resize_location.size.height;

                draw_frame(
                        surface        , 
                        resize_location, 
                        secondary_color, 
                        0              , 
                        0              , 
                        ei_relief_none , 
                        clipper
                );
        }

        if ( toplevel->title != NULL )
        {
                ei_font_t text_font = ei_default_font;

                // Get title size
                ei_size_t content_size;
                hw_text_compute_size(toplevel->title, text_font, &(content_size.width), &(content_size.height));

                // Draw text et sa location
                ei_point_t content_location;
                ei_get_anchored_inner_position(&content_location, ei_anc_center, ei_size(location->size.width, 32), content_size);

                content_location.x += location->top_left.x;
                content_location.y += location->top_left.y;

                // Draw title
                ei_draw_text(
                        surface, 
                        &content_location, 
                        toplevel->title, 
                        text_font , 
                        (ei_color_t){ 0, 0, 0, 255 }, 
                        clipper
                );
        }

        // Draw le top level sur la picking surface
	draw_frame(pick_surface, *location, *(ei_widget_get_pick_color(widget)), 0, 0, ei_relief_none, clipper);

        // Draw des childs sur la picking surface
        if ( toplevel->close_button != NULL )
        {
                ei_widget_get_class(toplevel->close_button)->drawfunc(toplevel->close_button, surface, pick_surface, clipper);
        }

        ei_impl_widget_draw_children(widget, surface, pick_surface, &childs_clipper);
}




void setdefaultsfunc_toplevel(ei_widget_t widget)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;
        
        ei_color_t default_background_color_copy = ei_default_background_color;

        toplevel->state = DEFAULT;

        // Defaults values
        ei_toplevel_configure(
                widget,
		&(ei_size_t    ) { 320, 240     },
                &default_background_color_copy,
                &(int          ) { 4            },
                &(ei_string_t  ) { "Toplevel"   },
                &(bool         ) { true         },
                &(ei_axis_set_t) { ei_axis_both },
                &(ei_size_ptr_t) { NULL         }
        );
}




void geomnotifyfunc_toplevel(ei_widget_t widget)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;

        ei_rect_t content_rect = *ei_widget_get_content_rect(widget);

        // Ajustement pour le contenu ne se chevauche pas avec la bordure et qu'il soit bien entre les bordures
        content_rect.top_left.x += toplevel->border_width     ;
        content_rect.top_left.y += toplevel->border_width + 32;

        content_rect.size.width  -= 2 * toplevel->border_width     ;
        content_rect.size.height -= 2 * toplevel->border_width + 32;

        ei_widget_set_content_rect(widget, &content_rect);

        // Placer le "boutton" de fermeture 
        if ( toplevel->state != MOVING )
        {
                ei_place(
                        toplevel->close_button, 
                        &(ei_anchor_t){ ei_anc_southwest }, 
                        &(int){ -1 * toplevel->border_width + 8 }, 
                        &(int){ -1 * toplevel->border_width - 8 }, 
                        NULL, NULL, NULL, NULL, NULL, NULL
                );
        }
}




bool handlefunc_toplevel(ei_widget_t widget, struct ei_event_t*	event)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;

        const ei_rect_t* location = ei_widget_get_screen_location(widget);
        const ei_rect_t* content_rect = ei_widget_get_content_rect(widget);

        switch( event->type )
        {
                case ei_ev_mouse_buttondown:


                        // Si on clique sur le widget, on le met en premier plan (même s'il y était déjà)
                        put_in_front_callback(widget);

                        // Position de la souris par rapport à la toplevel
                        toplevel->mouse_relative_position = ei_point_sub(event->param.mouse.where, location->top_left);

                        // Souris positionnée à la title barre
                        if ( toplevel->mouse_relative_position.y < 32 )
                        {
                                // Activation du toplevel
                                ei_event_set_active_widget(widget);

                                // Declarer le mouvement du toplevel entier 
                                toplevel->state = MOVING;
        
                                return true;
                        }
                        //Souris positionnée au "bouton" de redimensionnement s'il y en a un
                        else if ( toplevel->resizable != ei_axis_none &&
                                  toplevel->mouse_relative_position.x >= location->size.width  - 16 &&
                                  toplevel->mouse_relative_position.y >= location->size.height - 16 )
                        {
                                // Activation du toplevel
                                ei_event_set_active_widget(widget);

                                // Declarer le redimensionnement du toplevel entier 
                                toplevel->state = RESIZING;

                                return true;
                        }

                        return false;

                case ei_ev_mouse_buttonup:

                        if ( widget == ei_event_get_active_widget() )
                        {
                                // Inactivation du toplevel
                                ei_event_set_active_widget(NULL);
                                return true;
                        }
                        return false;
                        
                case ei_ev_mouse_move:
                        if ( widget == ei_event_get_active_widget() )
                        {
                                ei_point_t mouse_position = event->param.mouse.where;

                                if ( toplevel->state == MOVING )
                                {
                                        ei_widget_t parent = ei_widget_get_parent(widget);

                                        // Déplacer le toplevel avec le déplacement de la souris
                                        if ( parent != NULL )
                                        {
                                                const ei_rect_t* parent_content_rect = ei_widget_get_content_rect(parent);

                                                ei_place_xy(
                                                        widget, 
                                                        mouse_position.x - toplevel->mouse_relative_position.x - parent_content_rect->top_left.x, 
                                                        mouse_position.y - toplevel->mouse_relative_position.y - parent_content_rect->top_left.y
                                                );
                                        }
                                        else
                                        {
                                                ei_place_xy(
                                                        widget, 
                                                        mouse_position.x - toplevel->mouse_relative_position.x, 
                                                        mouse_position.y - toplevel->mouse_relative_position.y
                                                );
                                        }

                                        return true;
                                }
                                else if ( toplevel->state == RESIZING )
                                {
                                        // L'emplacement futur de la souris par rapport au toplevel qui va servir à déterminer la taille du toplevel
                                        ei_point_t next_mouse_relative_position = ei_point_sub(mouse_position, ei_widget_get_screen_location(widget)->top_left);
                                        
                                        ei_size_t new_size;
                                        new_size.width  = next_mouse_relative_position.x - toplevel->mouse_relative_position.x + content_rect->size.width ;
                                        new_size.height = next_mouse_relative_position.y - toplevel->mouse_relative_position.y + content_rect->size.height;

                                        // Changement de la taille
                                        // Le but etant que si la min size est atteinte et que la souris (toujours cliqué) s'éloigne du button de redimensionnement,
                                        // le redimensionmmenet ne va pas s'effectuer de nouveau que jusqu'à ce que la souris revient à la position du button
                                        if ( new_size.width >= toplevel->min_size.width )
                                        {
                                                toplevel->mouse_relative_position.x = mouse_position.x - location->top_left.x;
                                        }
                                        else
                                        {
                                                new_size.width = toplevel->min_size.width;
                                        }

                                        if ( new_size.height >= toplevel->min_size.height )
                                        {
                                                toplevel->mouse_relative_position.y = mouse_position.y - location->top_left.y;
                                        }
                                        else
                                        {
                                                new_size.height = toplevel->min_size.height;

                                        }

                                        ei_toplevel_configure(widget, &new_size, NULL, NULL, NULL, NULL, NULL, NULL);

                                        return true;
                                }
                        }

                        return false;
                default:
                        return false;
        }

        return false;
}
