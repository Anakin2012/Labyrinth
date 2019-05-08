#include "board.h"
#include "ball.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>
#include <math.h>

/* Za pretvaranje stepena u radijane. */
#define DEGREE_TO_RAD 0.0174533

/* Ubrzanje Zemljine teze. */
#define GRAVITY 9.81

/* Zbog zaokruzivanja, minimalna distanca koja se racuna kao kolizija. (0 ne radi). */
#define MIN_DIST 0.0000001

/* Kolizija vertikalni zid, vraca -1 ako je sa desne strane, +1 sa leve. */
int collisionX(struct Ball ball, struct Wall wall) {
  int levo;

  if(ball.pos[0] < wall.one.x/NUM_OF_UNITS)
    levo = 1;
  else
    levo = -1;

  /* Prvi uslov - ako je udarila u zid, rastojanje centra loptice i zida je manje od poluprecnika loptice.
     Drugi uslov - da je pozicija loptice + velicina loptice samo u opsegu gde se prostire zid. */
  if(levo == 1 && fabs(ball.pos[0] - wall.one.x/NUM_OF_UNITS) < ball.size &&
  ((ball.oldPos[1]+ball.size - wall.one.y/NUM_OF_UNITS)> MIN_DIST  && ball.oldPos[1]-ball.size < wall.two.y/NUM_OF_UNITS)) {
    return levo;
  }

  /* Isto samo sa desne strane. */
  if(levo == -1 && fabs(ball.pos[0] - (wall.one.x/NUM_OF_UNITS + 1/NUM_OF_UNITS)) < ball.size &&
  ((ball.oldPos[1]+ball.size - wall.one.y/NUM_OF_UNITS) > MIN_DIST && ball.oldPos[1]-ball.size < wall.two.y/NUM_OF_UNITS)) {
    return levo;
  }

  /* Vraca 0 ako nije udarila u zid. */
  return 0;
}

/* Kolizija horizontalan zid, vraca 1 sa gornje, -1 sa donje strane. */
int collisionY(struct Ball ball, struct Wall wall) {
  int gore = 1;

  if(ball.pos[1] < wall.one.y/NUM_OF_UNITS)
    gore = 1;
  else
    gore = -1;

  /* Prvi uslov - ako je udarila u zid, rastojanje centra loptice i zida je manje od poluprecnika loptice.
     Drugi uslov - Ako je u loptica u opsegu gde se zid prostire.  */
  if(gore == 1 && fabs(ball.pos[1] - wall.one.y/NUM_OF_UNITS) < ball.size &&
  ((((ball.oldPos[0]+ball.size-MIN_DIST) > wall.one.x/NUM_OF_UNITS)) && ((ball.oldPos[0]-ball.size) < wall.two.x/NUM_OF_UNITS))) {
    return gore;
  }

  /* Isto, samo za donju stranu. */
  if(gore == -1 && fabs(ball.pos[1] - (wall.one.y/NUM_OF_UNITS + 1/NUM_OF_UNITS)) < ball.size &&
    ((((ball.oldPos[0]+ball.size-MIN_DIST) > wall.one.x/NUM_OF_UNITS)) && ((ball.oldPos[0]-ball.size) < wall.two.x/NUM_OF_UNITS))) {
    return gore;
  }

  /* Vraca 0 ako nije udarila u zid. */
  return 0;
}

/* Funkcija koja proverava da li je loptica upala u rupu, vraca 1 ako jeste. */
int testHole(struct Ball* b, struct Board board, struct Point* p) {

  /* Za svaku rupu racuna rastojanje od centra loptice, i ako je manje od poluprecnika
     loptice znaci da je upala, dodeljuje tacki koordinate te rupe i vraca 1. */
  for(int i = 0; i < board.level->numOfHoles; i++) {

    double dist = (board.level->holes[i].x/NUM_OF_UNITS-b->pos[0])*(board.level->holes[i].x/NUM_OF_UNITS-b->pos[0]) +
                  (board.level->holes[i].y/NUM_OF_UNITS-b->pos[1])*(board.level->holes[i].y/NUM_OF_UNITS-b->pos[1]);
    dist = sqrt(dist);

    if(dist < b->size) {
      p->x = board.level->holes[i].x/NUM_OF_UNITS;
      p->y = board.level->holes[i].y/NUM_OF_UNITS;
      return 1;
    }
  }

  /* Vraca 0 ako ne upadne u rupu. */
  return 0;
}

