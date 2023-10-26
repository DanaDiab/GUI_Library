#include "ei_implementation.h"
#include "ei_widgetclass.h"



// Linked list of widget classes
static ei_widgetclass_t* widgetclass_list = NULL;




void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
        // Ajout de la nouvelle classe en tête
        widgetclass->next = widgetclass_list;
        widgetclass_list = widgetclass;
}




ei_widgetclass_t* ei_widgetclass_from_name(ei_const_string_t name)
{
        ei_widgetclass_t* current = widgetclass_list;

        while( current != NULL && strcmp(current->name, name) != 0 )
        {
                current = current->next;
        }

        return current;
}




size_t ei_widget_struct_size()
{
        return sizeof(ei_impl_widget_t);
}




