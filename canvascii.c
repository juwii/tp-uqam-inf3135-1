#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition des variables

#define MAX_HEIGHT 40
#define MAX_WIDTH 80
#define USAGE "\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
Draws on an ASCII canvas. The canvas is provided on stdin and\n\
the result is printed on stdout. The dimensions of the canvas\n\
are limited to at most 40 rows and at most 80 columns.\n\
\n\
If no argument is provided, the program prints this help and exit.\n\
\n\
Canvas options:\n\
  -n HEIGHT,WIDTH           Creates a new empty canvas of HEIGHT rows and\n\
                            WIDTH columns. Should be used as first option,\n\
                            otherwise, the behavior is undefined.\n\
                            Ignores stdin.\n\
  -s                        Shows the canvas and exit.\n\
  -k                        Enables colored output. Replaces characters\n\
                            between 0 and 9 by their corresponding ANSI\n\
                            colors:\n\
                              0: black  1: red      2: green  3: yellow\n\
                              4: blue   5: magenta  6: cyan   7: white\n\
\n\
Drawing options:\n\
  -p CHAR                   Sets the pen to CHAR. Allowed pens are\n\
                            0, 1, 2, 3, 4, 5, 6 or 7. Default pen\n\
                            is 7.\n\
  -h ROW                    Draws an horizontal line on row ROW.\n\
  -v COL                    Draws a vertical line on column COL.\n\
  -r ROW,COL,HEIGHT,WIDTH   Draws a rectangle of dimension HEIGHTxWIDTH\n\
                            with top left corner at (ROW,COL).\n\
  -l ROW1,COL1,ROW2,COL2    Draws a discrete segment from (ROW1,COL1) to\n\
                            (ROW2,COL2) with Bresenham's algorithm.\n\
  -c ROW,COL,RADIUS         Draws a circle centered at (ROW,COL) of\n\
                            radius RADIUS with the midpoint algorithm.\n\
"

struct canvas {
    char pixels[MAX_HEIGHT][MAX_WIDTH]; // A matrix of pixels
    unsigned int width;                 // Its width
    unsigned int height;                // Its height
    char pen;                           // The character we are drawing with
};

enum error {
    OK                         = 0, // Everything is ok
    ERR_WRONG_PIXEL            = 1, // Wrong pixel value in canvas
    ERR_CANVAS_TOO_HIGH        = 2, // Canvas is too high
    ERR_CANVAS_TOO_WIDE        = 3, // Canvas is too wide
    ERR_CANVAS_NON_RECTANGULAR = 4, // Canvas is non rectangular
    ERR_UNRECOGNIZED_OPTION    = 5, // Unrecognized option
    ERR_MISSING_VALUE          = 6, // Option with missing value
    ERR_WITH_VALUE             = 7  // Problem with value
};

/* fonction qui affiche le type de l'erreur
 *@ param numéro de l'erreur
 */

void affichErr(enum error err){
	switch(err){
		case '0':
			fprintf(stderr, "OK");
			break;
		case '1':
			fprintf(stderr, "ERR_WRONG_PIXEL");
			break;
		case '2':
			fprintf(stderr, "ERR_CANVAS_TOO_HIGH");
			break;
		case '3':
			fprintf(stderr, "ERR_CANVAS_TOO_WIDE");
			break;
		case '4':
			fprintf(stderr, "ERR_CANVAS_NON_RECTANGULAR");
			break;
		case '5':
			fprintf(stderr, "ERR_UNRECOGNIZED_OPTION");
			break;
		case '6':
			fprintf(stderr, "ERR_MISSING_VALUE");
			break;
		case '7':
			fprintf(stderr, "ERR_WITH_VALUE");
			break;
	}
}

/* fonction qui affiche une erreur si les dimensions sont supérieures à la limite fixée
 * @param nbLignes nbColonnes : dimensions du canvas
 */

void dimValides(unsigned nbLignes, unsigned nbColonnes){
	
	enum error err;

	if(nbLignes>MAX_HEIGHT){
		err = 2;
		affichErr(err);
		exit(2);
	}
	if(nbColonnes>MAX_WIDTH){
		err = 3;
		affichErr(err);
		exit(3);
	}
}

/* fonction qui affiche le manuel si aucun argument n'a été saisi
 * @param argc
 */

void afficheMan(int argc, char *argv){
	if (argc == 1){
		fprintf(stdout, USAGE, argv);
		exit(0);
	}
}

/* fonction qui affiche un canevas avec des certaines dimensions et un certain caractere
 * @param nbLignes nbColonnes le nombre de lignes et de colonnes du canevas
 * caractere : le caractere que l'on va afficher
 */

void creationCanvas(unsigned int nbLignes,unsigned int nbColonnes, char caractere){
	
	int i, j;

	for(i = 0; i<nbColonnes; i++){
		for(j = 0; j<nbLignes; j++){
			fprintf(stdout,"%c", caractere);
		}
		fprintf(stdout,"\n");
	}
}

/* fonction qui affiche un canevas vide quand l'option -n est utilisée
 * @param argc argv[1] argv[2]
 */

void canvasVide(int argc, char *argvU, char *argvD){
	// a modifier pour nombres > 9 avec itoa
	if(argc == 3){
		if(argvU == "-","n"){
			struct canvas c;

			if(argvD[1] == ','){
				c.height = (unsigned int) argvD[0] - (unsigned int)'0';
			} else {
				//char *arg[];
				//strcat(*arg,argvD[0]);
				//strcat(*arg,argv[1]);
				c.height = (unsigned int)(argvD[0])-(unsigned int)'0';
			}
			if(strlen(argvD) == 4 && argvD[1]!=','){
				c.width = (unsigned int) argvD[3] - (unsigned int)'0';
			} else if(strlen(argvD) == 3){
				c.width = (unsigned int) argvD[2] - (unsigned int)'0';
			} else if(strlen(argvD) == 4 && argvD[1]==','){
				//strcat(argvD[2], argvD[3]);
				c.width = (unsigned int)(argvD[2]) - (unsigned int)'0';
			}


			c.width = (unsigned int) argvD[2] - (unsigned int)'0';
			c.height = (unsigned int) argvD[0] - (unsigned int)'0';
			// initialiser le pixel = rajouter une fonction
			dimValides(c.width, c.height);
			creationCanvas(c.width, c.height, '.');
		}
	}
}


int main(int argc, char *argv[]) {

    //printf("argc = %d\n", argc);
    
    for (unsigned int i = 0; i < argc; ++i) {
       // printf("argv[%d] = %s\n", i, argv[i]);
    }
    
    afficheMan(argc, argv[0]);

    canvasVide(argc, argv[1], argv[2]);

    return 0;
}
