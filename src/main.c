#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

#include "ball.h"
#include "board.h"
#include "image.h"
/* Iz main.h se ucitava NUM_OF_UNITS. */
#include "main.h"

#define TIMER_ID 0
#define TIMER_INTERVAL 25

/* Imena fajlova za teksture table i zida. */
#define FILENAME0 "wood.bmp"
#define FILENAME1 "wood1.bmp"

/* Definisu se makroi:
   maksimalni ugao za koji se rotira tabla,
   korak za koji se rotira tabla. */
#define MAX_ANGLE 35.0
#define STEP 0.5

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_timer(int id);

/* Strukture table, loptice i nivoa(odredjuje raspored rupa i zidova). */
static struct Board board;
static struct Ball b;
static struct Level level;

/* Pocetno i proteklo vreme. */
int startTime = 0;
int passedTime = 0;

/* Fleg koji odredjuje da li je animacija u toku. */
static int animation_ongoing;

/* Identifikatori tekstura */
static GLuint names[2];

/* Promenljiva za ispis loga. */
static int debug = -1;

/* Promenljiva za ispis pauze. */
static int pause = -1;



/* Funkcija koja ispisuje tekst na ekranu. */
void text(const char* txt, int pos) {

  glDisable(GL_LIGHTING);

  /* 5 pozicija, za 5 razlicitih tekstova razlicitih boja. */
  switch(pos) {
    case 1:
      /* Za tekst koji se prikazuje pritiskom na taster 'l'. */
      glColor3f(1, 1, 0);
	    glRasterPos3f(-3, 3, -2);
      break;
    case 2:
      /* Za tekst koji se prikazuje kada loptica upadne u rupu. */
      glColor3f(1, 0, 0);
      glRasterPos3f(2.5, 2, -2);
      break;
    case 3:
      /* Za tekst koji se prikazuje kada se predje nivo. */
      glColor3f(0, 1, 0);
      glRasterPos3f(2.5, 2, -2);
      break;
    case 4:
      /* Za tekst koji se prikazuje pritiskom na taster 'p'. */
      glColor3f(1, 1, 0);
	    glRasterPos3f(2.5, 2, -2);
      break;
    case 5:
      /* Za tekst koji se prikazuje pre pokretanja animacije. */
      glColor3f(1, 1, 0);
      glRasterPos3f(-2, -1.7, -2);
      break;
  }

  glutBitmapString(GLUT_BITMAP_HELVETICA_18, txt);
	glEnable(GL_LIGHTING);
}

int main(int argc, char **argv) {

  /* Inicijalizuje se GLUT. */
  Image *image;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  /* Kreira se prozor. */
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);

  /* Registruju se callback funkcije. */
  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  glutDisplayFunc(on_display);

  /* Obavlja se OpenGL inicijalizacija. */
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);

  /* Ukljucuju se teksture. */
  glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,
	          GL_TEXTURE_ENV_MODE,
	          GL_REPLACE);

  glGenTextures(2, names);
	image = image_init(0, 0);

	/* Kreira se prva tekstura. */
	image_read(image, (char*)FILENAME0);

	/* Generisu se identifikatori tekstura. */
	glBindTexture(GL_TEXTURE_2D, names[0]);
	glTexParameteri(GL_TEXTURE_2D,
		              GL_TEXTURE_WRAP_S,
                  GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,
		              GL_TEXTURE_WRAP_T,
                  GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,
		              GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
		              GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               image->width, image->height, 1,
               GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

  /* Kreira se druga tekstura. */
  image_read(image, (char*)FILENAME1);

  glBindTexture(GL_TEXTURE_2D, names[1]);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_S,
                  GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_T,
                  GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               image->width, image->height, 1,
               GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	/* Unistava se objekat za citanje tekstura iz fajla. */
	image_done(image);

  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

  /* Na pocetku nije ukljucena animacija. */
  animation_ongoing = 0;

  /* Quadric object za crtanje rupe, inicijalizuje se samo jednom. */
  GLUquadricObj* holeObject = gluNewQuadric();

  /* Pozivaju se f-je za inicijalizaciju. */
  initLevel(&level, holeObject);
  initBoard(&board, &level, NUM_OF_UNITS);
  initBall(&b, 1.0/ NUM_OF_UNITS, level.ballPos);

  /* Program ulazi u glavnu petlju. */
  glutMainLoop();

  return 0;
}

