/**
 * TP 1 : affichage d'un canvas avec differentes formes geometriques et possiblement en couleurs
 * @author	Juliette Létondot 
 */


// import des librairies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


// definition des constantes
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

#define ERR_PIXEL "\
Error: wrong pixel value %c\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERR_CANVASH "\
Error: canvas is too high (max height: 40)\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERR_CANVASW "\
Error: canvas is too wide (max width: 80)\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERR_CANVASNONRECT "\
Error: canvas should be rectangular\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERR_OPTION "\
Error: unrecognized option -%c\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERR_MISSVALUE "\
Error: missing value with option -%c\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
"

#define ERROR_VALUE "\
Error: incorrect value with option -%c\n\
Usage: %s [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
	  [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
[...]\n\
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

// Declaration des types
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


// Declaration des fonctions

/** 
 * fonction qui affiche le type de l'erreur
 * @param	err 	numéro de l'erreur
 		valeurPixel	valeur du pixel faux
		option	option qui pose probleme
		argv	liste des arguments en entree
 */
void afficherErreur(enum error err, char option,char valeurPixel,char *argv[]);


/**
 * fonction qui affiche les pixels colorés
 * @param int pixel     pixel a afficher
 */
void affichePixelColore(int pixel);


/**
 * fonction qui affiche un canvas avec des certaines dimensions et un certain caractere
 * @param       struct canvas c	le canvas a afficher
 */
void creationCanvas(struct canvas c);


/**
 * fonction qui affiche une erreur si les dimensions sont supérieures à la limite fixée
 * @param	nbLignes	nombre de lignes du canvas
 * 		nbColonnes	nombre de colonnes du canvas
 * 		argv		la liste des arguments en entree
 */
void dimensionsValides(unsigned nbLignes, unsigned nbColonnes, char *argv[]);


/**
 * fonction qui permet de changer le caractere avec lequel on dessine le canvas
 * @param       argc    nombre d'arguments
 *              argv    liste des arguments en entree
 *              c       pointeur vers un canvas
 */
void changerCrayon(int argc, char *argv[], struct canvas *c);


/**
 * fonction qui remplit le tableau de pixels avec les données d'un fichier en stdin
 * @param	argv		la liste des arguments en entree
 * 		struct canvas c	le canvas
 */
void rempliPixelsAvecStdin(char *argv[], struct canvas *c);


/**
 * fonction qui affiche le manuel si aucun argument n'a été saisi
 * @param       argc    nombre d'arguments
 *              *argv   premier argument
 */
void afficherManuel(int argc, char *argv);


/**
 * fonction qui gère les erreurs de la chaine de caracteres apres le -n
 * @param	ch1		premier argument de l'option
 * 		ch2 		deuxieme argument de l'option 	
 * 		option		option entree
 * 		argv		entrees
*/
void erreurVirgules(char *ch1, char *ch2, char option, char *argv[]);


/**
 * fonction qui supprime le caractere virgule d'une chaine de caractères et initialise les dimensions du canevas
 * @param       chaine	arguments de l'option
 * 		c	canvas
 * 		option	option souhaitee
 * 		argv	liste des arguments en entree
*/
void supprimerVirgule(char *chaine, struct canvas *c,char option, char *argv[]);


/**
 * fonction qui affiche un canevas vide quand l'option -n est utilisée
 * @param	argc	nombre d'entrees 
 * 		argvU	pointeur vers le premier argument 
 * 		argv[2]	pointeur vers le deuxieme argument
 * 		c	pointeur vers le canvas
 */
void canvasVide(char *argv[], char *argvD, struct canvas *c);


/**
 * fonction qui gère l'erreur de saisie pour la ligne horizontale
 * @param 	struct canvas c	pointeur vers un canvas 
 * 		*argv		liste des arguments en entree
 */
void afficheErreurLigneH(struct canvas *c, char *argv[], int i);


/**
 * fonction qui affiche une ligne horizontale
 * @param	i	indice de argv ou il y a l'option
 * 		argv	liste des arguments en entree
 * 		c	pointeur vers le canvas
 */
void traceLigneHorizontale(int i, char *argv[], struct canvas *c);


/**
 * fonction qui gère l'erreur de saisie pour la ligne horizontale
 * @param 	struct canvas c	pointeur vers le canvas 
 * 		*argv		liste des arguments en entree
 */
void afficheErreurLigneV(struct canvas *c, char *argv[], int i);


/**
 * fonction qui trace une ligne verticale 
 * @param	i	indice de la liste argv ou il y a l'option
 * 		argv	liste des arguments en entree
 * 		c	pointeur vers un canvas
 */
void traceLigneVerticale(int i, char *argv[], struct canvas *c);


/**
 * fonction qui convertit les données en entrée en int pour obtenir les données du rectangle
 * @param 	rectangle	pointeur vers un tableau qui contiendra les coordonnees
 * 		argv		liste des arguments en entree
 * 		i		indice de la liste argv ou il y a l'option 
 * 		option		option souhaitee
 */		
void trouveCoordonnees(int *rectangle, char *argv[], int i, char option);


/**
 * fonction qui permet de tracer un rectangle
 * @param	i	indice de argv ou il y a l'option	 
 * 		argv 	liste des arguments en entree
 * 		c	pointeur vers un canvas
*/
void traceRectangle(int i, char *argv[], struct canvas *c);


/**
 * fonction qui trace un segment entre deux points
 * @param	i	indice de argv ou il y a l'option
 * 		argv	liste des arguments en entree
 * 		c	pointeur vers un canvas
 */
void traceSegment(int i,char *argv[], struct canvas *c);


/**
 * fonction qui affiche une erreur en cas de rayon negatif
 * @param	rayon	entier rayon du cercle souhaite
 * 		argv	liste des arguments en entree
 */
void afficheErreurCercle(int rayon, char *argv[]);


/**
 * fonction qui trace un cercle
 * @param	i	indice de argv ou il y a l'option
 * 		argv	liste des arguments en entree
 * 		c	pointeur vers un canvas
 */
void traceCercle(int i, char *argv[], struct canvas *c);


/**
 * fonction qui affiche les pixels en couleur
 * @param	c	pointeur vers un canvas
 */
void afficheCouleurs(struct canvas *c);


/**
 * fonction qui detecte un caractere interdit dans le canvas
 * @param       argv	liste des arguments en entree
 *              struct canvas c	pointeur vers un canvas
 */
void erreurCaractereInterdit(char *argv[], struct canvas *c);


/**
 * fonction qui appelle toutes les fonctions de gestion d'erreur
 * @param       argv		liste des arguments en entree
 *              struct canvas *c pointeur vers un canvas
 */
void gestionErreurs(char *argv[], struct canvas *c);


/**
 * fonction qui check si les arguments de l'option sont bons
 * @param	i	indice de l'option dans argv
 * 		option	option souhaitee
 * 		argv	liste des arguments en entree
*/
void verifierArgOptions(int i, char option, int argc, char *argv[]);


/**
 * fonction qui appelle les options en fonction de l'entree
 * @param       argc	nombre d'arguments en entree
 *              argv	liste des arguments
 */
void appelOptions(int argc, char *argv[], struct canvas *c);


// Implementation des fonctions
void afficherErreur(enum error err, char option,char valeurPixel,char *argv[]){
	switch(err){
		case 0:
			fprintf(stderr, "OK");
			break;
		case 1:
			fprintf(stderr, ERR_PIXEL,valeurPixel,argv[0]);
			break;
		case 2:
			fprintf(stderr, ERR_CANVASH, argv[0]);
			break;
		case 3:
			fprintf(stderr, ERR_CANVASW, argv[0]);
			break;
		case 4:
			fprintf(stderr, ERR_CANVASNONRECT,argv[0]);
			break;
		case 5:
			fprintf(stderr, ERR_OPTION,option,argv[0]);
			break;
		case 6:
			fprintf(stderr, ERR_MISSVALUE,option,argv[0]);
			break;
		case 7:
			fprintf(stderr, ERROR_VALUE,option,argv[0]);
			break;
	}
}


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


void creationCanvas(struct canvas c){
        int i, j;
        if(c.couleur == 0){
        	for(i = 0; (unsigned int)i<c.height; i++){
                	for(j = 0; (unsigned int)j<c.width; j++){
                        	fprintf(stdout,"%c", c.pixels[i][j]);
                	}
                	fprintf(stdout,"\n");
        	}	
        }
}


void dimensionsValides(unsigned nbLignes, unsigned nbColonnes, char *argv[]){
	if(nbLignes>MAX_HEIGHT && nbColonnes>MAX_WIDTH){
		afficherErreur(7,'n','.',argv);
		exit(7);
	}
	if(nbLignes>MAX_HEIGHT){
                afficherErreur(2,'n','.',argv);
                exit(2);
        }
	if(nbColonnes>MAX_WIDTH){
                afficherErreur(3, 'n','.',argv);
                exit(3);
        }
}


void changerCrayon(int argc, char *argv[], struct canvas *c){	
	int nbArguments = argc;
	int changementCrayon = 0;
 	while(nbArguments>2){
        	if(argv[nbArguments][0] == '-' && argv[nbArguments][1] == 'p'){
                	c->pen = argv[nbArguments+1][0];
			changementCrayon = 1;
			break;
		}
		nbArguments-=1;
	}
	if(changementCrayon == 0){
		c->pen = '7';
	}
	else if(!isdigit(argv[nbArguments+1][0])){
		afficherErreur(7,'p','.',argv);
		exit(7);
	}
}


void rempliPixelsAvecStdin(char *argv[], struct canvas *c){
	FILE* fichier = stdin;
	int i,j;
	i = j = 0;
	char caractere;
	fseek(fichier,0,SEEK_SET);
	while(1){
		caractere = (char)fgetc(fichier);
		if(feof(fichier)){
			break;
		}
		if(caractere == '\n'){
			i = i + 1;
			dimensionsValides(i, j, argv);
			if(i>1 && (unsigned int)j!=c->width && j<=80 && i<=40){
				afficherErreur(4,'n','.',argv);
				exit(4);
			}
			if(j>0 && j<81){
				c->width = j;
			}
			j = 0;
		} else {
			c->pixels[i][j] = caractere;
			j = j + 1;
		}
	}
	c->height = (unsigned int)i;
	fclose(fichier);
}


void afficherManuel(int argc, char *argv){
	if (argc == 1){
		fprintf(stdout, USAGE, argv);
		exit(0);
	}
}


void erreurVirgules(char *ch1, char *ch2, char option, char *argv[]){	
	int i;
	for(i = 0; (long unsigned int)i<strlen(ch1);i++){
		if(!isdigit(ch1[i])){
			afficherErreur(7,option,'.',argv);
			exit(7);
        	}
	}
	int j;
	for(j = 0; (long unsigned int)j<strlen(ch2);j++){
        	if(!isdigit(ch2[j])){
			afficherErreur(7,option,'.',argv);
              		exit(7);
		}
        }
}


void supprimerVirgule(char *chaine, struct canvas *c,char option, char *argv[]){
	char *ch1, *ch2;
	ch1 = strtok(chaine, ",");
	ch2 = (strtok(NULL, ","));
       	if(ch1==NULL || ch2 == NULL){
		afficherErreur(7,option,'.',argv);
		exit(7);
	}	
	erreurVirgules(ch1, ch2,option,argv);
	if(option!='n'){
		dimensionsValides(atoi(ch1), atoi(ch2),argv);
	}
	c->height = atoi(ch1);
	c->width = atoi(ch2);
}


void canvasVide(char *argv[], char *argvD, struct canvas *c){
	supprimerVirgule(argvD, c, 'n', argv);
	int i,j;
	if((unsigned int)c->height>MAX_HEIGHT || (unsigned int)c->width>MAX_WIDTH){
        	afficherErreur(7,'n','.',argv);
                exit(7);
        }
	for(i = 0; (unsigned int)i<c->height; i++){
		for(j = 0; (unsigned int)j<c->width; j++){
			c->pixels[i][j] = '.';
		}
	}
}


void afficheErreurLigneH(struct canvas *c, char *argv[], int i){
	if((unsigned int)atoi(argv[i + 1])>= c->height || atoi(argv[i+1])<=0){
		afficherErreur(7,'h','.',argv);		
		exit(7);
	}
}


void traceLigneHorizontale(int i, char *argv[], struct canvas *c){
	int j;
	changerCrayon(i,argv,c);
	afficheErreurLigneH(c,argv,i);
	for(j = 0; (unsigned int)j<c->width; j++){
		c->pixels[atoi(argv[i + 1])][j] = c->pen;
	}
}


void afficheErreurLigneV(struct canvas *c, char *argv[], int i){
        if((unsigned int)atoi(argv[i + 1])>= c->width || atoi(argv[i+1])<=0){
		afficherErreur(7,'v','.',argv);
                exit(7);
        }
}


void traceLigneVerticale(int i, char *argv[], struct canvas *c){
        int j;
	changerCrayon(i,argv,c);
        afficheErreurLigneV(c,argv,i);
        for(j = 0; (unsigned int)j<c->height; j++){
        	c->pixels[j][atoi(argv[i + 1])] = c->pen;
        }
}


void trouveCoordonnees(int *rectangle, char *argv[], int i, char option){
	char *ch1;
	ch1=(strtok(argv[i+1], ","));
	char *ch2;
	ch2=(strtok(NULL, ","));
	char *ch3;
	ch3=(strtok(NULL, ","));
	char *ch4;
	ch4=(strtok(NULL, ","));
	if(ch1==NULL || ch2 == NULL || ch3 == NULL || ch4==NULL){
		afficherErreur(7,option,'.',argv);
		exit(7);
	}
	rectangle[0] = atoi(ch1);
        rectangle[1] = atoi(ch2);
        rectangle[2] = atoi(ch3);
        rectangle[3] = atoi(ch4);
	if(option == 'r'){
		if(rectangle[2]<0 || rectangle[3]<0){
			afficherErreur(7,'r','.',argv);
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


void traceRectangle(int i, char *argv[], struct canvas *c){
	int j,k;
	int rectangle[4];
	int *p;
	p = &rectangle[0];
	trouveCoordonnees(p,argv,i,'r');
	changerCrayon(i,argv,c);
	for(j = rectangle[0] ; j<rectangle[0] + rectangle[2] ; j++){
		if(rectangle[1]>=0 && j>=0){
			c->pixels[j][rectangle[1]] = c->pen;
		}
		if(rectangle[1]+rectangle[3]-1>=0 && j>=0){
			c->pixels[j][rectangle[1]+rectangle[3]-1]=c->pen;
		}
	}
	for(k = rectangle[1]; k<rectangle[1] + rectangle[3]; k++){
		if(rectangle[0]>=0 && k>=0){
			c->pixels[rectangle[0]][k] = c->pen;
		}
		if(rectangle[0]+rectangle[2]-1>=0 && k>=0){
			c->pixels[rectangle[0]+rectangle[2]-1][k] = c->pen;
		}
	}
}


void traceSegment(int i,char *argv[], struct canvas *c){
        int segment[4];
        int *p;
        p = &segment[0];
	changerCrayon(i,argv,c);
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


void afficheErreurCercle(int rayon, char *argv[]){
	if(rayon<0){
		afficherErreur(7,'c','.',argv);
		exit(7);
	}
}


void traceCercle(int i, char *argv[], struct canvas *c){
	int cercle[3];
	changerCrayon(i,argv,c);
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


void afficheCouleurs(struct canvas *c){
	c->couleur = 1;		
	int j,k;
	for(j = 0; (unsigned int)j<c->height; j++){
		for(k = 0; (unsigned int)k<c->width; k++){
			affichePixelColore(c->pixels[j][k]);
		}
		fprintf(stdout,"\n");
	}
}


void erreurCaractereInterdit(char *argv[], struct canvas *c){
	int i,j;
	for(i = 0; (unsigned int)i<c->height; i++){
		for(j = 0; (unsigned int)j<c->width; j++){
			if(!isdigit(c->pixels[i][j]) && c->pixels[i][j]!='.'){
				afficherErreur(1,'n',c->pixels[i][j],argv);
				exit(1);
			}
		}
	}
}


void gestionErreurs(char *argv[], struct canvas *c){
	erreurCaractereInterdit(argv,c);
	dimensionsValides(c->height,c->width, argv);
}


void verifierArgOptions(int i, char option, int argc, char *argv[]){
	if(argc-1 == i){
	       afficherErreur(6,option,'.',argv);
	       exit(6);
	}
}


void appelOptions(int argc, char *argv[], struct canvas *c){
        int i;
        c->pen = '7';
        afficherManuel(argc, argv[0]);
        if(argv[1][1] != 'n'){
                rempliPixelsAvecStdin(argv, c);
        }
        for(i = 1; i<argc; i++){
                if(argv[i][0] == '-' && strlen(argv[i])==2 && !isdigit(argv[i][1])){
                        if(argv[i][1] == 'n'){
                                verifierArgOptions(i, 'n', argc, argv);
				canvasVide(argv, argv[2], c);
                        }
                        else if(argv[i][1] == 's'){
                                gestionErreurs(argv, c);
				creationCanvas(*c);
                                exit(0);
			}
                        else if(argv[i][1] == 'h'){
				verifierArgOptions(i, 'h', argc, argv);
                                traceLigneHorizontale(i,argv,c);
                        }
                        else if(argv[i][1] == 'v'){
				verifierArgOptions(i, 'v', argc, argv);
                                traceLigneVerticale(i,argv,c);
                        }
                        else if(argv[i][1] == 'r'){
				verifierArgOptions(i, 'r', argc, argv);
                                traceRectangle(i,argv,c);
                        }
                        else if(argv[i][1] == 'l'){
				verifierArgOptions(i, 'l', argc, argv);
                                traceSegment(i,argv,c);
                        }
                        else if(argv[i][1] == 'c'){
				verifierArgOptions(i, 'c', argc, argv);
                                traceCercle(i,argv,c);
                        }
                        else if(argv[i][1] == 'p'){
				verifierArgOptions(i, 'p', argc, argv);
                                changerCrayon(i,argv,c);
                        }
                        else if(argv[i][1] == 'k'){
                                afficheCouleurs(c);
                        }
                        else {
                                afficherErreur(5,argv[i][1],'.',argv);
                                exit(5);
                        }
                }
        }
}


int main(int argc, char *argv[]) {
    struct canvas c;
    c.pen = '7';
    c.couleur = 0;
    appelOptions(argc, argv, &c);
    creationCanvas(c);
    gestionErreurs(argv,&c);
    return 0;
}
