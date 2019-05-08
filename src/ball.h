#ifndef BALL
#define BALL
#include "board.h"
#include "point.h"

/* Clanovi strukture Ball:
 * pos - x, y i z koordinata pozicije loptice,
 * oldPos - prethodna pozicija,
 * v - vektor brzine loptice,
 * size - velicina loptice,
 * a - vektor ubrzanja loptice,
 * mass - masa loptice.
 * isInHole - da li je centar lopte unutar rupe.
 * holeCenter - ako je lopta u rupi, gde je centar rupe.
 * endGame - da li je loptica stigla do kraja (ciljne rupe). */
struct Ball {

  double pos[3];
  double oldPos[3];
  double v[3];
  double size;
  double a[3];
  double mass;
  int isInHole;
  struct Point holeCenter;
  int endGame;
};

/* Funkcije koje se koriste za crtanje i pomeranje loptice. */
void nextPos(struct Ball* b, struct Board board, int time);
void drawBall(struct Ball b, struct Board board);
void initBall(struct Ball* b, double size, struct Point pos);
void calcAcc(struct Ball* b, struct Board board, double time);
int testHole(struct Ball* b, struct Board board, struct Point* p);
int testTargetHole(struct Ball* b, struct Board board, struct Point* p);
int collisionX(struct Ball ball, struct Wall wall);
int collisionY(struct Ball ball, struct Wall wall);

#endif
