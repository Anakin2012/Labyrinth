#include "board.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

/* Funkcija koja iscrtava inicijalnu verziju table. */
void initBoard(struct Board* board, struct Level* level, double unit) {

  /* Inicijalizuju se promenljive. */
  board->unit = 1.0/unit;
  board->level = level;

  /* Na pocetku se uglovi postavljaju na 0. */
  for(int i = 0; i < 3; i++)
    board->angles[i] = 0;

  /* Gornji deo table. */
  board->points[0].x = -1.0;
  board->points[0].y = -1.0;
  board->points[0].z = 0.0;

  board->points[1].x = 1.0;
  board->points[1].y = -1.0;
  board->points[1].z = 0;

  board->points[2].x = 1.0;
  board->points[2].y = 1.0;
  board->points[2].z = 0.0;

  board->points[3].x = -1.0;
  board->points[3].y = 1.0;
  board->points[3].z = 0.0;

  /* Donji deo kvadra, sa prednje strane. */
  board->points[4].x = -1.0;
  board->points[4].y = -1.0;
  board->points[4].z = 0;

  board->points[5].x = -1.0;
  board->points[5].y = -1.0;
  board->points[5].z = -0.1;

  board->points[6].x = 1.0;
  board->points[6].y = -1.0;
  board->points[6].z = -0.1;

  board->points[7].x = 1.0;
  board->points[7].y = -1.0;
  board->points[7].z = 0.0;

  /* Donja strana sve isto kao gornja, samo pomerena po z-osi. */
  board->points[8].x = -1.0;
  board->points[8].y = -1.0;
  board->points[8].z = -1.0/ 10;

  board->points[9].x = 1.0;
  board->points[9].y = -1.0;
  board->points[9].z = -1.0/ 10;

  board->points[10].x = 1.0;
  board->points[10].y = 1.0;
  board->points[10].z = -1.0/ 10;

  board->points[11].x = -1.0;
  board->points[11].y = 1.0;
  board->points[11].z = -1.0/ 10;

  /* Donji deo, sa zadnje strane. */
  board->points[12].x = -1.0;
  board->points[12].y = 1.0;
  board->points[12].z = 0;

  board->points[13].x = -1.0;
  board->points[13].y = 1.0;
  board->points[13].z = -0.1;

  board->points[14].x = 1.0;
  board->points[14].y = 1.0;
  board->points[14].z = -0.1;

  board->points[15].x = 1.0;
  board->points[15].y = 1.0;
  board->points[15].z = 0;
}

/* Funkcija koja postavlja koordinate teksture. Pozvace se 4 puta, za svaki od 4 moguca slucaja. */
void setTextPoint(int i) {

  switch(i) {
    case 0:
      glTexCoord2f(0, 0);
      break;
    case 1:
      glTexCoord2f(0, 1);
      break;
    case 2:
        glTexCoord2f(1, 1);
        break;
    case 3:
        glTexCoord2f(1, 0);
        break;
  }
}

/* Funkcija koja iscrtava tablu. */
void drawBoard(struct Board board, GLuint* texture) {

  glBindTexture(GL_TEXTURE_2D, texture[0]);

  glPushMatrix();

  /* Rotira se za uglove table po x i y osi. */
  glRotatef(board.angles[0], 1, 0, 0);
  glRotatef(board.angles[1], 0, 1, 0);

  /* Iscrtava se gornji deo. */
  glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    for(int i = 0; i < 4; i++) {
      setTextPoint(i);
      glVertex3f(board.points[i].x, board.points[i].y, board.points[i].z);
    }
  glEnd();

  /* Iscrtava se donji deo. */
  glBegin(GL_POLYGON);
    for(int i = 4; i < 8; i++)
      glVertex3f(board.points[i].x, board.points[i].y, board.points[i].z);
  glEnd();

  glBegin(GL_POLYGON);
    for(int i = 8; i < 12; i++)
      glVertex3f(board.points[i].x, board.points[i].y, board.points[i].z);
  glEnd();

  glBegin(GL_POLYGON);
  for(int i = 12; i < 16; i++)
    glVertex3f(board.points[i].x, board.points[i].y, board.points[i].z);
  glEnd();

  /* Crta se ciljna zelena rupa. */

  glPushMatrix();
    GLfloat diffuse_coeffs[] = {0.3, 0.9, 0.3, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glDisable(GL_TEXTURE_2D);
      drawHole(board.level->targetHole.x*board.unit, board.level->targetHole.y*board.unit, board.unit, board.level->cylinder);
    glEnable(GL_TEXTURE_2D);
  glPopMatrix();

  /* Crtanje rupa na tabli. */
  for(int i = 0; i < board.level->numOfHoles; i++) {
    glPushMatrix();
     glDisable(GL_LIGHT0);
     glDisable(GL_TEXTURE_2D);
      drawHole(board.level->holes[i].x*board.unit, board.level->holes[i].y*board.unit, board.unit, board.level->cylinder);
     glEnable(GL_TEXTURE_2D);
     glEnable(GL_LIGHT0);
    glPopMatrix();
  }

  /* Crtanje zidova na tabli (oni su druge teksture). */
  glBindTexture(GL_TEXTURE_2D, texture[1]);
  drawWalls(board);

  glPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
}

/* Funkcija za iscrtavanje vertikalnih zidova. */
void drawWallV(struct Wall wall, double unit) {

  glTranslatef(0, 0, 0);
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 0);

  /* Zid odozgo.*/
  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit+unit, wall.one.y*unit, unit);
  setTextPoint(2);
    glVertex3f(wall.one.x*unit+unit, wall.two.y*unit + unit, unit);
  setTextPoint(3);
    glVertex3f(wall.one.x*unit, wall.two.y*unit + unit, unit);

  glEnd();

  /* Zid ssa leve strane.*/
  glBegin(GL_POLYGON);
  glNormal3f(0,0,0);

  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit, wall.two.y*unit+unit, unit);
  setTextPoint(2);
    glVertex3f(wall.one.x*unit, wall.two.y*unit+unit, 0);
  setTextPoint(3);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, 0);

  glEnd();

  /* Zid sa desne strane. */
  glBegin(GL_POLYGON);
  glNormal3f(0,0,0);

  setTextPoint(0);
    glVertex3f(wall.one.x*unit+unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit+unit, wall.two.y*unit+unit, unit);
  setTextPoint(2);
    glVertex3f(wall.one.x*unit+unit, wall.two.y*unit+unit, 0);
  setTextPoint(3);
    glVertex3f(wall.one.x*unit+unit, wall.one.y*unit, 0);

  glEnd();

  /* Prednji deo zida. */
  glBegin(GL_POLYGON);
  glNormal3f(0,0,0);

  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit+unit, wall.one.y*unit, unit);
  setTextPoint(2);
    glVertex3f(wall.one.x*unit+unit, wall.one.y*unit, 0);
  setTextPoint(3);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, 0);

  glEnd();

  /* Zadnji i donji deo zida se ne vidi, pa se ni ne crta. */
}

