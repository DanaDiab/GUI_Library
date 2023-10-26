#include "ei_draw_utils.h"

#include "ei_utils.h"

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

#define SHIFT(a, b) ((a) << b)




void draw_pixel(int id_red, int id_green, int id_blue, int id_alpha, uint8_t* pixel, const ei_color_t* color)
{
        pixel[id_red  ] = (color->alpha * color->red   + (255 - color->alpha) * pixel[id_red  ]) / 255;
        pixel[id_green] = (color->alpha * color->green + (255 - color->alpha) * pixel[id_green]) / 255;
        pixel[id_blue ] = (color->alpha * color->blue  + (255 - color->alpha) * pixel[id_blue ]) / 255;
        pixel[id_alpha] = color->alpha;
}




ei_rect_t ei_rect_intersect(const ei_rect_t* rect_1, const ei_rect_t* rect_2)
{
        if ( rect_1 == rect_2 ) { return *rect_1; }

        ei_point_t result_point = ei_point(
                MAX(rect_1->top_left.x, rect_2->top_left.x),
                MAX(rect_1->top_left.y, rect_2->top_left.y)
        );

        ei_size_t result_size = ei_size(
                rect_2->size.width  + rect_2->top_left.x - result_point.x,
                rect_2->size.height + rect_2->top_left.y - result_point.y
        );

        if ( result_point.x + result_size.width > rect_1->top_left.x + rect_1->size.width )
        {
                result_size.width = rect_1->top_left.x + rect_1->size.width - result_point.x;
        }
        if ( result_point.y + result_size.height > rect_1->top_left.y + rect_1->size.height )
        {
                result_size.height = rect_1->top_left.y + rect_1->size.height - result_point.y;
        }

        ei_rect_t result_rect = ei_rect(
                result_point, result_size
        );

        if ( result_rect.size.width <= 0 || result_rect.size.height <= 0 )
        {
                return ei_rect_zero();
        }

        return result_rect;
}




ei_rect_t ei_surface_rect(const ei_size_t* surface_size, const ei_rect_t* clipper)
{
        ei_rect_t surface_rect = ei_rect(ei_point_zero(), *surface_size);
        return ei_rect_intersect(&surface_rect, clipper != NULL ? clipper : &surface_rect);
}




char ei_clipping_code(const ei_rect_t* clipper, const ei_point_t* point)
{
        return SHIFT(point->y > clipper->top_left.y + clipper->size.height, 3) |
               SHIFT(point->y < clipper->top_left.y                       , 2) |
               SHIFT(point->x > clipper->top_left.x + clipper->size.width , 1) |
                    (point->x < clipper->top_left.x                          ) ;
}




void add_at_color(ei_color_t* color, int value)
{
        color->red   = ( 255 - color->red   < value ? 255 : color->red   + value );
        color->green = ( 255 - color->green < value ? 255 : color->green + value );
        color->blue  = ( 255 - color->blue  < value ? 255 : color->blue  + value );
}




void sub_at_color(ei_color_t* color, int value)
{
        color->red   = ( color->red   < value ? 0 : color->red   - value );
        color->green = ( color->green < value ? 0 : color->green - value );
        color->blue  = ( color->blue  < value ? 0 : color->blue  - value );
}




bool ei_rect_is_equal(const ei_rect_t* rect_1, const ei_rect_t* rect_2)
{
        return rect_1->top_left.x  == rect_2->top_left.x  &&
               rect_1->top_left.y  == rect_2->top_left.y  &&
               rect_1->size.width  == rect_2->size.width  &&
               rect_1->size.height == rect_2->size.height ;
}




bool ei_point_is_equal(const ei_point_t* point_1, const ei_point_t* point_2)
{
        return point_1->x == point_2->x && point_1->y == point_2->y;
}




bool ei_color_is_equal(const ei_color_t* color_1, const ei_color_t* color_2)
{
        return color_1->red   == color_2->red   &&
               color_1->green == color_2->green &&
               color_1->blue  == color_2->blue  &&
               color_1->alpha == color_2->alpha;
}
