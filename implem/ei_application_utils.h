#ifndef EI_APPLICATION_UTILS_H
#define EI_APPLICATION_UTILS_H

#include "ei_types.h"
#include "hw_interface.h"




/**
 * \brief	Returns the offscreen picking surface
 */
ei_surface_t ei_get_offscreen_picking();




/**
 * \brief	Sets the surface in the parameters as the offscreen picking surface
 */
void ei_set_offscreen_picking(ei_surface_t surface);

#endif
