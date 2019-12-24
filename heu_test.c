#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h> 

#define FACES  24     // Sum of cube faces
#define IDLEN  25     // uchar[24] + \0
#define OPRNUM  6
#define UCHAR_MAX 9

typedef unsigned char uchar; // uchar
uchar heu_map[FACES][FACES];
// int heu_map[FACES][FACES] = {
//   // 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N
//   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 0
//   {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 1
//   {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 2
//   {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 3
//   {1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 4
//   {1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 5
//   {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 6
//   {1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 7
//   {1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 8
//   {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 9
//   {1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1}, // A
//   {1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1}, // B
//   {1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1}, // C
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1}, // D
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1}, // E
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1}, // F
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1}, // G
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1}, // H
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1}, // I
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1}, // J
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1}, // K
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1}, // L
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1}, // M
//   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}, // N
// };

/* GLOBAL */
uchar charset[FACES] = {
  0x30, // '0'
  0x31, // '1'
  0x32, // '2'
  0x33, // '3'
  0x34, // '4'
  0x35, // '5'
  0x36, // '6'
  0x37, // '7'
  0x38, // '8'
  0x39, // '9'
  0x41, // 'A'
  0x42, // 'B'
  0x43, // 'C'
  0x44, // 'D'
  0x45, // 'E'
  0x46, // 'F'
  0x47, // 'G'
  0x48, // 'H'
  0x49, // 'I'
  0x4A, // 'J'
  0x4B, // 'K'
  0x4C, // 'L'
  0x4D, // 'M'
  0x4E, // 'N'
};

int h ( uchar *node ); // Heuristics
void print_cube ( uchar *cube );
uchar* rotateRU ( uchar *cube );
uchar* rotateRD ( uchar *cube );
uchar* rotateLU ( uchar *cube );
uchar* rotateLD ( uchar *cube );
uchar* rotateBR ( uchar *cube );
uchar* rotateBL ( uchar *cube );
void init_heu_map( void );
void print_cube_d( uchar *cube );

int main ( void ) {
  int i,j,k,times;
  uchar *start, goal[IDLEN];
  uchar *tmp[6], *tmp_cube;
  uchar *(*rotate[])(uchar*) = {
    rotateRU, rotateRD,
    rotateLU, rotateLD,
    rotateBR, rotateBL
  };

  // for (i=0;i<FACES;i++) {
  //   printf("%c [%u->%d:%d]:",
  //     charset[i],charset[i],
  //     charset[i]-'0',charset[i]-'A'+10);
  //   if ( isalnum(charset[i]) ) printf(" isalnum");
  //   if ( isalpha(charset[i]) ) printf(" ABC");
  //   if ( isdigit(charset[i]) ) printf(" 123");
  //   printf("\n");
  // }

  init_heu_map();

  /* initialize start cube */
  srand((unsigned)time(NULL));
  start = (uchar*)malloc(sizeof(uchar)*IDLEN);
  for (i=0;i<FACES;i++) start[i] = charset[i];
  times = rand() % 14;
  printf("rotate: %d\n",times);
  printf("[GOAL]->");
  for (i=0;i<times;i++){
    k = rand()%OPRNUM;
    switch (k) {
      case 0 : printf("RU->"); break;
      case 1 : printf("RD->"); break;
      case 2 : printf("LU->"); break;
      case 3 : printf("LD->"); break;
      case 4 : printf("BR->"); break;
      case 5 : printf("BL->"); break;
    }
    tmp_cube = start;
    start = (*rotate[k])(tmp_cube);
    free(tmp_cube);
  }
  start[FACES] = '\0';

  /* initialize goal cube */
  for (i=0;i<FACES;i++) goal[i] = charset[i];
  goal[FACES] = '\0';
  printf("[START]\n");
  print_cube(start);

  // for (i=0;i<FACES;i++) print_cube_d(heu_map[i]);

  printf("h(start):%d\n",h(start));
  printf("h(goal):%d\n",h(goal));

  // tmp[0] = rotateRU( goal );
  // tmp[1] = rotateRD( goal );
  // tmp[2] = rotateLU( goal );
  // tmp[3] = rotateLD( goal );
  // tmp[4] = rotateBR( goal );
  // tmp[5] = rotateBL( goal );

  // print_cube(goal);
  // printf("%s -(RU)-> %s h():%d\n",goal,tmp[0],h(tmp[0]) );
  // print_cube(tmp[0]);
  // printf("%s -(RD)-> %s h():%d\n",goal,tmp[1],h(tmp[1]) );
  // print_cube(tmp[1]);
  // printf("%s -(LU)-> %s h():%d\n",goal,tmp[2],h(tmp[2]) );
  // print_cube(tmp[2]);
  // printf("%s -(LD)-> %s h():%d\n",goal,tmp[3],h(tmp[3]) );
  // print_cube(tmp[3]);
  // printf("%s -(BR)-> %s h():%d\n",goal,tmp[4],h(tmp[4]) );
  // print_cube(tmp[4]);
  // printf("%s -(BL)-> %s h():%d\n",goal,tmp[5],h(tmp[5]) );
  // print_cube(tmp[5]);

  return 0;
}

int h ( uchar *cube )
{
  int i,sum = 0;
  uchar c,offset;

  for (i=0;i<FACES;i++){
    c = cube[i];
    if ( isdigit(c) ) offset = '0';
    else offset = 'A' - 10;
    sum += heu_map[c - offset][i];
    printf("%d ",c - offset);
  }
  // printf("\n");
  return sum;
}

