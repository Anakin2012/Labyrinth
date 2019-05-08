#ifndef POINT
#define POINT

/* Struktura Point koja predstavlja tacku sa 3 koordinate,
   koristi se pri crtanju objekata. */
struct Point {
  float x;
  float y;
  float z;
};

/* Struktura Wall koja predstavlja zid odredjen
   dvema tackama, i karakterom koji odredjuje pravac
   zida, moze biti V (vertical) ili H (horizontal). */
struct Wall {
  struct Point one;
  struct Point two;
  char direction;
};

#endif
