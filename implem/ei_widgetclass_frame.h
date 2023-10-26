#ifndef EI_WIDGETCLASS_FRAME_H
#define EI_WIDGETCLASS_FRAME_H

#include "hw_interface.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_widgetclass.h"




// Frame
typedef struct ei_impl_frame_t
{
        ei_impl_widget_t widget;

        ei_color_t  color ;
        ei_relief_t relief;
        
        int border_width;

        ei_string_t text       ;
        ei_font_t   text_font  ;
        ei_color_t  text_color ;
        ei_anchor_t text_anchor;

        ei_surface_t  img       ;
        ei_rect_ptr_t img_rect  ;
        ei_anchor_t   img_anchor;

        bool has_natural_size;  // Si pas de requested_size, alors le widget a sa taille naturel : la taille du contenu (texte, image ..)

} ei_impl_frame_t;

typedef ei_impl_frame_t* ei_frame_t;




// Allocation
ei_widget_t allocfunc_frame();


// Release
void releasefunc_frame(ei_widget_t widget);


// Draw
void drawfunc_frame(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);


// Set defaults
void setdefaultsfunc_frame(ei_widget_t widget);


// Geometry notify
void geomnotifyfunc_frame(ei_widget_t widget);


// Handle event
bool handlefunc_frame(ei_widget_t widget, struct ei_event_t* event);

#endif
