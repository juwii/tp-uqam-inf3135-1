#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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

#define ERROR_VALUE_H "\
Error: incorrect value with option -h \n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR] \n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH] \n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS] \n\
[...]\n\
"

#define ERROR_VALUE_V "\
Error: incorrect value with option -v \n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR] \n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH] \n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS] \n\
[...] \n\
"

#define ERR_VALUE_COOR_RECT "\
Error: incorrect value with option -r \n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR] \n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH] \n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS] \n\
[...] \n\
"

#define ERROR_VALUE_C "\
Error: incorrect value with option -c \n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR] \n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH] \n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS] \n\
[...] \n\
"

#define RESET "\x1B[0m" // remise a zero
#define NOIR "\x1B[40m" //0
#define ROUGE "\x1B[41m" //1
#define VERT "\x1B[42m" //2
#define JAUNE "\x1B[43m" //3 
#define BLEU "\x1B[44m" //4
#define ROSE "\x1B[45m" //5
#define CYAN "\x1B[46m" //6
#define BLANC "\x1B[47m" //7

struct canvas {
    char pixels[MAX_HEIGHT][MAX_WIDTH]; // A matrix of pixels
    unsigned int width;                 // Its width
    unsigned int height;                // Its height
    char pen;                           // The character we are drawing with
    int couleur;			// 0 si sans couleur 1 sinon
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
 *@ param err 	numéro de l'erreur
 */

void afficherErreur(enum error err){
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

/* fonction qui affiche les pixels colorés
 * @param int pixel
 */

void affichePixelColore(int pixel){

	switch(pixel){
		case '0':
			fprintf(stdout, NOIR " " RESET);
			break;
		case '1':
			fprintf(stdout, ROUGE " " RESET);
			break;
		case '2':
			fprintf(stdout, VERT " " RESET);
			break;
		case '3':
			fprintf(stdout, JAUNE " " RESET);
                        break;
		case '4':
			fprintf(stdout, BLEU " " RESET);
                        break;
		case '5':
			fprintf(stdout, ROSE " " RESET);
                        break;
		case '6':
			fprintf(stdout, CYAN " " RESET);
                        break;
		case '7':
			fprintf(stdout, BLANC " " RESET);
                        break;
		default:
			fprintf(stdout, " ");
			break;
	}
}

/* fonction qui affiche un canevas avec des certaines dimensions et un certain caractere
 * @param nbLignes, nbColonnes le nombre de lignes et de colonnes du canevas
 * caractere : le caractere que l'on va afficher
 */

void creationCanvas(struct canvas c){

        int i, j;

        if(c.couleur == 0){

        for(i = 0; i<c.height; i++){
                for(j = 0; j<c.width; j++){
                        fprintf(stdout,"%c", c.pixels[i][j]);
                }
                fprintf(stdout,"\n");
        }

        }
}


/* fonction qui permet de changer le caractere avec lequel on dessine le canvas
 * @param
 */

void changerCrayon(int argc, char *argv[], struct canvas *c){
	
	int i;
	int nbArguments = argc;
	int changementCrayon = 0;

	while(nbArguments>1){
        	if(argv[nbArguments-1][0] == '-' && argv[nbArguments-1][1] == 'p'){
                	c->pen = argv[i+1][0];
			changementCrayon = 1;
			break;
		}
		nbArguments-=1;
	}

	if(changementCrayon == 0){
		c->pen = '7';
	}

}

/* fonction qui remplit le tableau de pixels avec les données d'un fichier en stdin
 * @param argc argv struct canvas c
 */

void rempliPixelsAvecStdin(int *argc, char *argv[], struct canvas *c){
	
	if(*argc>1){

	if(argv[1][1] != 'n'){

	FILE* fichier = stdin;
	int i,j;
	i = 0; 
	j = 0;
	char caractere;

	// affich erreur en cas d'echec ouverture
	
	fseek(fichier,0,SEEK_SET);
	
	if(fichier == NULL){
		printf("erreur");
	}
	
	while(!feof(fichier)){
		caractere = (char)fgetc(fichier);
		if(caractere == '\n'){
			i = i + 1;
			c->width = j;
			j = 0;
		
		} else {
			c->pixels[i][j] = caractere;
			j = j + 1;
		}
	}
	
	fclose(fichier);

	c->height = i;
	
	int k;
	for(k = 0; k<*argc; k++){	
	if(argv[k][0] == '-' && argv[k][1] == 's'){
		*argc = k+1;
	}
	}
	}
	}
}

/* fonction qui affiche une erreur si les dimensions sont supérieures à la limite fixée
 * @param nbLignes nbColonnes : dimensions du canvas
 */

void dimensionsValides(unsigned nbLignes, unsigned nbColonnes){
	
	enum error err;

	if(nbLignes>MAX_HEIGHT){
		err = 2;
		afficherErreur(err);
		exit(2);
	}
	if(nbColonnes>MAX_WIDTH){
		err = 3;
		afficherErreur(err);
		exit(3);
	}
}

/* fonction qui affiche le manuel si aucun argument n'a été saisi
 * @param 	argc	nombre d'arguments
 * 		*argv	premier argument
 */

void afficherManuel(int argc, char *argv){
	if (argc == 1){
		fprintf(stdout, USAGE, argv);
		exit(0);
	}
}

/* fonction qui gère les erreurs de la chaine de caracteres apres le -n
 * @param ch1 ch2 nbVirgules chaine
*/

void erreurVirgules(char *ch1, char *ch2, char *chaine){
	
	int i;
	for(i = 0; i<strlen(ch1);i++){
		if(!isdigit(ch1[i])){
			exit(7);
        	}
	}

	int j;
	for(j = 0; j<strlen(ch2);j++){
        	if(!isdigit(ch2[j])){
              		exit(7);
		}
        }
}

/* fonction qui supprime le caractere virgule d'une chaine de caractères et initialise les dimensions du canevas
 * @param
*/

void supprimerVirgule(char *chaine, struct canvas *c){

	char *ch1;
	char *ch2;
	
	ch1 = strtok(chaine, ",");
	ch2 = (strtok(NULL, ",")); 

	erreurVirgules(ch1, ch2, chaine);

	c->height = atoi(ch1);
	c->width = atoi(ch2);
}



/* fonction qui affiche un canevas vide quand l'option -n est utilisée
 * @param argc argv[1] argv[2]
 */

void canvasVide(int argc, char *argvU, char *argvD, struct canvas *c){
	
	if(argvU[0]=='-' && argvU[1] == 'n' && strlen(argvU) == 2){
		
		c->pen = '.';
		supprimerVirgule(argvD, c);
		// -p possible avant ??
		int i,j;
		for(i = 0; i<c->height; i++){
			for(j = 0; j<c->width; j++){
				c->pixels[i][j] = '.';
			}
		}

		dimensionsValides(c->width, c->height);
		
	}
}

/* fonction qui gère l'erreur de saisie pour la ligne horizontale
 * @param struct canvas c *argv
 */

void afficheErreurLigneH(struct canvas *c, char *argv[], int i){
	
	if(atoi(argv[i + 1])> c->height || argv[i+1]<0){
		fprintf(stdout, ERROR_VALUE_H, argv[0]);		
		exit(7);
	}
}


/* fonction qui affiche une ligne horizontale
 * @param
 */

void traceLigneHorizontale(int argc, char *argv[], struct canvas *c){

	int i;
	int j;

	for(i = 0; i<argc; i++){
		if(argv[i][0] == '-' && argv[i][1]=='h'){
			changerCrayon(argc,argv,c);
			afficheErreurLigneH(c,argv,i);
			for(j = 0; j<c->width; j++){
				c->pixels[atoi(argv[i + 1])][j] = c->pen;
			}
		}
	}
}

/* fonction qui gère l'erreur de saisie pour la ligne horizontale
 * @param struct canvas c *argv
 */

void afficheErreurLigneV(struct canvas *c, char *argv[], int i){

        if(atoi(argv[i + 1])> c->width || argv[i+1]<0){
                fprintf(stdout, ERROR_VALUE_V, argv[0]);
                exit(7);
        }
}

/* fonction qui trace une ligne verticale de 7
 * @param argc argv c
 */

void traceLigneVerticale(int argc, char *argv[], struct canvas *c){
	int i;
        int j;
        for(i = 0; i<argc; i++){
                if(argv[i][0] == '-' && argv[i][1]=='v'){
			changerCrayon(argc,argv,c);
                        afficheErreurLigneV(c,argv,i);
                        for(j = 0; j<c->height; j++){
                                c->pixels[j][atoi(argv[i + 1])] = c->pen;
                        }
                }
        }
}

/* fonction qui convertit les données en entrée en int pour obtenir les données du rectangle
 * @param rectangle argv
 */

void trouveCoordonnees(int *rectangle, char *argv[], int i, char option){

        rectangle[0] = atoi(strtok(argv[i+1], ","));
        rectangle[1] = atoi(strtok(NULL, ","));
	rectangle[2] = atoi(strtok(NULL, ","));
	rectangle[3] = atoi(strtok(NULL, ","));
	
	// gestion des erreurs du rectangle
	if(option == 'r'){

	if(rectangle[2]<0 || rectangle[3]<0){
		fprintf(stderr, ERR_VALUE_COOR_RECT, argv[0]);
		exit(7);
	}
	}

	if(option == 'l'){
		if(rectangle[1]>rectangle[3]){
			int tmp1 = rectangle[0];
			int tmp2 = rectangle[1];
			rectangle[0] = rectangle[2];
			rectangle[1] = rectangle[3];
			rectangle[2] = tmp1;
			rectangle[3] = tmp2;
		
		}
	}
}


/* fonction qui permet de tracer un rectangle
 * @param argc argv c
*/

void traceRectangle(int argc, char *argv[], struct canvas *c){

	int i,j,k;
	int rectangle[4];
	int *p;

	p = &rectangle[0];

	for(i = 0; i<argc; i++){
		if(argv[i][0] == '-' && argv[i][1] == 'r'){
			trouveCoordonnees(p,argv,i,'r');
			changerCrayon(argc,argv,c);
			// affiche erreur en cas de tailles négatives
			for(j = rectangle[0] ; j<rectangle[0] + rectangle[2] ; j++){
				if(rectangle[1]>=0){
				c->pixels[j][rectangle[1]] = c->pen;
				}
				if(rectangle[1]+rectangle[3]>=0){
				c->pixels[j][rectangle[1]+rectangle[3]]=c->pen;
				}
			}
			for(k = rectangle[1]; k<rectangle[1] + rectangle[3]; k++){
				if(rectangle[0]>=0){
				c->pixels[rectangle[0]][k] = c->pen;
				}
				if(rectangle[0]+rectangle[2]-1>=0){
				c->pixels[rectangle[0]+rectangle[2]-1][k] = c->pen;
				}
			}
		}
	}
}

/* fonction qui trace un segment entre deux points
 * @param
 */

void traceSegment(int argc,char *argv[], struct canvas *c){
	
	// obtenir les coordonnées
	int i,j,k;
        int segment[4];
        int *p;

        p = &segment[0];

	for(i = 0; i<argc; i++){
                if(argv[i][0] == '-' && argv[i][1] == 'l'){
			changerCrayon(argc,argv,c);
			trouveCoordonnees(p,argv,i,'l');

			// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm?section=7#All_cases
			int dx,sx,dy,sy,err,e2;
			int etat = 1;
			dx = abs(segment[2]-segment[0]);
			sx = segment[0]<segment[2] ? 1 : -1;
			dy = -abs(segment[3]-segment[1]);
			sy = segment[1]<segment[3];
			err = dx + dy;
			
			while(etat==1){
				if(segment[0]>=0){
				c->pixels[segment[0]][segment[1]] = c->pen;
				}
				if(segment[0] == segment[2] && segment[1] == segment[3]){
				        etat = 0;	
					break;
				}
				e2 = 2*err;
				if(e2>=dy){
					err += dy;
					segment[0] += sx;
					
				}
				if(e2<=dx){
					err += dx;
					segment[1] += sy;
				}
			}


		}
	}
}

/* fonction qui affiche une erreur en cas de rayon negatif
 * @param
 */

void afficheErreurCercle(int rayon, char *argv[]){
	if(rayon<0){
		fprintf(stderr, ERROR_VALUE_C, argv[0]);
		exit(7);
	}
}

/* fonction qui trace un cercle
 * @param
 */

void traceCercle(int argc, char *argv[], struct canvas *c)
{

	int i;
	int cercle[3];

	for(i = 0; i<argc; i++){
                if(argv[i][0] == '-' && argv[i][1] == 'c'){
			changerCrayon(argc,argv,c);

			cercle[0] = atoi(strtok(argv[i+1], ","));
        		cercle[1] = atoi(strtok(NULL, ","));
        		cercle[2] = atoi(strtok(NULL, ","));
			afficheErreurCercle(cercle[2],argv);

			int f = 1 - cercle[2];
			int ddF_x = 0;
    			int ddF_y = -2 * cercle[2];
    			int x = 0;
    			int y = cercle[2];
			
			if(cercle[0]>=0 && cercle[1]+cercle[2]>=0){
			c->pixels[cercle[0]][cercle[1]+cercle[2]] = c->pen;
			}
			if(cercle[0]>=0 && cercle[1]-cercle[2]>=0){
			c->pixels[cercle[0]][cercle[1]-cercle[2]] = c->pen;
			}
			if(cercle[0]+cercle[2]>=0 && cercle[1]>=0){
			c->pixels[cercle[0]+cercle[2]][cercle[1]] = c->pen;
			}
			if(cercle[0]-cercle[2]>=0 && cercle[1]>=0){
			c->pixels[cercle[0]-cercle[2]][cercle[1]]=c->pen;
			}

    			while(x < y){
        			if(f >= 0) {		
            				y--;
            				ddF_y += 2;
            				f += ddF_y;
       				}
    		
        			x++;
        			ddF_x += 2;
        			f += ddF_x + 1;

				if(cercle[0]+x>=0 && cercle[1]+y>=0){
				c->pixels[cercle[0]+x][cercle[1]+y] = c->pen;
				}
				if(cercle[0]-x>=0 && cercle[1]+y>=0){
				c->pixels[cercle[0]-x][cercle[1]+y] = c->pen;
				}
				if(cercle[0]+x>=0 && cercle[1]-y>=0){
				c->pixels[cercle[0]+x][cercle[1]-y] = c->pen;
				}
				if(cercle[0]-x>=0 && cercle[1]-y>=0){
				c->pixels[cercle[0]-x][cercle[1]-y] = c->pen;
				}
				if(cercle[0]+y>=0 && cercle[1]+x>=0){
				c->pixels[cercle[0]+y][cercle[1]+x] = c->pen;
				}
				if(cercle[0]-y>=0 && cercle[1]+x>=0){
				c->pixels[cercle[0]-y][cercle[1]+x] = c->pen;
				}
				if(cercle[0]+y>=0 && cercle[1]-x>=0){
				c->pixels[cercle[0]+y][cercle[1]-x] = c->pen;
				}
				if(cercle[0]-y>=0 && cercle[1]-x>=0){
				c->pixels[cercle[0]-y][cercle[1]-x] = c->pen;
				}
    			}
		}
	}
}


/* fonction qui affiche les pixels en couleur
 * @param argc argv c
 */

void afficheCouleurs(int argc, char *argv[], struct canvas *c){
	
	int i;	
	for(i = 0; i<argc; i++){
		if(argv[i][0] == '-' && argv[i][1] == 'k'){
			c->couleur = 1;		
			int j,k;
			for(j = 0; j<c->height; j++){
				for(k = 0; k<c->width; k++){
					affichePixelColore(c->pixels[j][k]);
				}
				fprintf(stdout,"\n");
			}
		}
	}
}


int main(int argc, char *argv[]) {

    //printf("argc = %d\n", argc);
    
    for (unsigned int i = 0; i < argc; ++i) {
     // printf("argv[%d] = %s\n", i, argv[i]);
    }
    
    struct canvas c;
    c.pen = '7';
    c.couleur = 0;

    rempliPixelsAvecStdin(&argc, argv, &c);

    afficherManuel(argc, argv[0]);

    canvasVide(argc, argv[1], argv[2], &c);

    traceLigneHorizontale(argc,argv,&c);

    traceLigneVerticale(argc,argv,&c);

    traceRectangle(argc,argv,&c);

    traceSegment(argc,argv,&c);

    traceCercle(argc,argv,&c);

    afficheCouleurs(argc,argv,&c);

    creationCanvas(c);

    return 0;
}
