#include "ei_implementation.h"
#include "ei_placer.h"




void ei_place(ei_widget_t       widget,
	      ei_anchor_t*	anchor,
              int*		x,
              int*		y,
              int*		width,
              int*		height,
              float*		rel_x,
              float*		rel_y,
              float*		rel_width,
              float*		rel_height) 
{ 
        // Le cas où le widget n'est pas placé sur l'écran
        if ( widget->placer_params == NULL )
        { 
                widget->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
		widget->placer_params->anchor     = ei_anc_northwest;
                widget->placer_params->x          =  0;
                widget->placer_params->y          =  0;
                widget->placer_params->width      = -1;
                widget->placer_params->height     = -1;
                widget->placer_params->rel_x      = -1;
                widget->placer_params->rel_y      = -1;
                widget->placer_params->rel_width  = -1;
                widget->placer_params->rel_height = -1;
        }

        if ( anchor     != NULL ) { widget->placer_params->anchor     = *anchor    ; }
        if ( x          != NULL ) { widget->placer_params->x          = *x         ; }
        if ( y          != NULL ) { widget->placer_params->y          = *y         ; }
        if ( width      != NULL ) { widget->placer_params->width      = *width     ; }
        if ( height     != NULL ) { widget->placer_params->height     = *height    ; }
        if ( rel_x      != NULL ) { widget->placer_params->rel_x      = *rel_x     ; }
        if ( rel_y      != NULL ) { widget->placer_params->rel_y      = *rel_y     ; }
        if ( rel_width  != NULL ) { widget->placer_params->rel_width  = *rel_width ; }
        if ( rel_height != NULL ) { widget->placer_params->rel_height = *rel_height; }

        // Re-calcul des emplacements des enfants
        ei_impl_placer_run(widget);
}




void ei_placer_forget(ei_widget_t widget)
{
        if ( widget->placer_params != NULL )
        {
                free(widget->placer_params);
        }

        // Déclarer le widget comme non placé
        widget->placer_params = NULL;
}