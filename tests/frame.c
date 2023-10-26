#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget_configure.h"


/*
 * ei_main --
 *
 * Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_widget_t frame, button;

	/* Create the application and change the color of the background. */
	ei_app_create((ei_size_t){600, 600}, false);
	ei_frame_set_bg_color(ei_app_root_widget(), (ei_color_t){0x52, 0x7f, 0xb4, 0xff});

	/* Create, configure and place the frame on screen. */
	frame = ei_widget_create("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure(
                frame,
                &(ei_size_t){ 300,200 },
                &(ei_color_t){ 127, 127, 127, 255 },
		&(int){ 6 },
                &(ei_relief_t){ ei_relief_raised }, 
                NULL, NULL, NULL, NULL, NULL, NULL, NULL
    	);
	ei_place_xy(frame, 150, 200);


	button = ei_widget_create("button", ei_app_root_widget(), NULL, NULL);
	ei_button_configure(
                button, 
                &(ei_size_t){ 200, 50 },
                &(ei_color_t){ 200, 200, 200, 255 },
				&(int){  6 },
                &(int){ 10 }, 
                &(ei_relief_t){ ei_relief_sunken }, 
                &(char*){ "Clique moi !" }, 
                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	ei_place_xy(button, 200, 300);

	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
