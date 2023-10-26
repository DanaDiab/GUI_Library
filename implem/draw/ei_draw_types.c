#include "ei_draw_types.h"
#include "ei_utils.h"

#include <assert.h>

void ei_side_list_append(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side)
{
        // Liste vide ou ajout en tête
        if ( *ptr_side_list == NULL || side->x_y_min < (*ptr_side_list)->x_y_min )
        {
                side->next = *ptr_side_list;
                *ptr_side_list = side;
        }
        // Ajout en queue
        else
        {
                ei_linked_side_cell_t* current = *ptr_side_list;
                while ( current->next != NULL && current->next->x_y_min < side->x_y_min )
                {
                        current = current->next;
                }
 
                side->next = current->next;
                current->next = side;
        }
}




void ei_side_list_extend(ei_side_list_t* ptr_side_list, ei_side_list_t added_list)
{
        ei_linked_side_cell_t *previous = NULL,
                              *current  = *ptr_side_list,
                              *next;
 
        if ( added_list != NULL )
        {
                ei_linked_side_cell_t *test = added_list;
 
                while ( test )
                {
                        test = test->next;
                }
        }
 
        while ( added_list != NULL )
        {
                while ( current != NULL && current->x_y_min < added_list->x_y_min )
                {
                        previous = current;
                        current  = current->next;
                }
 
                next = added_list->next;
 
                if ( previous != NULL )
                {
                        previous->next = added_list;
                }
                else
                {
                        *ptr_side_list = added_list;
                }

                added_list->previous = previous;
                added_list->next     = current ;

                if ( current != NULL )
                {
                        current->previous = added_list;
                        current = current->next;
                }

                current = added_list;
                added_list = next;
        }
}




void ei_side_list_pop(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side)
{
        if ( *ptr_side_list == side )
        {
                *ptr_side_list = side->next;
        }

        if ( side->previous != NULL )
        {
                side->previous->next = side->next;
        }
                   
        if ( side->next != NULL )
        {
                side->next->previous = side->previous;
        }

        free(side);
}




void ei_side_list_resort(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side)
{
        // Sort x
        ei_linked_side_cell_t *previous = side->previous;

        while ( previous != NULL && previous->x_y_min >= side->x_y_min )
        {
                previous = previous->previous;
        }

        if ( previous != side->previous )
        {
                if ( side->previous != NULL )
                {
                        side->previous->next = side->next;
                }
                    
                if ( side->next != NULL )
                {
                        side->next->previous = side->previous;
                }

                side->previous = previous;
                side->next     = NULL    ;

                // Append left
                if ( previous == NULL )
                {
                        side->next = *ptr_side_list;
                        side->next->previous = side;

                        *ptr_side_list = side;
                }
                else
                {
                        if ( previous->next != NULL )
                        {
                                side->next = previous->next;
                                side->next->previous = side;
                        }

                        previous->next = side;
                }
        }
}




void circular_linked_point_append_left(ei_circular_linked_cell_point_t** ptr_list, double x, int y)
        {
        ei_circular_linked_cell_point_t* cell = malloc(sizeof(ei_circular_linked_cell_point_t));
        assert( cell != NULL );

        cell->head = true;
        cell->x = x;
        cell->y = y;

        if ( *ptr_list == NULL )
        {
                cell->next     = cell;
                cell->previous = cell;

                *ptr_list = cell;

                return;
        }

        cell->next = *ptr_list;
        cell->previous = (*ptr_list)->previous;

        cell->previous->next = cell;
        cell->next->previous = cell;

        (*ptr_list)->head = false;
        *ptr_list = cell;
}




void circular_linked_point_free(ei_circular_linked_cell_point_t** ptr_list)
{
        ei_circular_linked_cell_point_t *current_cell = (*ptr_list)->next,
                                        *   next_cell;

        while ( !(current_cell->head) )
        {
                next_cell = current_cell->next;
                free(current_cell);

                current_cell = next_cell;
        }

        free(*ptr_list);

        *ptr_list = NULL;
}




 /*
 * Sides table
 */
typedef ei_side_list_t* ei_side_table_t;

int ei_side_table_append(ei_side_table_t side_table, int height, ei_point_t* begin, ei_point_t* end)
{
        ei_linked_side_cell_t* side = malloc(sizeof(ei_linked_side_cell_t));
        ei_point_t *ptr_max = begin, *ptr_min = end;

        if ( begin->y < end->y )
        {
                ptr_max = end  ;
                ptr_min = begin;
        }

        side->y_max   = ptr_max->y;

        side->x_y_min  = ptr_min->x;
        side->gradiant = ((double)(ptr_max->x - ptr_min->x)) / ((double)(ptr_max->y - ptr_min->y));

	if ( 0 <= ptr_min->y && ptr_min->y < height )
	{
		ei_side_list_append(&(side_table[ptr_min->y]), side);
	}

        return ptr_min->y;
}
