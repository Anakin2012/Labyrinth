#include <GL/glut.h>

#include "point.h"
#ifndef LEVEL
#define LEVEL
#define MAX_HOLES 30
#define MAX_WALLS 1000

/* Clanovi strukture Level:
 * rows - redovi na tabli, niz od 40 stringova,
 * numOfHoles - broj rupa,
 * numOfWalls - broj zidova,
 * numOfCorners - broj krajeva zidova,
 * holes - tacke koje predstavljaju koordinate rupa,
 * walls - strukture Wall koje predstavljaju zidove.
 * targetHole - struktura Point koja predstavlja tacku centra ciljne rupe.
 * corners - struktora Wall koja predstavlja krajeve vertikalnih i horizontalnih zidova.
 * ballPos - inicijalna pozicija loptice.
 * cylinder - dodato za crtanje rupa.
*/
struct Level {
  char* rows[40];
  int numOfHoles;
  int numOfWalls;
  int numOfCorners;
  struct Point holes[MAX_HOLES];
  struct Wall walls[MAX_WALLS];
  struct Point targetHole;
  struct Wall corners[MAX_WALLS*2];
  struct Point ballPos;
  GLUquadricObj* cylinder;
};

/* Funkcije za inicijalizaciju. */
void initLevel(struct Level* l, GLUquadricObj* holeObject);
void initHoles(struct Level* l, GLUquadricObj* holeObject);
void initWalls(struct Level* l);

#endif
