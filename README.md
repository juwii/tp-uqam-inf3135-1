# TP1: Dessiner sur un canevas ASCII"


## Description

Le projet consiste en l'affichage d'un canvas. Le canvas peut être affiché à partir d'un 
fichier ou alors avec l'option `-n` avec le nombre de lignes et de colonnes.
Différentes formes géométriques peuvent être affichées sur le canvas :

* Une ligne horizontale à un indice choisi avec l'option `-h`.

* Une ligne verticale à un indice choisi avec l'option `-v`.

* Un rectangle avec l'option `-r` en saissant les coordonnées d'un point puis la longueur
et largeur souhaitées.

* Un segment avec l'option `-l` en saisissant les coordonnées des deux points à relier.

* Un cercle avec l'option `-c` en saisissant les coordonnées du centre et son rayon.

Ces formes géométriques sont dessinées avec le chiffre 7 par défaut. 
L'option `-p` permet de modifier le caractère avec lequel est dessiné le canvas.
Le caractère peut être compris entre 0 et 7.
L'option `-k` permet d'afficher le canvas en couleur avec son équivalent en code ASCII.

Ce projet est accompli dans le cadre du cours "INF3135" à l'UQAM enseigné par M. Serge Dogny.
Il s'agit du TP1.
Lien vers l'énoncé du TP : [sujet du travail](sujet.md).

## Auteur

Juliette Létondot (LETJ74550003)

## Fonctionnement

Pour faire fonctionner le projet, il faut tout d'abord le compiler. Pour le compiler, 
il faut taper la commande `make` dans un terminal en se plaçant dans le dossier où est
enregistré le projet.
Pour exécuter le programme, taper `./canvascii`. Ajouter ensuite les options souhaitées.
Les formats d'entrée sont des options avec ou sans argument : `./canvascii -n 1,1 -h 2`
ou alors un fichier en entrée : `./canvascii -s < examples/empty5x8.canvas`.
En sortie, on obtient un canvas affiché dans le terminal.
La taille maximale d'un canvas est de 40x80.

Exemple :
```sh
$ make
gcc -o canvascii -Wall -Wextra -std=c11 canvascii.c 
$ ./canvascii -n 10,10 -h 2 -p 3 -c 5,5,2
..........
..........
7777777777
....333...
...3...3..
...3...3..
...3...3..
....333...
..........
..........
```
Le programme gère les bugs tels que :
* Une mauvaise valeur de pixel.
* Si les dimensions du canvas lu ou saisi sont supérieures à la taille maximale.
* Si les lignes du canvas lu ne sont pas toutes de même taille.
* Si l'utilisateur saisit une option non connue.
* Si l'on oublie une valeur à une option qui en nécessite une.
* Si l'on fournit une valeur mal formatée ou non valide. 

Pour plus d'informations, exécuter la commande `./canvascii` pour afficher le 
manuel d'utilisation.

## Tests

Pour lancer la série de tests, compiler le programme : `make` puis taper la commande 
`make test`. 
Voici le résultat de la série de tests :

 ✓ With no argument, shows help  
 ✓ Creating an empty 3x2 canvas  
 ✓ Maximum allowed size is 40x80  
 ✓ Loading and prints an empty 5x8 canvas  
 ✓ Using all pens between 0 and 7  
 ✓ Drawing horizontal line on 5x8 canvas with option -h  
 ✓ Drawing vertical line on 5x8 canvas with option -v  
 ✓ Drawing rectangle on 5x8 canvas with option -r  
 ✓ Drawing line on 5x5 canvas with option -l  
 ✓ Drawing circle on 5x8 canvas with option -c  
 ✓ Combining multiple options  
 ✓ Drawing non diagonal segment  
 ✓ Drawing large circle  
 ✓ Clipping line from (1,1) to (5,8)  
 ✓ Clipping circle of radius 3 centered at (3,3)  
 ✓ Option -k is recognized  
 ✓ Forbidding character # in canvas  
 ✓ Canvas of 41 lines is too high  
 ✓ Canvas of 81 columns is too wide  
 ✓ Width must be uniform for all lines  
 ✓ Unrecognized option -a  
 ✓ Option -n must be provided with values  
 ✓ Wrong value with option -p  
 ✓ Wrong value with option -h  
 ✓ Wrong value with option -v  
 ✓ Wrong syntax with option -n  
 ✓ Wrong dimensions with option -n  
 ✓ Negative value with option -h is forbidden  
 ✓ Negative value with option -v is forbidden  
 ✓ Negative positions with option -r are allowed  
 ✓ Negative dimensions with option -r are forbidden  
 ✓ Negative positions with option -l are allowed  
 ✓ Negative positions with option -c are allowed  
 ✓ Negative radius with option -c is forbidden  

34 tests, 0 failures  

## Dépendances

[Bats](https://github.com/bats-core/bats-core)
[GCC](https://gcc.gnu.org/)

## Références

[isdigit](https://www.programiz.com/c-programming/library-function/ctype.h/isdigit)
[strtok](http://www.cplusplus.com/reference/cstring/strtok/)
[Algorithme de
Bresenham](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
[Rosetta
Code](http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C)

## État du projet

* [X] Le nom du dépôt GitLab est exactement `inf3135-automne2021-tp1` (Pénalité de
  **50%**).
* [X] L'URL du dépôt GitLab est exactement (remplacer `utilisateur` par votre
  nom identifiant GitLab) `https://gitlab.info.uqam.ca/utilisateur/inf3135-automne2020-tp1`
  (Pénalité de **50%**).
* [X] L'utilisateur `dogny_g` a accès au projet en mode *Developer*
  (Pénalité de **50%**).
* [X] Le dépôt GitLab est un *fork* de [ce
  dépôt](https://gitlab.info.uqam.ca/dogny_g/tp1-inf3135-a21)
  (Pénalité de **50%**).
* [X] Le dépôt GitLab est privé (Pénalité de **50%**).
* [X] Le dépôt contient au moins un fichier `.gitignore`.
* [X] Le fichier `Makefile` permet de compiler le projet lorsqu'on entre
  `make`. Il supporte les cibles `html`, `test` et `clean`.
* [X] Le nombre de tests qui réussissent/échouent avec la `make test` est
  indiqué quelque part dans le fichier `README.md`.
* [X] Les sections incomplètes de ce fichier (`README.md`) ont été complétées.
* [X] L'en-tête du fichier est documentée.
* [X] L'en-tête des déclarations des fonctions est documentée (*docstring*).
* [X] Le programme ne contient pas de valeurs magiques.
