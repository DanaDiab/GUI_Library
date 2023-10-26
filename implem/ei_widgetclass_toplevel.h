#ifndef EI_WIDGETCLASS_TOPLEVEL_H
#define EI_WIDGETCLASS_TOPLEVEL_H

#include "hw_interface.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_draw.h"
#include "ei_widgetclass.h"
#include "ei_widgetclass_button.h"




// States
typedef enum ei_toplevel_state_t { DEFAULT, RESIZING, MOVING } ei_toplevel_state_t;

//Toplevel
typedef struct ei_impl_toplevel_t
{
        ei_impl_widget_t widget;

        ei_color_t color;

        ei_string_t title;

        bool closable;

        int border_width;

        ei_axis_set_t resizable;
        ei_size_t     min_size ;

        ei_widget_t close_button;

        ei_toplevel_state_t state;
        ei_point_t mouse_relative_position;
    
} ei_impl_toplevel_t;

typedef ei_impl_toplevel_t* ei_toplevel_t;



// Responsable de faire passer un widget de l'arrière au premier plan
void put_in_front_callback(ei_widget_t widget);

// Allocation
ei_widget_t allocfunc_toplevel();


// Release
void releasefunc_toplevel(ei_widget_t widget);


// Draw
void drawfunc_toplevel(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);


// Set defaults
void setdefaultsfunc_toplevel(ei_widget_t widget);


// Geometry notify
void geomnotifyfunc_toplevel(ei_widget_t widget);


// Handle event
bool handlefunc_toplevel(ei_widget_t widget, struct ei_event_t* event);

#endif