/* Funkcija za iscrtavanje horizontalnih zidova. */
void drawWallH(struct Wall wall, double unit) {

  glTranslatef(0, 0, 0);
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 0);

  /* Gornji deo zida. */
  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit, wall.one.y*unit+unit, unit);
  setTextPoint(2);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit+unit, unit);
  setTextPoint(3);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit, unit);

  glEnd();

  /* Prednji deo zida. */
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 0);

  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(3);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit, unit);
  setTextPoint(2);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit, 0);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, 0);

  glEnd();

  /* horizontalni zid sa leve strane. */
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 0);

  setTextPoint(0);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.one.x*unit, wall.one.y*unit+unit, unit);
  setTextPoint(2);
    glVertex3f(wall.one.x*unit, wall.one.y*unit+unit, 0);
  setTextPoint(3);
    glVertex3f(wall.one.x*unit, wall.one.y*unit, 0);

  glEnd();

  /* horizontalni zid sa desne strane. */
  glBegin(GL_POLYGON);
  glNormal3f(0, 0, 0);

  setTextPoint(0);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit, unit);
  setTextPoint(1);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit+unit, unit);
  setTextPoint(2);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit+unit, 0);
  setTextPoint(3);
    glVertex3f(wall.two.x*unit+unit, wall.one.y*unit, 0);

  glEnd();
}

/* Funkcija za iscrtavanje zidova, poziva prethodne dve funkcije. */
void drawWalls(struct Board board) {

  for(int i = 0; i < board.level->numOfWalls; i++) {
    if(board.level->walls[i].direction == 'H') {
      drawWallH(board.level->walls[i], board.unit);
    }
    if(board.level->walls[i].direction == 'V') {
      drawWallV(board.level->walls[i], board.unit);
    }
  }
}

/* Funkcija za iscrtavanje jedne rupe. */
void drawHole(double x, double y, double size, GLUquadricObj* cylinder) {

  /* Crta se krug, koristi se gluDisk. */
  glTranslatef(x+0.5*size, y+0.5*size, 0.00001);
  gluQuadricDrawStyle(cylinder, GLU_FILL);
  gluDisk(cylinder, 0, size, 30, 30 );
  glTranslatef(-x-0.5*size, -y-0.5*size, -0.00001);
}

/* Funkcija za pomeranje table. */
void moveBoard(struct Board* board, int where, double maxAngle, double step) {

  /* 4 slucaja za 4 komande na tastaturi.
     Ako se prosledi:   0 - tabla se rotira napred,
                        1 - tabla se rotira nazad,
                        2 - tabla se rotira na levo,
                        3 - tabla se rotira na desno. */
  switch(where) {
    case 0:
      if(board->angles[0] > -maxAngle)
        board->angles[0] -= step;
      break;

    case 1:
      if(board->angles[0] < maxAngle)
        board->angles[0] += step;
      break;

    case 2:
      if(board->angles[1] > -maxAngle)
        board->angles[1] -= step;
      break;

    case 3:
      if(board->angles[1] < maxAngle)
        board->angles[1] += step;
      break;
  }
}
