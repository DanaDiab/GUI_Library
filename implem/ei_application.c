#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "ei_utils.h"
#include "ei_types.h"
#include "ei_draw.h"
#include "draw/ei_draw_utils.h"
#include "ei_application.h"
#include "ei_application_utils.h"
#include "ei_event.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_configure.h"
#include "ei_widgetclass_button.h"
#include "ei_widgetclass_frame.h"
#include "ei_widgetclass_toplevel.h"


#define MIN(a, b) (( a < b ) ? (a) : (b))
#define MAX(a, b) (( a > b ) ? (a) : (b))


static bool quit_request;

static ei_widget_t root_widget;

static ei_surface_t root_window_surface;

static ei_linked_rect_t* list_rect = NULL;


static ei_widgetclass_t frame_class = {
        "frame"               ,
        &      allocfunc_frame,
        &    releasefunc_frame,
        &       drawfunc_frame,
        &setdefaultsfunc_frame,
        & geomnotifyfunc_frame,
        &     handlefunc_frame,
        NULL
};
static ei_widgetclass_t button_class = {
        "button"               ,
        &      allocfunc_button,
        &    releasefunc_button,
        &       drawfunc_button,
        &setdefaultsfunc_button,
        & geomnotifyfunc_button,
        &     handlefunc_button,
        NULL
};
static ei_widgetclass_t toplevel_class = {
        "toplevel"               ,
        &      allocfunc_toplevel,
        &    releasefunc_toplevel,
        &       drawfunc_toplevel,
        &setdefaultsfunc_toplevel,
        & geomnotifyfunc_toplevel,
        &     handlefunc_toplevel,
        NULL
};




void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
        hw_init();

        // Enregistre les 3 classes
        ei_widgetclass_register(&    frame_class);
        ei_widgetclass_register(&   button_class);
        ei_widgetclass_register(&toplevel_class);

        //  Creation de root surface et root window
        root_window_surface = hw_create_window(main_window_size, fullscreen);
	hw_surface_set_origin(root_window_surface, ei_point_zero());

        ei_size_t root_window_size = hw_surface_get_size(root_window_surface);
        
        // Marquer l'intégralité de la zone de dessin invalide, afin qu'elle soit redessiner
        ei_app_invalidate_rect(&(ei_rect_t){ ei_point_zero(), root_window_size });

        // Initialize root widget as a frame
        root_widget = ei_widget_create("frame", NULL, NULL, NULL);
        ei_frame_set_requested_size(root_widget, root_window_size);

        root_widget->screen_location = ei_rect(ei_point_zero(), root_window_size);
        root_widget->content_rect = root_widget->screen_location;
        root_widget->pick_id = 0;

        // Picking surface
        ei_surface_t offscreen_picking = hw_surface_create(root_window_surface, root_window_size, true);
        ei_set_offscreen_picking(offscreen_picking);
}




void ei_app_free(void)
{
	ei_widget_destroy(root_widget);

	hw_surface_free(ei_get_offscreen_picking());

	ei_linked_rect_t* current = list_rect;
	ei_linked_rect_t* tmp = NULL;
	while (current)
	{
		tmp = current;
		current = current->next;
		free(tmp);
	}

        hw_quit();
}




