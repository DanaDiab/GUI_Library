#ifndef EI_DRAW_TYPES_H
#define EI_DRAW_TYPES_H

#include "ei_types.h"




/*
 * Sorted linked list sides
 */
typedef struct ei_linked_side_cell_t
{
        int y_max;

        double x_y_min;
        double gradiant;

        struct ei_linked_side_cell_t* previous;
        struct ei_linked_side_cell_t* next;

} ei_linked_side_cell_t;

typedef ei_linked_side_cell_t* ei_side_list_t;




/**
 * \brief                       Ajout d'un élément à la liste.
 * @param  ptr_side_list        Un pointeur vers la liste dan laquelle on ajoute.
 * @param  side                 Un pointeur vers un side qu'il faut ajouter.
 */
void ei_side_list_append(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side);




/**
 * \brief                       Le chainage de deux listes.
 * @param  ptr_side_list        Un pointeur vers la liste initiale.
 * @param  added_list           Une liste qu'il faut chainé à la liste initiale.
 */
void ei_side_list_extend(ei_side_list_t* ptr_side_list, ei_side_list_t added_list);


/**
 * \brief                       Suppression d'un élément de la liste.
 * @param  ptr_side_list        Un pointeur vers la liste.
 * @param  side                 Un pointeur vers un side qu'il faut supprimer.
 */
void ei_side_list_pop(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side);




/**
 * \brief                       Ajout d'un élément à la liste triée.
 * @param  ptr_side_list        Un pointeur vers la liste qu'on trie.
 * @param  side                 Un pointeur vers un side qu'il faut ré-inserer au bon endroit.
 */
void ei_side_list_resort(ei_side_list_t* ptr_side_list, ei_linked_side_cell_t* side);




/*
 * Circular Linked list points
 */
typedef struct ei_circular_linked_cell_point_t 
{
	double x;
        int    y;

        bool head;

	struct ei_circular_linked_cell_point_t* next;
        struct ei_circular_linked_cell_point_t* previous;

} ei_circular_linked_cell_point_t;




/**
 * \brief             Ajout dans une liste chainée circulaire.
 * @param  ptr_list   Un pointeur vers la liste dans laquelle on fait l'ajout.
 * @param  x          Coordonnée en abscisse du point à ajouter.
 * @param  y          Coordonnée en ordonné du point à ajouter
 */
void circular_linked_point_append_left(ei_circular_linked_cell_point_t** ptr_list, double x, int y);




/**
 * \brief             Libère une liste chainée circulaire.
 * @param  ptr_list   Un pointeur vers la liste à libérer.
 */
void circular_linked_point_free       (ei_circular_linked_cell_point_t** ptr_list);




/*
 * Sides table
 */
typedef ei_side_list_t* ei_side_table_t;




/**
 * \brief               Ajoute une cellule à une table de côtés.
 * @param  side_table   La table de côtés à laquelle ajouter la cellule.
 * @param  height       La hauteur.
 * @param  begin        Le point de départ de la cellule.
 * @param  end          Le point de fin de la cellule.
 * @return              La valeur de la coordonnée Y minimale de la cellule ajoutée.
 */
int ei_side_table_append(ei_side_table_t side_table, int height, ei_point_t* begin, ei_point_t* end);

#endif
