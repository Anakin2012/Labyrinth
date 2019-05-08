#include <stdio.h>

#include "level.h"
#include "main.h"

/* Funkcija koja inicijalizuje izgled nivoa, moze se menjati
   unosom stringova po zelji, i tako praviti razliciti nivoi. */
void initLevel(struct Level* l, GLUquadricObj* holeObject) {

  /* Izgled table je predstavljen nizom 40 stringova:
     V - oznacava pocetak vertikalnog zida.
     H - oznacava pocetak horizontalnog zida.
     _ - oznacava sredinu zida.
     E - oznacava kraj zida.
     0 - oznacava rupu na tabli.
     C - oznacava rupu u koju treba da se dovede loptica, ciljna rupa.
     L - oznacava lopticu.
     Na osnovu pozicija ovih karaktera u nizu se iscrtavaju rupe
     i zidovi na tabli.
  */

  /* Trenutno ima samo jedan nivo, za konacnu verziju bi trebalo napraviti ucitavanje
     vise nivoa iz datoteke u ovom formatu. */
  l->rows[0] = "V______________________________________E";
  l->rows[1] = "H                      H               H";
  l->rows[2] = "_ 0                    _             0 _";
  l->rows[3] = "_                    L _               _";
  l->rows[4] = "_                      _               _";
  l->rows[5] = "_                      _       H       _";
  l->rows[6] = "_        V____________EE       _       _";
  l->rows[7] = "_        H                     _       _";
  l->rows[8] = "_        _                     _       _";
  l->rows[9] = "_    V__E_                     _       _";
  l->rows[10]= "_        _                   0 _       _";
  l->rows[11]= "_        _                     _       _";
  l->rows[12]= "_        _     V______________E_       _";
  l->rows[13]= "_        _   0 H               _       _";
  l->rows[14]= "_        _     _               _    V_E_";
  l->rows[15]= "_        _  V_E_               _       _";
  l->rows[16]= "_        _     _      V_EH     _     0 _";
  l->rows[17]= "_ 0      _     _         _     _       _";
  l->rows[18]= "_        _     _         _     _       _";
  l->rows[19]= "_V__E    _     _       C _     _       _";
  l->rows[20]= "_        _     _         _     _       _";
  l->rows[21]= "_        _     _  0      _     _       _";
  l->rows[22]= "_        _     _         _     _       _";
  l->rows[23]= "_        _     _         _     _       _";
  l->rows[24]= "_        _     EV________E     _       _";
  l->rows[25]= "_        _                     _       _";
  l->rows[26]= "_        _                     _       _";
  l->rows[27]= "_        _                     _V_E    _";
  l->rows[28]= "_    V__E_                     _       _";
  l->rows[29]= "_        _  0               0  _ 0     _";
  l->rows[30]= "_      0 _                     E       _";
  l->rows[31]= "_        EV____________________E       _";
  l->rows[32]= "_           H               H          _";
  l->rows[33]= "_        0  _             0 _          _";
  l->rows[34]= "_           E               E          _";
  l->rows[35]= "_                                      _";
  l->rows[36]= "_                  H                   _";
  l->rows[37]= "_                  _                   _";
  l->rows[38]= "E                  E                   E";
  l->rows[39]= "V______________________________________E";

  initHoles(l, holeObject);
  initWalls(l);
}

/* Funkcija koja inicijalizuje rupe. */
void initHoles(struct Level* l, GLUquadricObj* holeObject) {

  int z = 0;

  l->cylinder = holeObject;
  
  /* Prolazi se kroz svaku vrstu i kolonu i ako se naidje na nulu,
  *  dodeljuju se koordinate rupa. */
  for(int i = 0; i < 40; i++) {
    for(int k = 0; k < 40; k++) {

      if(l->rows[i][k] == 'L') {
        l->ballPos.x = (i-NUM_OF_UNITS)/NUM_OF_UNITS;
        l->ballPos.y = (k-NUM_OF_UNITS)/NUM_OF_UNITS;
      }

      /* Obicna rupa. */
      if(l->rows[i][k] == '0') {

        l->holes[z].x = i-NUM_OF_UNITS;
        l->holes[z].y = k-NUM_OF_UNITS;
        z++;
      }
      /* Ciljna rupa. */
      if(l->rows[i][k] == 'C') {
        l->targetHole.x = i-NUM_OF_UNITS;
        l->targetHole.y = k-NUM_OF_UNITS;
      }
    }
  }

  /* Ukupan broj rupa. */
  l->numOfHoles = z;
}

/* Funkcija koja inicijalizuje zidove. */
void initWalls(struct Level* l) {

  int z = 0;
  int c = 0;

  /* Dodeljuju se koordinate i pravac vertikalnih zidova. */
  for(int i = 0; i < 40; i++) {
    for(int k = 0; k < 40; k++) {
      if(l->rows[i][k] == 'V') {

        /* Ako naidje navertikalni id sa odozgo ili odozdo, ponasa se kao da udara u horizontalni. */
        l->corners[c].direction = 'H';
        l->corners[c].one.x = i-NUM_OF_UNITS;
        l->corners[c].one.y = k-NUM_OF_UNITS;
        l->corners[c].two.x = i - NUM_OF_UNITS;
        l->corners[c].two.y = k-NUM_OF_UNITS;
        c++;


        l->walls[z].direction = 'V';
        l->walls[z].one.x = i-NUM_OF_UNITS;
        l->walls[z].one.y = k-NUM_OF_UNITS;
        for(int m = k+1; m < 40; m++) {
          if(l->rows[i][m] == 'E') {

            l->corners[c].direction = 'H';
            l->corners[c].one.x = i-NUM_OF_UNITS;
            l->corners[c].one.y = m-NUM_OF_UNITS;
            l->corners[c].two.x = i - NUM_OF_UNITS;
            l->corners[c].two.y = m-NUM_OF_UNITS;
            c++;

            l->walls[z].two.x = i-NUM_OF_UNITS;
            l->walls[z].two.y = m-NUM_OF_UNITS;
            z++;
            break;
          }
        }
      }
    }
  }

  /* Koordinate i pravac horizontalnih zidova. */
  for(int i = 0; i < 40; i++) {
    for(int k = 0; k < 40; k++) {
      if(l->rows[i][k] == 'H') {

        /* Ako naidje na horizontalni zid sa strane, ponasa se kao da udara u vertikalni. */
        l->corners[c].direction = 'V';
        l->corners[c].one.x = i-NUM_OF_UNITS;
        l->corners[c].one.y = k-NUM_OF_UNITS;
        l->corners[c].two.x = i - NUM_OF_UNITS;
        l->corners[c].two.y = k-NUM_OF_UNITS;
        c++;

        l->walls[z].direction = 'H';
        l->walls[z].one.x = i-NUM_OF_UNITS;
        l->walls[z].one.y = k-NUM_OF_UNITS;
        for(int m = i+1; m < 40; m++) {
          if(l->rows[m][k] == 'E') {

            l->corners[c].direction = 'V';
            l->corners[c].one.x = m-NUM_OF_UNITS;
            l->corners[c].one.y = k-NUM_OF_UNITS;
            l->corners[c].two.x = m - NUM_OF_UNITS;
            l->corners[c].two.y = k-NUM_OF_UNITS;
            c++;

            l->walls[z].two.x = m-NUM_OF_UNITS;
            l->walls[z].two.y = k-NUM_OF_UNITS;
            z++;
            break;
          }
        }
      }
    }
  }

  /* Ukupan broj zidova.*/
  l->numOfWalls = z;
  l->numOfCorners = c;
}
