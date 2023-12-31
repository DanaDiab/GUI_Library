/**
 *  @file	ei_event.h
 *  @brief	Allows the binding and unbinding of callbacks to events.
 *
 *  \author 
 *  Created by François Bérard on 30.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */
#ifndef EI_EVENT_H
#define EI_EVENT_H

#include "ei_types.h"
#include "ei_widget.h"

/**
 * @brief	The types of events.
 */
typedef enum {
	ei_ev_none		= 0,	///< No event, used to initialize the structure.
	ei_ev_app,			///< An application event, created by \ref hw_event_post_app.
	ei_ev_close,			///< The user requested to close the application's window.
	ei_ev_exposed,			///< The application's window has been exposed and should be redrawn.
	ei_ev_keydown,			///< A keyboard key has been pressed.
	ei_ev_keyup,			///< A keyboard key has been released.

	/* event types at or above "ei_ev_mouse_buttondown" are mouse events that require picking. */
	ei_ev_mouse_buttondown,		///< A mouse button has been pressed.
	ei_ev_mouse_buttonup,		///< A mouse button has been released.
	ei_ev_mouse_move,		///< The mouse has moved.

	ei_ev_last			///< Last event type, its value is the number of event types.
} ei_eventtype_t;

/**
 * @brief	The modifier keys (shift, alt, etc.)
 */
typedef enum {
	ei_mod_shift_left	= 0,	///< The "shift" key at the left of the space bar.
	ei_mod_alt_left,		///< The "alternate" key at the left of the space bar.
	ei_mod_meta_left,		///< The "meta" (command) key at the left of the space bar.
	ei_mod_ctrl_left,		///< The "control" key at the left of the space bar.
	ei_mod_shift_right,		///< The "shift" key at the right of the space bar.
	ei_mod_alt_right,		///< The "alternate" key at the right of the space bar.
	ei_mod_meta_right,		///< The "meta" (command) key at the right of the space bar.
	ei_mod_ctrl_right		///< The "control" key at the right of the space bar.

} ei_modifier_key_t;

/**
 * @brief	A bitfield indicating which of the modifier keys are currently pressed.
 */
typedef uint32_t			ei_modifier_mask_t;

/**
 * @brief	The various mouse buttons.
 */
typedef enum {
	ei_mouse_button_left,		///< The left mouse button.
	ei_mouse_button_middle,		///< The middle mouse button.
	ei_mouse_button_right		///< The right mouse button.
} ei_mouse_button_t;

/**
 * @brief	The event parameter for keyboard-related event types.
 */
typedef struct {
	SDL_Keycode 			key_code;	///< The keyboard key code (see \ref SDL_keycode.h)
	ei_modifier_mask_t		modifier_mask;	///< The state of the modifier keys at the time of the event.
} ei_key_event_t;

/**
 * @brief	The event parameter for mouse-related event types.
 */
typedef struct {
	ei_point_t			where;		///< Where the mouse pointer was at the time of the event.
	ei_mouse_button_t 		button;		///< The mouse button that was pressed or released. Only valid for \ref ei_ev_mouse_buttondown or \ref ei_ev_mouse_buttonup event types.
	ei_modifier_mask_t		modifier_mask;	///< The state of the modifier keys at the time of the event.
} ei_mouse_event_t;

/**
 * @brief	The event parameter for application defined event types.
 */
typedef struct {
	void*				user_param;
} ei_app_event_t;

/**
 * @brief	Describes an event.
 */
typedef struct ei_event_t {
	ei_eventtype_t	type;				///< The type of the event.
	union {
		ei_key_event_t		key;		///< Event parameters for keyboard-related events (see \ref ei_key_event_t).
		ei_mouse_event_t	mouse;		///< Event parameters for mouse-related	events (see \ref ei_mouse_event_t).
		ei_app_event_t		application;	///< Event parameters for application-related events (see \ref ei_app_event_t).
	} param;
} ei_event_t;

/**
 * @brief	Tests is a modifier key was pressed at the time of an event.
 *
 * @param	modifier_mask	The modifier_mask field of a \ref ei_key_event_t or a \ref ei_mouse_event_t.
 * @param	modifier	The modifier key.
 *
 * @return			true if this modifier key was pressed, false otherwise.
 */
static
inline bool			ei_has_modifier	(ei_modifier_mask_t	modifier_mask,
						 ei_modifier_key_t	modifier)
						{ return (bool)(modifier_mask & (1 << modifier)); }

/**
 * Sets the widget which is currently being manipulated by the user.
 *
 * @param	widget		The widget to declare as active, or NULL to declare
 *				that a widget is no more being manipulated.
 */
void				ei_event_set_active_widget(ei_widget_t widget);

/**
 * Returns the widget currently being manipulated by the user.
 *
 * @return			The widget currently being manipulated, or NULL.
 */
ei_widget_t			ei_event_get_active_widget(void);



/**
 * @brief	A function that is called in response to an event that has not been processed
 *		by any widget.
 *
 * @param	event		The event containing all its parameters (type, etc.)
 *
 * @return			true if the function handled the event,
 *				false otherwise, in this case the event is dismissed.
 */
typedef bool			(*ei_default_handle_func_t)(ei_event_t* event);

/**
 * Sets the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @param	func		The event handling function.
 */
void				ei_event_set_default_handle_func(ei_default_handle_func_t func);

/**
 * Returns the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @return			The address of the event handling function.
 */
ei_default_handle_func_t	ei_event_get_default_handle_func(void);



#endif
