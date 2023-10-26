#include "ei_application_utils.h"

static ei_surface_t offscreen_picking;




ei_surface_t ei_get_offscreen_picking()
{
        return offscreen_picking;
}




void ei_set_offscreen_picking(ei_surface_t surface)
{
        offscreen_picking = surface;
}
