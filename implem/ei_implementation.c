#include "ei_implementation.h"
#include "ei_utils.h"
#include "draw/ei_draw_utils.h"
#include "ei_application.h"
#include "ei_widget_attributes.h"

#define get_location_point(abs, rel, distance) ( abs != -1 ) * abs + ( rel != -1 ) * rel * distance
#define get_location_size(abs, rel, distance, default) rel != -1 ? rel * distance : ( abs != -1 ? abs : default )




void ei_impl_placer_run(ei_widget_t widget)
{
        // Screen location
        ei_rect_t old_location = widget->screen_location;

        widget->screen_location.top_left = ei_point_zero();
        widget->screen_location.size     = widget->requested_size;

        // Gestionnaire de géométrie
        if ( widget->placer_params != NULL )
        {
                ei_rect_t parent_rect = (widget->parent != NULL) ? widget->parent->content_rect : ei_rect_zero();
                struct ei_impl_placer_params_t* placer_params = widget->placer_params;

                // X & Y
                widget->screen_location.top_left.x += parent_rect.top_left.x + get_location_point(placer_params->x, placer_params->rel_x, parent_rect.size.width );
                widget->screen_location.top_left.y += parent_rect.top_left.y + get_location_point(placer_params->y, placer_params->rel_y, parent_rect.size.height);
                
                // Width & Height
                widget->screen_location.size.width  = get_location_size(placer_params->width , placer_params->rel_width , parent_rect.size.width , widget->screen_location.size.width );
                widget->screen_location.size.height = get_location_size(placer_params->height, placer_params->rel_height, parent_rect.size.height, widget->screen_location.size.height);

                // Anchor
                switch ( widget->placer_params->anchor )
                {
                        // // Ajustement de la coordonnée X en fonction des positions d'ancrage
                        case ei_anc_north :
                        case ei_anc_center:
                        case ei_anc_south :
                                widget->screen_location.top_left.x -= widget->screen_location.size.width / 2;
                                break;

                        case ei_anc_northeast:
                        case ei_anc_east     :
                        case ei_anc_southeast:
                                widget->screen_location.top_left.x -= widget->screen_location.size.width;
                                break;

                        default:
                                break;
                }
                switch ( widget->placer_params->anchor )
                {
                        // Ajustement de la coordonnée Y en fonction des positions d'ancrage
                        
                        case ei_anc_west  :
                        case ei_anc_center:
                        case ei_anc_east  :
                                widget->screen_location.top_left.y -= widget->screen_location.size.height / 2;
                                break;

                        case ei_anc_southwest:
                        case ei_anc_south    :
                        case ei_anc_southeast:
                                widget->screen_location.top_left.y -= widget->screen_location.size.height;
                                break;

                        default:
                                break;
                }
        }

        // Content rect
	widget->content_rect = widget->screen_location;

        // Notify
        widget->wclass->geomnotifyfunc(widget);

        // Invalide rectangle
        ei_app_invalidate_rect(&(widget->screen_location));
        ei_app_invalidate_rect(&old_location);

        // Childrens
        ei_widget_t current_widget = widget->children_head;

        while ( current_widget != NULL )
        {
                if ( ei_widget_is_displayed(current_widget) )
                {
                        ei_impl_placer_run(current_widget);
                }

                current_widget = current_widget->next_sibling;
        }
}




void ei_impl_widget_draw_children(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
        if ( clipper->size.width != 0 && clipper->size.height != 0 )
        {
                ei_widget_t current_widget = widget->children_head;
                ei_rect_t intersection;

                while ( current_widget != NULL )
                {
                        // Dessine les enfants du widget dans le cas où ils sont placés à l'écran
                        if ( ei_widget_is_displayed(current_widget) )
                        {
                                intersection = ei_rect_intersect(ei_widget_get_screen_location(current_widget), clipper);

                                current_widget->wclass->drawfunc(current_widget, surface, pick_surface, &intersection);
                        }
                        
                        current_widget = current_widget->next_sibling;
                }
        }
}




void ei_get_anchored_inner_position(ei_point_t* location, ei_anchor_t anchor, ei_size_t parent_size, ei_size_t object_size)
{
        // X
        switch ( anchor )
        {
                case ei_anc_north:
                case ei_anc_center:
                case ei_anc_south:
                        
                        // Afin que le milieu du coté nord de l'objet soit ancré au milieur du coté nord de son parent
                        location->x = parent_size.width / 2 - object_size.width / 2;
                        break;

                case ei_anc_northeast:
                case ei_anc_east:
                case ei_anc_southeast:
                        
                        // Afin que le coté est du widget soit ancré à celui de son parent
                        location->x = parent_size.width - object_size.width;
                        break;

                default:
                        location->x = 0;
                        break;
        }

        // Y
        switch ( anchor )
        {
                case ei_anc_west:
                case ei_anc_center:
                case ei_anc_east:
                        
                        // Afin de centrer verticalement le widget avec son parent
                        location->y = parent_size.height / 2 - object_size.height / 2;
                        break;

                case ei_anc_southwest:
                case ei_anc_south:
                case ei_anc_southeast:
                        
                        //Afin d'aligner le coté sud de widget avec celui de son parent
                        location->y = parent_size.height - object_size.height;
                        break;

                default:
                        location->y = 0;
                        break;
        }
}