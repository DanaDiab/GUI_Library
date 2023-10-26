#ifndef EI_DRAW_FRAME_H
#define EI_DRAW_FRAME_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ei_types.h"
#include "hw_interface.h"


/* Fonction responsable du dessin d'une frame, ayant un relief et un arc*/
void draw_frame(ei_surface_t* surface, ei_rect_t location, ei_color_t color, int border_width, int corner_radius, ei_relief_t relief, ei_rect_t* clipper);

#endif
