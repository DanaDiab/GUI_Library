#include "ei_draw_image.h"
#include "ei_draw.h"
#include "ei_utils.h"
#include "ei_draw_utils.h"

void ei_draw_image(ei_surface_t surface, const ei_point_t* where, ei_surface_t image_surface, const ei_rect_t* image_rect, const ei_rect_t* clipper)
{
        // Initilisation
        ei_size_t surface_size = hw_surface_get_size(surface);
        ei_rect_t surface_rect = ei_surface_rect  (&surface_size, clipper);
        ei_rect_t correct_image_rect = *image_rect;
        ei_rect_t rel_image_rect = ei_rect(*where, image_rect->size);
        
        // Intersect
        surface_rect = ei_rect_intersect(&surface_rect, &rel_image_rect);
        correct_image_rect = ei_rect(ei_point_add(ei_point_sub(surface_rect.top_left, *where), correct_image_rect.top_left), surface_rect.size);

        if ( surface_rect.size.width == 0 || correct_image_rect.size.width == 0 ) { return; }

        // Copy
        hw_surface_lock(image_surface);
        ei_copy_surface(surface, &surface_rect, image_surface, &correct_image_rect, true);
        hw_surface_unlock(image_surface);
}