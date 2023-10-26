#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"
#include "ei_draw_frame.h"




/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	hw_init();

	ei_size_t    win_size    = ei_size(800, 600);
	ei_surface_t main_window = hw_create_window(win_size, false);
        
	ei_color_t white = { 0xff, 0xff, 0xff, 0xff };
	ei_color_t black = { 0x00, 0x00, 0x00, 0xff };

	ei_rect_t clipper = ei_rect(ei_point(200, 150), ei_size(400, 300));
		
	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);

	ei_fill(main_window, &((ei_color_t){ 200, 200, 200, 255 }), NULL);
	ei_fill(main_window, &white, &clipper);

    	int nb_loop = 1000;
    	double end, start;

   	start = hw_now();

	for ( int i = 0 ; i < nb_loop ; i++ )
	{
		draw_frame(main_window, ei_rect(ei_point(230, 180), ei_size(340, 240)), (ei_color_t){ 100, 100, 127, 255 }, 0, 5, ei_relief_raised, &clipper);

		for ( int x = 0 ; x < 10 ; x++ )
		{
			for ( int y = 0 ; y < 10 ; y++ )
			{
				draw_frame(main_window, ei_rect(ei_point(250 + 20 * x, 200 + 20 * y), ei_size(20, 20)), (ei_color_t){ 100, 100, 127, 255 }, 5, 20, ei_relief_raised, &clipper);
			}
		}
	}

    	end = hw_now();

    	printf("Execution time for draw_frame: %f s.\n", (end - start) / (double)nb_loop);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);
	
	/* Wait for a character on command line. */
    	ei_event_t event;

	event.type = ei_ev_none;
	while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
	hw_event_wait_next(&event);

	hw_quit();
	return (EXIT_SUCCESS);
}
