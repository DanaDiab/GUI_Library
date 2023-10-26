#include "draw/ei_draw_utils.h"
#include "ei_widget_configure.h"
#include "ei_widget_attributes.h"
#include "ei_widgetclass_frame.h"
#include "ei_widgetclass_button.h"
#include "ei_widgetclass_toplevel.h"
#include "ei_application.h"
#include "ei_event.h"




void ei_frame_configure(ei_widget_t	        widget,
			ei_size_t*		requested_size,
			const ei_color_t*       color,
			int*			border_width,
			ei_relief_t*		relief,
			ei_string_t*		text,
			ei_font_t*		text_font,
			ei_color_t*		text_color,
			ei_anchor_t*		text_anchor,
			ei_surface_t*		img,
			ei_rect_ptr_t*		img_rect,
			ei_anchor_t*		img_anchor)
{
        ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;

        if ( requested_size != NULL )
        { 
                widget->requested_size = *requested_size; 
                frame->has_natural_size = false;
        }

        if ( color        != NULL ) { frame ->color        = *color       ; }
        if ( border_width != NULL ) { frame ->border_width = *border_width; }
        if ( relief       != NULL ) { frame ->relief       = *relief      ; }
        if ( text_font    != NULL ) { frame ->text_font    = *text_font   ; }
        if ( text_color   != NULL ) { frame ->text_color   = *text_color  ; }
        if ( text_anchor  != NULL ) { frame ->text_anchor  = *text_anchor ; }
        if ( img_anchor   != NULL ) { frame ->img_anchor   = *img_anchor  ; }
        
        // Text
        if ( text != NULL && *text != NULL )
        {
                if ( frame->text != NULL )
                {
                        // Afin de pouvoir changer le texte, s'il en existe déjà
                        free(frame->text);
                }

                frame->text = malloc(( strlen(*text) + 1 ) * sizeof(char));
                strcpy(frame->text, *text);
        }

        // Image
        if ( img_rect != NULL )
        {
                if ( frame->img_rect != NULL )
                {
                        // Afin de pouvoir changer l'image, s'il en existe une déjà
                        free(frame->img_rect);
                }

                if ( *img_rect != NULL )
                {
                        frame->img_rect = malloc(sizeof(ei_rect_t));
                        *(frame->img_rect) = **img_rect;
                }
                else
                {
                        frame->img_rect = NULL;
                }

        }

        if ( img != NULL ) 
        {
                if ( frame->img != NULL )
                {
                        // Afin de pouvoir changer l'image, s'il en existe une déjà
                        hw_surface_free(frame->img);
                }

                if ( *img != NULL )
                {
                        // Puisque img_rect peut être NULL
                        ei_rect_t surface_rect = hw_surface_get_rect(*img);

                        frame->img = hw_surface_create(*img, surface_rect.size, true);

                        hw_surface_lock(frame->img);
                        hw_surface_lock(*img);

                        ei_copy_surface(frame->img, &surface_rect, *img, &surface_rect , true);

                        hw_surface_unlock(*img);
                        hw_surface_unlock(frame->img);
                }
                else
                {
                        frame->img = NULL;
                }
        }
        
        // Content size
        if ( frame->has_natural_size )
        {
                ei_size_t content_size = ei_size_zero();

                // Initialisation du content_size
                if ( frame->text != NULL )
                {
                        hw_text_compute_size(frame->text, frame->text_font, &(content_size.width), &(content_size.height));
                }
                else if ( frame->img != NULL )
                {
                        content_size = ( frame->img_rect != NULL ) ? frame->img_rect->size : hw_surface_get_rect(frame->img).size;
                }

                // Prise en compte de la bordure
                widget->requested_size.width  = content_size.width  + 2 * frame->border_width;
                widget->requested_size.height = content_size.height + 2 * frame->border_width;
        }

        // Changement sur l'écran
        if ( ei_widget_is_displayed(widget) )
        {
                ei_impl_placer_run(widget);
        }
        ei_app_invalidate_rect(&(widget->screen_location));
}




 void ei_button_configure(ei_widget_t		widget,
			  ei_size_t*		requested_size,
			  const ei_color_t*	color,
			  int*			border_width,
			  int*			corner_radius,
			  ei_relief_t*		relief,
			  ei_string_t*		text,
			  ei_font_t*		text_font,
			  ei_color_t*		text_color,
			  ei_anchor_t*		text_anchor,
			  ei_surface_t*		img,
			  ei_rect_ptr_t*	img_rect,
			  ei_anchor_t*		img_anchor,
                          ei_callback_t*	callback,
                          ei_user_param_t*	user_param)
{
        ei_impl_button_t* button = (ei_impl_button_t*)widget;

        if ( requested_size != NULL )
        { 
                widget->requested_size = *requested_size; 
                button->has_natural_size = false;
        }

        if ( color         != NULL ) { button->color         = *color        ; }
        if ( border_width  != NULL ) { button->border_width  = *border_width ; }
        if ( relief        != NULL ) { button->relief        = *relief       ; }
        if ( text_font     != NULL ) { button->text_font     = *text_font    ; }
        if ( text_color    != NULL ) { button->text_color    = *text_color   ; }
        if ( text_anchor   != NULL ) { button->text_anchor   = *text_anchor  ; }
        if ( img_anchor    != NULL ) { button->img_anchor    = *img_anchor   ; }
        if ( corner_radius != NULL ) { button->corner_radius = *corner_radius; }
        if ( callback      != NULL ) { button->callback      = *callback     ; }
        if ( user_param    != NULL ) { button->user_param    = *user_param   ; }
        
        // Allocation d'espace pour le text et remplacement s'il en existait  déjà un
        if ( text != NULL && *text != NULL )
        {
                if ( button->text != NULL )
                {
                        free(button->text);
                }

                button->text = malloc(( strlen(*text) + 1 ) * sizeof(char));
                strcpy(button->text, *text);
        }

        // Allocation d'espace pour l'image et remplacement s'il en existait  déjà une
        if ( img_rect != NULL )
        {
                if ( button->img_rect != NULL )
                {
                        free(button->img_rect);
                }

                if ( *img_rect != NULL )
                {
                        button->img_rect = malloc(sizeof(ei_rect_t));
                        *(button->img_rect) = **img_rect;
                }
                else
                {
                        button->img_rect = NULL;
                }

        }

        if ( img != NULL ) 
        {
                if ( button->img != NULL )
                {
                        hw_surface_free(button->img);
                }

                if ( *img != NULL )
                {
                        // Creation des surfaces qui vont contenir l'image
                        ei_rect_t surface_rect = hw_surface_get_rect(*img);

                        button->img = hw_surface_create(*img, surface_rect.size, true);

                        hw_surface_lock(button->img);
                        hw_surface_lock(*img);

                        ei_copy_surface(button->img, &surface_rect, *img, &surface_rect , true);

                        hw_surface_unlock(*img);
                        hw_surface_unlock(button->img);
                }
                else
                {
                        button->img = NULL;
                }
        }
        

        // Content size
        if ( button->has_natural_size )
        {
                ei_size_t content_size = ei_size_zero();

                // Initialisation du content_size
                if ( button->text != NULL )
                {
                        hw_text_compute_size(button->text, button->text_font, &(content_size.width), &(content_size.height));
                }
                else if ( button->img != NULL )
                {
                        content_size = ( button->img_rect != NULL ) ? button->img_rect->size : hw_surface_get_rect(button->img).size;
                }

                // Prise en compte de la bordure
                widget->requested_size.width  = content_size.width  + 2 * button->border_width;
                widget->requested_size.height = content_size.height + 2 * button->border_width;
        }

        // Changement sur l'écran
        if ( ei_widget_is_displayed(widget) )
        {
                ei_impl_placer_run(widget);
        }
        ei_app_invalidate_rect(&(widget->screen_location));
}