static void on_keyboard(unsigned char key, int x, int y) {

  switch (key) {
    case 27:
      /* Zavrsava se program. */
      exit(0);
      break;

    case 'r':
    case 'R':
      /* Ako je igra zavrsena, pritiskom 'R' se ponovo inicijalizuju tabla, loptica i rupe/zidovi. */
      if(!animation_ongoing) {
        initBoard(&board, &level, NUM_OF_UNITS);
        initBall(&b, 1.0/ NUM_OF_UNITS, level.ballPos);
      }

    case 'g':
    case 'G':
      /* Pokrece se animacija. */
      if (!animation_ongoing && b.endGame != 1 && b.isInHole != 1) {
          glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
          startTime = glutGet(GLUT_ELAPSED_TIME);
          animation_ongoing = 1;
          pause = -1;
      }
      break;

    case 'p':
    case 'P':
      /* Zaustavlja se animacija. */
      if(animation_ongoing) {
        animation_ongoing = 0;
        pause = -pause;
      }
      break;

    case 'w':
    case 'W':
      /* Tabla se rotira na gore. */
      moveBoard(&board, 0, MAX_ANGLE, STEP);
      break;

    case 'a':
    case 'A':
      /* Tabla se rotira na levo. */
      moveBoard(&board, 2, MAX_ANGLE, STEP);
      break;

    case 's':
    case 'S':
      /* Tabla se rotira na dole. */
      moveBoard(&board, 1, MAX_ANGLE, STEP);
      break;

    case 'd':
    case 'D':
      /* Tabla se rotira na desno. */
      moveBoard(&board, 3, MAX_ANGLE, STEP);
      break;

    case 'l':
    case 'L':
      /* Log - prikazuje ugao, poziciju loptice i da li je loptica u rupi. */
      debug = -debug;
      break;
  }
}

static void on_reshape(int width, int height) {

    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 10);
}

static void on_timer(int id) {

    if (id != TIMER_ID) return;

    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    /* passedTime je vreme proteklo izmedju dva poziva funkcije. */
    passedTime = currentTime - startTime;
    startTime = currentTime;

    /*  Ako je animacija u toku, pozivamo f-ju koja pomera lopticu. */
    if(animation_ongoing) {
      nextPos(&b, board, passedTime);
    }

    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if(animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_display(void) {

    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 1, 1, 1, 0 };

    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };

    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.4, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.6, 0.6, 0.85, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 0.6, 0.6, 0.6, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 60;

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Podesava se pogled odozgo. */
    gluLookAt(0, -0.5, 2.5, 0, 0, 0, 0, 1, 0);

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /* Pozivaju se f-je za crtanje table i crtanje loptice. */
    drawBoard(board, names);
    drawBall(b, board);

    /* Koristi se za ispis teksta na ekranu. */
    char str[2000];

    /* Tekst koji se prikazuje samo kada se tek pokrene program. */
    if(!animation_ongoing && b.endGame != 1 && pause != 1 && b.isInHole != 1) {
      sprintf(str, "PRESS G TO START ANIMATION\n");
      text(str, 4);
    }

    /* Dok je animacija u toku, na ekranu stoji tekst sa instrukcijama. */
    if(animation_ongoing) {
      sprintf(str, "Use W A S D to rotate the board.\n P - pause\n L - show/hide log\n");
      text(str, 4);
    }

    /* Ako je pritisnut taster 'l' ili 'L' ispisuje se trenutni ugao table,
       koordinate loptice, i da li je loptica u rupi. Korisno za debag. */
    if(debug == 1) {
    sprintf(str, "Angle = %.2f %.2f    in Hole = %d   BALL = (%.6f, %.6f)",
            board.angles[0], board.angles[1], b.isInHole, b.pos[0], b.pos[1]);
    text(str, 1);
    }

    /* Ako je pritisnut taster 'p' ili 'P' ispisuje se PAUSED na ekranu. */
    if(pause == 1) {
      sprintf(str, "PAUSED\nPress 'g' to continue.");
      text(str, 4);
    }

    /* Ako je loptica upala, zaustavlja se animacija, loptica je u rupi,
       ispisuje se tekst GAME OVER. */
    if(b.pos[2] < -0.07) {
      animation_ongoing = 0;
      b.isInHole = 1;
      sprintf(str, "GAME OVER\n YOU LOSE!\nPress 'r' to restart the game.");
      text(str, 2);
    }

    /* Ako je loptica stigla do kraja, zaustavlja se animacija, ispisuje
       se tekst LEVEL COMPLETE. */
    if(b.endGame == 1) {
      animation_ongoing = 0;
      sprintf(str, "LEVEL COMPLETE!\nPress 'r' to play again.");
      text(str,3);
    }

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}
