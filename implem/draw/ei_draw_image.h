#ifndef EI_DRAW_IMAGE_H
#define EI_DRAW_IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ei_types.h"
#include "hw_interface.h"




/**
 * \brief                Dessine une image sur une surface en respectant les paramètres de position et de découpage.
 * @param  surface       La surface sur laquelle dessiner l'image.
 * @param  where         Les coordonnées de position de l'image sur la surface.
 * @param  image_surface La surface contenant l'image à dessiner.
 * @param  image_rect    Le rectangle délimitant la portion de l'image à dessiner.
 * @param  clipper       Le rectangle de découpage restrictif.
 */
void ei_draw_image(ei_surface_t surface, const ei_point_t* where, ei_surface_t image_surface, const ei_rect_t* image_rect, const ei_rect_t* clipper);

#endif