/* Funkcija koja proverava da li je loptica stigla do ciljne rupe, radi isto kao prethodna f-ja. */
int testTargetHole(struct Ball* b, struct Board board, struct Point* p) {

    double dist = (board.level->targetHole.x/NUM_OF_UNITS-b->pos[0])*(board.level->targetHole.x/NUM_OF_UNITS-b->pos[0]) +
                  (board.level->targetHole.y/NUM_OF_UNITS-b->pos[1])*(board.level->targetHole.y/NUM_OF_UNITS-b->pos[1]);
    dist = sqrt(dist);

    if(dist < b->size) {
      p->x = board.level->targetHole.x/NUM_OF_UNITS;
      p->y = board.level->targetHole.y/NUM_OF_UNITS;
      return 1;
    }

  return 0;
}

/* Funkcija koja racuna brzinu i ubrzanje loptice. */
void calcAcc(struct Ball* b, struct Board board, double time) {

  /* Formule za racunanje ubrzanja i brzine na strmoj ravni. */
  b->a[1] = -sinf(board.angles[0]*DEGREE_TO_RAD)*b->mass * GRAVITY * 1 / 600.0f;
  b->a[0] = sinf(board.angles[1]*DEGREE_TO_RAD)*b->mass * GRAVITY * 1 / 600.0f;

  /* Ako upadne, onda racunamo i ubrzanje po z (ide na dole). */
  if(b->isInHole == 1) {
    /* p1 - cetar lopte , p2 - centar rupe. */
    struct Point p1;
    struct Point p2;
    p1.x = b->pos[0];
    p1.y = b->pos[1];
    p2 = b->holeCenter;

    /* s i c su rastojanja od centra loptice i centra rupe, po x  i y osi. d je rastojanje izmedju te dve  tacke. */
    double s = (p2.x - p1.x);
    double c = (p2.y - p1.y);
    double d = sqrt(s*s + c*c);

    /* Ako d nije nula, deli se sa d i dobijaju se vrednosti izmedju 0 i 1. */
    if(d != 0) {
      s = s/d;
      c = c/d;
    }

    /* Mnozi se s i c sa masom i ubrzanjem zemljine teze. */
    b->a[0] = 3*s*b->mass * GRAVITY * 1 / 600.0f;
    b->a[1] = 3*c*b->mass * GRAVITY * 1 / 600.0f;
    b->a[2] = -b->mass * GRAVITY * 1 / 600.0f;

    /* Brzina se postavlja na 0. */
    b->v[0] = 0;
    b->v[1] = 0;
  }
  /* Inace, ako nije u rupi, ne ide na dole. */
  else {
    b->a[2] = 0.0;
  }

  /* Brzina loptice. */
  b->v[0] = b->v[0] + b->a[0] * time;
  b->v[1] = b->v[1] + b->a[1] * time;
  b->v[2] = b->v[2] + b->a[2] * time;
}

/* Funkcija za crtanje loptice. */
void drawBall(struct Ball b, struct Board board) {

    glPushMatrix();
      glTranslatef(0, 0, 0);
      GLfloat diffuse_coeffs[] = {0.5, 0.5, 0.5, 1};
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
      glRotatef(board.angles[0], 1, 0, 0);
      glRotatef(board.angles[1], 0, 1, 0);
      glRotatef(board.angles[2], 0, 0, 1);

      glTranslatef(b.pos[0],b.pos[1], b.pos[2]);
      glutSolidSphere(b.size, 20, 20);
    glPopMatrix();
}

/* Funkcija za inicijalizaciju loptice. */
void initBall(struct Ball* b, double size, struct Point pos) {

  /* Na pocetku loptica nije ni u jednoj rupi (ni u ciljnoj). */
  b->isInHole = 0;
  b->endGame = 0;

  /* Pocetna pozicija i velicina loptice. */
  b->pos[0] = pos.x;
  b->pos[1] = pos.y;
  b->pos[2] = size;
  b->size = size;

  /* Brzina je na pocetku 0, loptica stoji. */
  b->v[0] = 0;
  b->v[1] = 0;
  b->v[2] = 0;

  /* Ubrzanje je na pocetku takodje 0. */
  b->a[0] = 0;
  b->a[1] = 0;
  b->a[2] = 0;

  /* Masa loptice se postavlja na 1. */
  b->mass = 1.0;
}

