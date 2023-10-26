#ifndef EI_DRAW_UTILS_H
#define EI_DRAW_UTILS_H

#include "hw_interface.h"
#include "ei_types.h"




/**
 * \brief	dessine un pixel avec la bonne couleur
 * @param       surface La surface sur laquelle il faut dessiner le pixel
 * @param       pixel   Le pixel a dessiner(à colorier)
 * @param       color   La couleur du pixel
 */
 void draw_pixel(int id_red, int id_green, int id_blue, int id_alpha, uint8_t* pixel, const ei_color_t* color);




/**
 * \brief	Fait l'intersection de deux rectangles
 * @param       rect_1 le rectangle 1
 * @param       rect_2 le rectangle 2
 * @return      Le rectangle resultant de l'intersection
 */
ei_rect_t ei_rect_intersect(const ei_rect_t* rect_1, const ei_rect_t* rect_2);




/**
 * \brief               Transforme une surface en rectangle dans les restrictions du clipper
 * @param  surface_size les dimensions de la surface à convertir
 * @param  clipper      le rectangle du clipper restrictif
 * @return              Le rectangle de l'intersection 
 */
ei_rect_t ei_surface_rect(const ei_size_t* surface_size, const ei_rect_t* clipper);




/**
 * \brief               Détermine le code de découpage d'un point par rapport à un clipper.
 * @param  clipper      Le rectangle de découpage restrictif.
 * @param  point        Le point à évaluer.
 * @return              Le code de découpage indiquant la position relative du point par rapport au clipper.
 */

char ei_clipping_code(const ei_rect_t* clipper, const ei_point_t* point);




/**
 * \brief               Ajoute une valeur à chaque composante d'une couleur.
 * @param  color        Un pointeur vers la couleur à modifier.
 * @param  value        La valeur à ajouter à chaque composante de la couleur.
 */
void add_at_color(ei_color_t* color, int value);




/**
 * \brief               Soustrait une valeur à chaque composante d'une couleur.
 * @param  color        Un pointeur vers la couleur à modifier.
 * @param  value        La valeur à soustraire à chaque composante de la couleur.
 */
void sub_at_color(ei_color_t* color, int value);




/**
 * \brief               Compare deux rectangle.
 * @param  rect_1       Un pointeur vers le 1er rectangle.
 * @param  rect_2       Un pointeur vers le 2nd rectangle.
 * @return              La fonction return true si les deux rectangles sont égaux ( taille et point topleft)
 */
bool ei_rect_is_equal(const ei_rect_t* rect_1, const ei_rect_t* rect_2);


/**
 * \brief               Compare deux points.
 * @param  rect_1       Un pointeur vers le 1er point.
 * @param  rect_2       Un pointeur vers le 2nd point.
 * @return              La fonction return true si les deux points sont au même coordonnées
 */
bool ei_point_is_equal(const ei_point_t* point_1, const ei_point_t* point_2);


/**
 * \brief               Compare deux couleurs.
 * @param  rect_1       Un pointeur vers la 1ere couleur.
 * @param  rect_2       Un pointeur vers la 2nde couleur.
 * @return              La fonction return true si les composantes RGBA des deux couleurs sont égales.
 */
bool ei_color_is_equal(const ei_color_t* color_1, const ei_color_t* color_2);

#endif
