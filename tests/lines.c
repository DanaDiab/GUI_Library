#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"
#include "../implem/draw/ei_draw_utils.h"



/* test_line --
 *
 *	Draws a simple line in the canonical octant, that is, x1>x0 and y1>y0, with
 *	dx > dy. This can be used to test a first implementation of Bresenham
 *	algorithm, for instance.
 */
void test_line(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t color  = { 0, 0, 0, 255 };
	ei_point_t pts[2] = { {200, 200}, {600, 400} };

	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color, clipper);
}



/* test_octogone --
 *
 *	Draws an octogone in the middle of the screen. This is meant to test the
 *	algorithm that draws a polyline in each of the possible octants, that is,
 *	in each quadrant with dx>dy (canonical) and dy>dx (steep).
 */
void test_octogone(ei_surface_t surface, ei_rect_t* clipper)
{
    ei_color_t color_poyline  = { 255,   0, 0, 255 };
    ei_color_t color_poyligon = {   0, 255, 0, 127 };

	ei_point_t pts[9];
	int i, xdiff, ydiff;

	/* Initialisation */
	pts[0].x = 400; pts[0].y = 90;

	/* Draw the octogone */
	for(i = 1; i <= 8; i++)
        {
		 /*	Add or remove 70/140 pixels for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 2 or 1, according to which coordinate grows faster
			The third term is simply the amount of pixels to skip */
		xdiff = (int)(pow(-1, (i + 1) / 4) * pow(2, (i / 2) % 2 == 0) * 70);
		ydiff = (int)(pow(-1, (i - 1) / 4) * pow(2, (i / 2) % 2) * 70);

		pts[i].x = pts[i-1].x + xdiff;
		pts[i].y = pts[i-1].y + ydiff;
	}

	/* Draw the shape with polylines */
	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyline, clipper);
	
	/* Draw the shape with a polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyligon, clipper);
}



/* test_square --
 *
 *	Draws a square in the middle of the screen. This is meant to test the
 *	algorithm for the special cases of horizontal and vertical lines, where
 *	dx or dy are zero
 */
void test_square(ei_surface_t surface, ei_rect_t* clipper)
{
    ei_color_t color_poyline  = { 255, 0, 0, 255 };
    ei_color_t color_poyligon = { 255, 0, 0, 127 };

	ei_point_t pts[5];
	int i, xdiff, ydiff;

	/* Initialisation */
	pts[0].x = 300; pts[0].y = 400;

	/* Draw the square */
	for(i = 1; i <= 4; i++) 
        {
		/*	Add or remove 200 pixels or 0 for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 0 or 1, according to which coordinate grows
			The third term is simply the side of the square */
		xdiff = (int)(pow(-1, i / 2) * (i % 2) * 200);
		ydiff = (int)(pow(-1, i / 2) * (i % 2 == 0) * 200);

		pts[i].x = pts[i-1].x + xdiff;
		pts[i].y = pts[i-1].y + ydiff;
	}

	/* Draw the shape with polylines */
	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyline, clipper);

	/* Draw the shape with polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyligon, clipper);
}



/* test_dot --
 *
 *	Draws a dot in the middle of the screen. This is meant to test the special 
 *	case when dx = dy = 0
 */
void test_dot(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t color = { 0, 0, 255, 255 };
	ei_point_t pts[2] = { {400, 300}, {400, 300} };

	/* Draw the shape with polylines */
	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color, clipper);

	/* Draw the shape with polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), color, clipper);

}
/* test mundane polygon --
 *
 *	Draws a mundane polygon that exceeds the border of the main window, it must not cause any error	
 */