/* Funkcija za pomeranje loptice. */
void nextPos(struct Ball* b, struct Board board, int time) {

  /* t je proteklo vreme od prethodne promene pozicije.
     elastic je mera zadrzavanja brzine po udarcu u zid. */
  double t = time/100.0;
  double elastic = 0.3;

  /* Racunaju se ubrzanja i brzine. */
  calcAcc(b, board, t);

  /* Pamti se prethodna pozicija loptice. */
  b->oldPos[0] = b->pos[0];
  b->oldPos[1] = b->pos[1];
  b->oldPos[2] = b->pos[2];

  /* Predjeni put = brzina * proteklo vreme. */
  b->pos[0] += b->v[0]*t;
  b->pos[1] += b->v[1]*t;
  b->pos[2] += b->v[2]*t;

  /* Za svaki pocetak i kraj zida, racuna se kolizija. */
  for(int i = 0; i < board.level->numOfCorners; i++) {
    if(board.level->corners[i].direction == 'V') {
      /* Ako je udarila desno. */
      if(collisionX(*b, board.level->corners[i]) == 1) {

        b->v[0] = -b->v[0]*elastic;
        b->pos[0] = board.level->corners[i].one.x/NUM_OF_UNITS - (b->size);
        b->a[0] = 0;
      }
      /* Ako je udarila levo. */
      else if(collisionX(*b, board.level->corners[i]) == -1) {
        b->v[0] = -b->v[0]*elastic;
        b->a[0] = 0;
        b->pos[0] = board.level->corners[i].one.x/NUM_OF_UNITS + 1/NUM_OF_UNITS + b->size;
      }
    }

    else if (board.level->corners[i].direction == 'H') {
      /* Ako je udarila gore. */
      if(collisionY(*b, board.level->corners[i]) == 1) {
        b->v[1] = -b->v[1]*elastic;
        b->pos[1] = board.level->corners[i].one.y/NUM_OF_UNITS - (b->size);
        b->a[1] = 0;
      }
      /* Ako je udarila dole. */
      else if(collisionY(*b, board.level->corners[i]) == -1) {
        b->v[1] = -b->v[1]*elastic;
        b->a[1] = 0;
        b->pos[1] = board.level->corners[i].one.y/NUM_OF_UNITS + 1/NUM_OF_UNITS + b->size;
      }
    }
  }

  /* Za svaki zid poziva se funkcija za koliziju, u zavisnosti od pravca zida.
     Menjaju se brzina, pozicija i ubrzanje loptice. */
  for(int i = 0; i < board.level->numOfWalls; i++) {
    if(board.level->walls[i].direction == 'V') {
      /* Ako je udarila desno. */
      if(collisionX(*b, board.level->walls[i]) == 1) {
        b->v[0] = -b->v[0]*elastic;
        b->pos[0] = board.level->walls[i].one.x/NUM_OF_UNITS - (b->size);
        b->a[0] = 0;
      }
      /* Ako je udarila levo. */
      else if(collisionX(*b, board.level->walls[i]) == -1) {
        b->v[0] = -b->v[0]*elastic;
        b->a[0] = 0;
        b->pos[0] = board.level->walls[i].one.x/NUM_OF_UNITS + 1/NUM_OF_UNITS + b->size;
      }
    }

    else if (board.level->walls[i].direction == 'H') {
      /* Ako je udarila gore. */
      if(collisionY(*b, board.level->walls[i]) == 1) {
        b->v[1] = -b->v[1]*elastic;
        b->pos[1] = board.level->walls[i].one.y/NUM_OF_UNITS - (b->size);
        b->a[1] = 0;
      }
      /* Ako je udarila dole. */
      else if(collisionY(*b, board.level->walls[i]) == -1) {
        b->v[1] = -b->v[1]*elastic;
        b->a[1] = 0;
        b->pos[1] = board.level->walls[i].one.y/NUM_OF_UNITS + 1/NUM_OF_UNITS + b->size;
      }
    }
  }

  /* Pravi se tacka, proverava se da li je loptica upala u rupu, i ako jeste,
     dodeljuje se ta tacka centru rupe i menja se isInHole na 1. */
  struct Point p;
  int k = testHole(b, board, &p);
  if(k == 1) {
    b->isInHole = 1;
    b->holeCenter = p;
  }
  else {
    b->isInHole = 0;
  }

  /* Proverava se da li je loptica stigla do ciljne rupe, ako jeste, endGame = 1 i igra je predjena. */
  int end = testTargetHole(b, board, &p);
  if(end) {
    b->endGame = 1;
  }
}
