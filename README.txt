Ce fichier contient la liste des fonctionnalités testées par application.

lines.c :
        - Principal test des primitives graphiques et du clipping analytique,
        - Test du débordement de texte hors du cadre de la surface, sans retour de l'autre coté de la fenêtre système,
        - Test de la fonction d'intersection de rectangle,
        - Dessin des polygones aux formes banales.

lines_shape.c :
        - Test de dessin des formes composant le bouton (et les frames).
          Le but étant de connaître et réduire le temps d'éxecution du dessin de ces formes.

frame.c et button.c : 
        - Création, exécution et sortie de l'application,
        - Création, configuration, placement des buttons et des frames,
        - Gestion minimale des évenements.

hello_world.c : 
        - Test d'un toplevel dans un toplevel,
        - Redimensionnement d'un toplevel,
        - Déplacement d'un toplevel.

two048.c :
        - Test de l'activation et du passage de l'application du second au premier plan,
          Ce passage au premier plan se fait lors du clic sur le widget, mais pas sur les fils (problématique de développement de la solution).

puzzle.c : 
        - Test d'affichage des images au sein des boutons,
        - Déplacement des enfants au sein de leur parent
        
minesweeper.c :
        - Test d'une application avec un grand nombre de widget.

Principales optimisations:
        - Les clippings analytiques,
        - Réduction du nombre de rectangle à dessiner par l'utilisation d'invalide_rect,
        - L'utilisation du logiciel "gprof" nous a permis d'optimiser certaines fonctions simples appelées un grand 
          nombre de fois (prenant un grand pourcentage cumulé du temps de traitement total)

exe.sh :
        Script bash permettant la simplification du lancement manuel des applications. 
        Le script prend en paramètre le nom de l'application à lancer.
        Il est requis d'avoir généré un MakeFile avant le lancement de ce script.