/* rotateXXs */
uchar* rotateRU ( uchar *cube )
{
  int i,offset = 12;
  int key[4] = {5,9,17,21};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[i]] = cube[key[(i+3)%4]];
    tmp[key[i]+1] = cube[key[(i+3)%4]+1];
  }
  for (i=0;i<4;i++) {
    tmp[i+offset] = cube[(i+3)%4+offset];
  }

  return tmp;
}
uchar* rotateRD ( uchar *cube )
{
  int i,offset = 12;
  int key[4] = {5,9,17,21};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[(i+3)%4]] = cube[key[i]];
    tmp[key[(i+3)%4]+1] = cube[key[i]+1];
  }
  for (i=0;i<4;i++) {
    tmp[(i+3)%4+offset] = cube[i+offset];
  }

  return tmp;
}
uchar* rotateLU ( uchar *cube )
{
  int i,offset = 16;
  int key[4] = {1,8,12,22};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[i]] = cube[key[(i+3)%4]];
    tmp[key[i]+1] = cube[key[(i+3)%4]+1];
  }
  for (i=0;i<4;i++) {
    tmp[(i+3)%4+offset] = cube[i+offset];
  }

  return tmp;
}
uchar* rotateLD ( uchar *cube )
{
  int i,offset = 16;
  int key[4] = {1,8,12,22};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[(i+3)%4]] = cube[key[i]];
    tmp[key[(i+3)%4]+1] = cube[key[i]+1];
  }
  for (i=0;i<4;i++) {
    tmp[i+offset] = cube[(i+3)%4+offset];
  }

  return tmp;
}
uchar* rotateBR ( uchar *cube )
{
  int i,offset = 20;
  int key[4] = {0,6,13,16};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[i]] = cube[key[(i+3)%4]];
    tmp[key[i]+1] = cube[key[(i+3)%4]+1];
  }
  for (i=0;i<4;i++) {
    tmp[i+offset] = cube[(i+3)%4+offset];
  }

  return tmp;
}
uchar* rotateBL ( uchar *cube )
{
  int i,offset = 20;
  int key[4] = {0,6,13,16};
  uchar *tmp;
  tmp = (uchar*)malloc(sizeof(uchar)*IDLEN);
  memcpy(tmp,cube,sizeof(uchar)*IDLEN);
  /* rotate cube */
  for (i=0;i<4;i++) {
    tmp[key[(i+3)%4]] = cube[key[i]];
    tmp[key[(i+3)%4]+1] = cube[key[i]+1];
  }
  for (i=0;i<4;i++) {
    tmp[(i+3)%4+offset] = cube[i+offset];
  }

  return tmp;
}

void print_cube( uchar *cube )
{
  // printf("  cube[%s]\n",cube);
  printf("        [%c][%c]              \n",cube[0],cube[1]);
  printf("        [%c][%c]              \n",cube[3],cube[2]);
  printf("  [%c][%c][%c][%c][%c][%c][%c][%c]  \n",cube[7],cube[4],cube[11],cube[8],cube[19],cube[16],cube[23],cube[20]);
  printf("  [%c][%c][%c][%c][%c][%c][%c][%c]  \n",cube[6],cube[5],cube[10],cube[9],cube[18],cube[17],cube[22],cube[21]);
  printf("        [%c][%c]              \n",cube[15],cube[12]);
  printf("        [%c][%c]              \n",cube[14],cube[13]);
  return;
}

void init_heu_map( void )
{
  int i,j,k,s;
  uchar *tmp[6];
  uchar before[FACES];
  uchar *(*rotate[])(uchar*) = {
    rotateRU, rotateRD,
    rotateLU, rotateLD,
    rotateBR, rotateBL
  };

  for (i=0;i<FACES;i++){
    for (j=0;j<FACES;j++){
      heu_map[i][j] = UCHAR_MAX;
    }
  }
  for (i=0;i<FACES;i++) before[i] = i;
  for (k=0;k<6;k++) tmp[k] = (*rotate[k])(before);

  /* 0 step */
  for (i=0;i<FACES;i++) heu_map[i][i] = 0;
  /* 1-4 step */
  for (s=0;s<4;s++) {
    for (j=0;j<FACES;j++) {
      for (i=0;i<FACES;i++) {
        for (k=0;k<OPRNUM;k++) {
          if (   heu_map[i][j] == UCHAR_MAX
              && heu_map[i][tmp[k][j]] == s ) {
            heu_map[i][j] = s + 1;
          }
        }
      }
    }
  }
  return;
}

void print_cube_d( uchar *cube )
{
  printf("        [%d][%d]              \n",cube[0],cube[1]);
  printf("        [%d][%d]              \n",cube[3],cube[2]);
  printf("  [%d][%d][%d][%d][%d][%d][%d][%d]  \n",cube[7],cube[4],cube[11],cube[8],cube[19],cube[16],cube[23],cube[20]);
  printf("  [%d][%d][%d][%d][%d][%d][%d][%d]  \n",cube[6],cube[5],cube[10],cube[9],cube[18],cube[17],cube[22],cube[21]);
  printf("        [%d][%d]              \n",cube[15],cube[12]);
  printf("        [%d][%d]              \n",cube[14],cube[13]);
  return;
}
