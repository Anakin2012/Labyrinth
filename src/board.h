#include <GL/glut.h>
#include "level.h"
#include "point.h"
#ifndef BOARD
#define BOARD

/* Clanovi strukture Board:
 * points - 16 tacaka pomocu kojih crtamo kocku koja predstavlja tablu,
 * angles - 3 ugla za koje se tabla rotira, za svaku od osa,
 * level - struktura koja daje izgled table, razlicite rasporede rupa i zidova na tabli,
 * unit - Bice 20.0, koristi se za skaliranje kod pozicija zidova i rupa. */
struct Board {
  struct Point points[16];
  double angles[3];
  struct Level* level;
  double unit;
};

/* Funkcije koje se koriste pri crtanju table. */
void drawBoard(struct Board board, GLuint* texture);
void initBoard(struct Board* board, struct Level* level, double unit);
void moveBoard(struct Board* board, int where, double maxAngle, double step);
void drawHole(double x, double y,double size, GLUquadricObj* cylinder);
void drawWalls(struct Board board);
void drawWallH(struct Wall wall, double unit);
void drawWallV(struct Wall wall, double unit);
void setTextPoint(int i);

#endif