void ei_app_run(void)
{
        // Initialisation
        struct ei_event_t event;
        ei_surface_t offscreen_picking = ei_get_offscreen_picking();
        
        ei_rect_t surface_rect = hw_surface_get_rect(root_window_surface);

        hw_surface_lock(offscreen_picking);
        
        // Tant que pas de demande d'arrêt du programme faire
        while ( quit_request == false )
        {        
                // Dessiner à l’écran les mises à jour nécessaires
                hw_surface_lock(root_window_surface);

                ei_linked_rect_t *cpy_list_rect = list_rect, 
                                 *current_rect  = list_rect,
                                 *next_rect;

                list_rect = NULL;

                while ( current_rect != NULL )
                {
                        // L'intersection entre le rectangle courant et surface_rect est utilisée 
                        // pour restreindre les opérations ultérieures à cette région : Clipping
                        
                        current_rect->rect = ei_rect_intersect(&(current_rect->rect), &surface_rect);

                        root_widget->wclass->drawfunc(root_widget, root_window_surface, offscreen_picking, &(current_rect->rect));

                        current_rect = current_rect->next;
                }

                hw_surface_unlock(root_window_surface);

                //Update sur l'écran
                hw_surface_update_rects(root_window_surface, cpy_list_rect);

                // Liberer la liste des rectangles à redessiner
                current_rect = cpy_list_rect;
                while ( current_rect != NULL )
                {
                        next_rect = current_rect->next;
                        free(current_rect);
                        current_rect = next_rect;
                }

                // Attendre un événement
                hw_event_wait_next(&event);
                
                // Analyser l’événement pour trouver le(s) traitant(s) associé(s)
                ei_eventtype_t type = event.type;

                // Appeler le(s) traitant(s) associé(s) et le widget concerné sur la picking surface
                ei_widget_t event_widget = ei_event_get_active_widget();

                if ( event_widget == NULL && ( event.type == ei_ev_mouse_buttondown || event.type == ei_ev_mouse_buttonup || event.type == ei_ev_mouse_move ) )
                {
                        event_widget = ei_widget_pick(&(event.param.mouse.where));
                }

                bool is_handle = false;
		if ( event_widget != NULL ) 
                {
			is_handle = event_widget->wclass->handlefunc(event_widget, &event);
		}

                // Si l'event n'a pas été traité
                if ( !is_handle )
                {
                        ei_default_handle_func_t default_handlefunc = ei_event_get_default_handle_func();
                        
                        if ( default_handlefunc != NULL )
                        {
                                default_handlefunc(&event);
                        }
                        
                }
        }

        hw_surface_unlock(offscreen_picking);
}




void ei_app_invalidate_rect(const ei_rect_t* rect)
{
        ei_rect_t new_rect = *rect;

        ei_linked_rect_t *current = list_rect,
                         *previous = NULL, 
                         *next;

        while ( current != NULL )
        {
                ei_rect_t intersection = ei_rect_intersect(&(current->rect), rect);

                // La zone est déjà concerné par un redessin
                if ( ei_rect_is_equal(&intersection, rect) )
                {
                        return;
                }
                
                // Si l'intersection est non vide, le code met à jour le rectangle actuel en calculant une nouvelle zone englobante
                else if ( intersection.size.width != 0 || intersection.size.height != 0 )
                {
                        new_rect.top_left.x = MIN( current->rect.top_left.x, rect->top_left.x );
                        new_rect.top_left.y = MIN( current->rect.top_left.y, rect->top_left.y );
                
                        new_rect.size.width  = MAX( current->rect.top_left.x + current->rect.size.width , rect->top_left.x + rect->size.width  ) - new_rect.top_left.x;
                        new_rect.size.height = MAX( current->rect.top_left.y + current->rect.size.height, rect->top_left.y + rect->size.height ) - new_rect.top_left.y;
                
                        next = current->next;

                        // Supprime le rectangle actuel de la liste des rectangles.
                        if ( previous != NULL )
                        {
                                previous->next = current->next;
                        }
                        else
                        {
                                list_rect = current->next;
                        }

                        free(current);
                        current = next;
                }
                else
                {
                        previous = current;
                        current = current->next;
                }
        }

        ei_linked_rect_t* new = malloc(sizeof(ei_linked_rect_t));
        assert(new != NULL);

        // Ajout du rect dans la liste quand il est pas déjà concerné par un redessin
        new->next = list_rect;
        new->rect = new_rect;
        list_rect = new;
}



 
void ei_app_quit_request(void)
{
        quit_request = true;
}




ei_widget_t ei_app_root_widget(void)
{
        return root_widget;
}




ei_surface_t ei_app_root_surface(void)
{
        return root_window_surface;
}