void test_mundane_polygon(ei_surface_t surface, ei_rect_t* clipper)
{
    ei_color_t color_poyline  = { 0, 0, 0, 255 };
    ei_color_t color_poyligon = { 0, 0, 0, 127 };

	ei_point_t pts[11] = {
        	 { 360, 380 },
        	 { 450, 340 },
        	 { 640, 350 },
        	 { 550, 300 },
        	 { 900, 260 },
        	 { 550, 200 },
        	 { 300, 100 },
        	 { 240, 140 },
        	 { 240, 200 },
        	 { 150, 240 },
			 { 280, 380 }
        };

	/* Draw the shape with polylines */
	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyline, clipper);
	
	/* Draw the shape with a polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), color_poyligon, clipper);
}




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

	/* Tests IMAG */
	test_line           (main_window, &clipper);
	test_octogone       (main_window, &clipper);
	test_square         (main_window, &clipper);
	test_dot            (main_window, &clipper);
	test_mundane_polygon(main_window, &clipper);

	ei_point_t where = { 210, 160 };
	ei_draw_text(main_window, &where, "Hello World!", NULL, black, &clipper);
	
	/* Tests ei_fill */	

	/* Inside window without clipper */
	ei_fill(main_window, &((ei_color_t){ 0, 0, 255, 125 }), NULL);

    	/* Inside window with clipper (inside) */
	clipper = ei_rect(ei_point(50, 50), ei_size(50, 50));
	ei_fill(main_window, &((ei_color_t){ 0, 0, 0, 255 }), &clipper);

    	/* Outside window with clipper (outside bottom-right) */
	clipper = ei_rect(ei_point(700, 500), ei_size(200, 200));
	ei_fill(main_window, &((ei_color_t){ 255, 255, 255, 127 }), &clipper);

	/* Outside window with clipper (outside top-left) */
	clipper = ei_rect(ei_point(-100, -100), ei_size(200, 200));
	ei_fill(main_window, &((ei_color_t){ 255, 255, 255, 127 }), &clipper);

	/* Tests ei_draw_text */

	/* Outside window without clipper (outside left) */
	ei_draw_text(main_window, &((ei_point_t){ -20, 300 }), "I am outside left", NULL, black, NULL);

	/* Outside window without clipper (outside right) */
	ei_draw_text(main_window, &((ei_point_t){ 700, 100 }), "I am outside right", NULL, black, NULL);

    	/* Outside window with clipper (outside left) */
    	clipper = ei_rect(ei_point(-10, 390), ei_size(100, 70));
	ei_draw_text(main_window, &((ei_point_t){ -20, 400 }), "I am outside left 2", NULL, black, &clipper);
	
	/* Tests ei_copy_surface */

	ei_surface_t offscreen = hw_surface_create(main_window, win_size, true);
	hw_surface_lock(offscreen);

	/* Draw a text in an offscreen surface, fills it with purple and copies it to the the main window(without a clipper) */

	ei_fill(offscreen, &((ei_color_t){ 255, 0, 255, 100 }), NULL);
	ei_draw_text(offscreen, &((ei_point_t){2,2}), "I come from another world !", NULL, ((ei_color_t){ 0, 255, 255, 200 }), NULL);

	ei_rect_t dst_rect = ei_rect(ei_point(400, 300), ei_size(50, 50));
	ei_rect_t src_rect = ei_rect(ei_point(2, 2), ei_size(50, 50));

	ei_copy_surface (main_window , &dst_rect , offscreen, &src_rect , true);

	/* Test de ei_intersect, la surface intersectée est en bleu */
	const ei_rect_t rect1 = ei_rect(ei_point(100,100),ei_size(300,300));
	const ei_rect_t rect2 = ei_rect(ei_point(150,250),ei_size(300,300));
	ei_rect_t inter = ei_rect_intersect(&rect1,&rect2);
	const ei_color_t color = {0,255,0,255};
	const ei_color_t color_inter = {0,0,255,255};
	ei_fill(main_window,&color,&rect1);
	ei_fill(main_window,&color,&rect2);
	ei_fill(main_window,&color_inter,&inter);

	/* -------------------------------------------- */
	
    	hw_surface_unlock(offscreen);
    	hw_surface_free(offscreen);

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