void default_close_button_callback (ei_widget_t widget, struct ei_event_t* event, ei_user_param_t user_param);

void ei_toplevel_configure (ei_widget_t widget,
			    ei_size_t* requested_size,
			    ei_color_t*	color,
			    int* border_width,
			    ei_string_t* title,
			    bool* closable,
			    ei_axis_set_t* resizable,
			    ei_size_ptr_t* min_size)
{
        ei_toplevel_t toplevel = (ei_toplevel_t)widget;

        // Seulement pour le top_level, la taille de la bordure est incluse dans requested_size, tout en gérant l'ancienne bordure
        if ( requested_size != NULL )
        {
                if ( border_width != NULL )
                {
                        widget->requested_size.width = requested_size->width + 2 * *border_width;
                        widget->requested_size.height = requested_size->height + 2 * *border_width;
                }
                else
                {
                        widget->requested_size.width = requested_size->width + 2 * toplevel->border_width;
                        widget->requested_size.height = requested_size->height + 2 * toplevel->border_width;
                }
                // Inclusion de la taille du title bar dans requested_size, qui est fixé a une taille de 32
                widget->requested_size.height += 32;

        }
        else
        {
                if ( border_width != NULL )
                {
                        widget->requested_size.width += - 2 * toplevel->border_width + 2 * *border_width;
                        widget->requested_size.height += - 2 * toplevel->border_width + 2 * *border_width;
                }
        }
        
        if ( color != NULL ) { toplevel->color = *color ; }

        if ( closable != NULL )
        {
                if ( *closable && toplevel->close_button == NULL )
                {
                        // Mise en place d'un bouton qui permet la fermeture du toplevel
                        
                        toplevel->close_button = ei_widget_create("button", widget, NULL, NULL);
                        ei_button_configure(
                                toplevel->close_button,
                                &(ei_size_t){ 16, 16 },
                                &(const ei_color_t){ 200, 0, 0, 255 },
                                &(int){ 0 },
                                &(int){ 8 },
                                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
                                &(ei_callback_t){default_close_button_callback}, 
                                NULL
                        );
                        ei_place(
                                toplevel->close_button, 
                                NULL, 
                                NULL, NULL, NULL, NULL, 
                                &(float){ 0 }, &(float){ 0 }, 
                                NULL, NULL
                        );
                }
                else if ( !(*closable) && toplevel->close_button != NULL )
                {
                        ei_widget_destroy(toplevel->close_button);
                        toplevel->close_button = NULL;
                }

                toplevel->closable = *closable;
        }
        
        if ( resizable != NULL ) { toplevel->resizable = *resizable; }

        if ( border_width != NULL ) { toplevel->border_width = *border_width; }

        // Allocation d'espace pour le text et remplacement s'il en existait déjà un
        if ( title != NULL && *title != NULL )
        {
                if ( toplevel->title != NULL )
                {
                        free(toplevel->title);
                }

                toplevel->title = malloc(( strlen(*title) + 1 ) * sizeof(char));
                strcpy(toplevel->title, *title);
        }
        
        if ( min_size  != NULL )
        {
                if ( *min_size != NULL )
                {
                        toplevel->min_size = **min_size;
                }
                else
                {
                        toplevel->min_size = ei_size(160, 120);
                }
        }

        // Le toplevel ne peuvent pas être plus petit que sa min_size
        if ( widget->requested_size.width < toplevel->min_size.width )
        {
                widget->requested_size.width = toplevel->min_size.width;
        }
        if ( widget->requested_size.height < toplevel->min_size.height )
        {
                widget->requested_size.height = toplevel->min_size.height;
        }

        // Changement sur l'ecran
        if ( ei_widget_is_displayed(widget) )
        {
                ei_impl_placer_run(widget);
        }
        ei_app_invalidate_rect(&(widget->screen_location));
}
        



// -- Default Callbacks --

void default_close_button_callback(ei_widget_t widget, struct ei_event_t* event, ei_user_param_t user_param)
{
        ei_widget_destroy(widget->parent);
}