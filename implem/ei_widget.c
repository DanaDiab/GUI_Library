#include <assert.h>
#include "ei_implementation.h"
#include "ei_utils.h"
#include "draw/ei_draw_utils.h"
#include "ei_draw.h"
#include "ei_widget.h"
#include "ei_placer.h"
#include "ei_application.h"
#include "ei_application_utils.h"
#include "ei_event.h"



// First available pick id
static uint32_t pick_last_id = 0;




ei_widget_t ei_widget_create(ei_const_string_t class_name, ei_widget_t parent, ei_user_param_t user_data, ei_widget_destructor_t destructor)
{
        ei_widgetclass_t* wclass = ei_widgetclass_from_name(class_name);
        assert(wclass != NULL);

        // Creation et initialisation par défaut du widget en fonction de la classe retourné par la fonction from_name
        ei_widget_t widget = wclass->allocfunc();

        widget->pick_id = pick_last_id++;

        widget->pick_color = malloc(sizeof(ei_color_t));
        assert(widget->pick_color != NULL);
        widget->pick_color->red   = ( widget->pick_id       ) & 255;
        widget->pick_color->green = ( widget->pick_id >>  8 ) & 255;
        widget->pick_color->blue  = ( widget->pick_id >> 16 ) & 255;
        widget->pick_color->alpha = 255;

	widget->children_head = NULL;
	widget->children_tail = NULL;
	widget->next_sibling  = NULL;

	widget->requested_size  = ei_size(0, 0);
        widget->screen_location = ei_rect_zero();
        widget->placer_params   = NULL;

        widget->wclass     = wclass;
        widget->parent     = parent;
        widget->user_data  = user_data;
        widget->destructor = destructor;

        // Liaison entre le widget, son parent et ses frères
        if ( parent != NULL )
        {
                if ( parent->children_head == NULL )
                {
                        parent->children_head = widget;
                        parent->children_tail = widget;
                }
                else
                {
                        parent->children_tail->next_sibling = widget;
                        parent->children_tail = widget;
                }
        }

        wclass->setdefaultsfunc(widget);

        return widget;
}




void ei_widget_destroy_worker(ei_widget_t widget);


void ei_widget_destroy(ei_widget_t widget)
{
	ei_app_invalidate_rect(&(widget->screen_location));

        // Delete it from parent
        if ( widget->parent != NULL )
        {
                ei_widget_t current = widget->parent->children_head;

                if ( current == widget )
                {
                        widget->parent->children_head = current->next_sibling;
                }
                else
                {
			while (current->next_sibling != widget)
			{
				current = current->next_sibling;
			}

			current->next_sibling = widget->next_sibling;
		}

		if ( current->next_sibling == NULL )
		{
			widget->parent->children_tail = current;
		}
        }

        // Run recursive worker
        ei_widget_destroy_worker(widget);
}




void ei_widget_destroy_worker(ei_widget_t widget)
{
        // Destroy Childs
        ei_widget_t current_widget = widget->children_head,
                    next_widget;

        while ( current_widget != NULL )
        {
                next_widget = current_widget->next_sibling;
                ei_widget_destroy_worker(current_widget);
                current_widget = next_widget;
        }

        // Call destructor
        if ( widget->destructor != NULL )
        {
                widget->destructor(widget);
        }

        // Remove from active
        if ( widget == ei_event_get_active_widget() )
        {
                ei_event_set_active_widget(NULL);
        }


        // Frees
        widget->wclass->releasefunc(widget);

        if ( widget->placer_params != NULL )
        {
                free(widget->placer_params);
        }

        free(widget->pick_color);
        free(widget);
}




bool ei_widget_is_displayed(ei_widget_t widget) { return widget->placer_params != NULL; }



// Fonction retournant le widget ayant la couleur "color" sur la surface de picking
ei_widget_t ei_widget_pick_worker(ei_widget_t widget, ei_color_t* color);

ei_widget_t ei_widget_pick(ei_point_t* where)
{
        ei_surface_t offscreen_picking = ei_get_offscreen_picking();

        uint8_t* first_pixel = hw_surface_get_buffer(offscreen_picking);
        uint8_t* pixel = first_pixel + 4 * ( where->y * hw_surface_get_size(offscreen_picking).width + where->x);

        
        int id_red, id_green, id_blue, id_alpha;
        hw_surface_get_channel_indices(offscreen_picking, &id_red, &id_green, &id_blue,  &id_alpha);
        
        // La couleur du pixel au point where selon les incides RGBA sur la surface de picking 
        ei_color_t pick_color = {
                pixel[id_red  ],
                pixel[id_green],
                pixel[id_blue ],
                255
        };

        // Recherche du widget correspond à la couleur
        return ei_widget_pick_worker(ei_app_root_widget(), &pick_color);
}

ei_widget_t ei_widget_pick_worker(ei_widget_t widget, ei_color_t* color)
{
        if ( ei_color_is_equal(widget->pick_color, color) )
        {
                return widget;
        }
        
        ei_widget_t current_widget = widget->children_head,
                    result;

        // Iteration sur les fils du widget, en comparant leurs couleurs à la couleur recherché
        while ( current_widget != NULL )
        {
                result = ei_widget_pick_worker(current_widget, color);       

                if ( result != NULL )
                {
                        return result;
                }

                current_widget = current_widget->next_sibling;
        }

        return NULL;
}
