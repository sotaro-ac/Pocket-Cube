// =================================================
// # ** INTELLIGENT INFORMATION PROCESSING **      #
// # [ Solving Pocket-Cube with A-star Algorithm ] #
// # --------------------------------------------- #
// # COMPILE:                                      #
// #   emcc -O3 -s WASM=1 EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' ...
// # --------------------------------------------- #
// # Copyright (c) 2020 sotaro-ac @License MIT     #
// # https://github.com/sotaro-ac/Pocket-Cube      #
// =================================================

/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h> // -lm
#include <limits.h>

/* DEFINES */
#define GODNUM 14    // God's Number
#define FACES 24     // Sum of cube faces
#define IDLEN 25     // uchar[24] + \0
#define TBLSIZ 65536 // 65536 (64KB) MAX: 3674160
#define OPRNUM 6     // Next states of cube
#define STEPCOST 12  // Cost of step between node to node

/* MACROS */

/* TYPEDEF */
typedef unsigned char uchar; // uchar

typedef struct _TNODE_
{
  uchar cube[IDLEN]; // cube condition (is unique)
  bool close;        // is closed or open
  struct _TNODE_ *parent;
  struct _TNODE_ *next;
} node_t;

/* GLOBAL */
int node_sum = 0;
int unhit_sum = 0;

uchar heu_map[FACES][FACES];

// [ Cube Expansion Plane and face index ]
//  <ARRAY INDEX>
//           [ 0][ 1]
//           [ 3][ 2]
//   [ 7][ 4][11][ 8][19][16][23][20]
//   [ 6][ 5][10][ 9][18][17][22][21]
//           [15][12]
//           [14][13]
//
//  <GOAL UCHAR VALUES>
//         [0][1]
//         [3][2]
//   [7][4][B][8][J][G][N][K]
//   [6][5][A][9][I][H][M][L]
//         [F][C]
//         [E][D]

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

/* PROTOTYPE */
void usage(char *out_name);
int hash(uchar *cube);
node_t *newNode(uchar *cube);
node_t *addHashTbl(node_t **hashTbl, uchar *cube);
node_t *searchNode(node_t **hashTbl, uchar *cube);
int f(node_t *node);          // = g() + h()
int g(node_t *node);          // Current sum of steps to start-node
int h(node_t *node);          // Heuristics
void init_heu_map(void);      // heuristics map
uchar *rotateRU(uchar *cube); // Rotate Right face to Up
uchar *rotateRD(uchar *cube); // Rotate Right face to Up
uchar *rotateLU(uchar *cube); // Rotate Left face to Up
uchar *rotateLD(uchar *cube); // Rotate Left face to Down
uchar *rotateBR(uchar *cube); // Rotate Bottom face to Right
uchar *rotateBL(uchar *cube); // Rotate Bottom face to Left
void print_cube(uchar *cube);
void print_rout(node_t *node);

/* MAIN */
int main(int argc, char *argv[])
{
  /* Local vars */
  node_t **hashTbl; // hash table (for searching exist nodes)
  node_t **nodeQue; // Node Queue (both open/close nodes in this)
  node_t **queswp;
  node_t *tmp, *top;
  uchar goal[IDLEN];
  uchar *start, *tmp_cube;
  uchar *(*rotate[])(uchar *) = {
      rotateRU, rotateRD,
      rotateLU, rotateLD,
      rotateBR, rotateBL};
  int size = 0;
  int que_max = TBLSIZ;
  int extend = 0;
  int closed = 0;
  int steps = 0;
  int loops = 0;
  int set_rotate = 0;
  int i, j, k;
  int min_idx, top_idx;

  switch (argc)
  {
  case 1:
    /* set start cube status randomly */
    unsigned seed = (unsigned)time(NULL);
    printf("seed: %u\n", seed);
    srand(seed);
    set_rotate = rand() % GODNUM;
    break;

  case 2:
    unsigned seed = atoi(argv[2]);
    printf("seed: %u\n", seed);
    srand(seed);
    set_rotate = atoi(argv[1]);
    break;

  case 3:
    unsigned seed = atoi(argv[2]);
    printf("seed: %u\n", seed);
    srand(seed);
    set_rotate = atoi(argv[1]);
    break;

  default:
    usage(argv[0]);
    break;
  }

  /* initialize goal cube */
  for (i = 0; i < FACES; i++)
    goal[i] = charset[i];
  goal[FACES] = '\0';

  /* initialize start cube */
  start = (uchar *)malloc(sizeof(uchar) * IDLEN);
  for (i = 0; i < FACES; i++)
    start[i] = charset[i];
  printf("rotate: %d\n", set_rotate);
  printf("[GOAL]->");
  for (i = 0; i < set_rotate; i++)
  {
    k = rand() % OPRNUM;
    switch (k)
    {
    case 0:
      printf("RU->");
      break;
    case 1:
      printf("RD->");
      break;
    case 2:
      printf("LU->");
      break;
    case 3:
      printf("LD->");
      break;
    case 4:
      printf("BR->");
      break;
    case 5:
      printf("BL->");
      break;
    }
    tmp_cube = start;
    start = (*rotate[k])(tmp_cube);
    free(tmp_cube);
  }
  start[FACES] = '\0';
  printf("[START]\n");
  print_cube(start);

  /* initialize heu_map */
  init_heu_map();

  /* initialize hash table */
  hashTbl = (node_t **)malloc(sizeof(node_t *) * TBLSIZ);
  if (hashTbl == NULL)
  {
    printf("FATAL: Failed to allocate memory.\n");
    return EXIT_FAILURE;
  }

  /* initialize nodeQue list */
  nodeQue = (node_t **)malloc(sizeof(node_t *) * TBLSIZ);
  if (nodeQue == NULL)
  {
    printf("FATAL: Failed to allocate memory.\n");
    return EXIT_FAILURE;
  }

  /* add start-node to hashTbl */
  nodeQue[size++] = addHashTbl(hashTbl, start);

  /*** A-star Algorithm ***/
  while (0 < size)
  {
    if (++loops % 100 == 0)
    {
      printf("Loop: %5d/ size: %5d/ node: %5d/ miss: %5d/ extend: %4d\r",
             loops, size, node_sum, unhit_sum, extend);
      fflush(stdout);
    }
    /* find next poppen-node by f() */
    top_idx = min_idx = size - 1;
    for (i = 0; i < size - 1; i++)
    {
      if (f(nodeQue[i]) < f(nodeQue[min_idx]))
      {
        min_idx = i;
      }
    }

    /* swap top-node and min_idx-node */
    tmp = nodeQue[top_idx];
    nodeQue[top_idx] = nodeQue[min_idx];
    nodeQue[min_idx] = tmp;

    /* pop top-node */
    top = nodeQue[--size];
    top->close = true;

    /* is Goal? */
    if (!memcmp(top->cube, goal, sizeof(uchar) * IDLEN))
    {
      printf("Loop: %5d/ size: %5d/ node: %5d/ miss: %5d/ extend: %4d\n",
             loops, size, node_sum, unhit_sum, extend);
      /* How many times from start */
      tmp = top;
      while (tmp->parent != NULL)
      {
        tmp = tmp->parent;
        steps++;
      }
      printf("steps: %d\n", steps);
      print_cube(top->cube);
      print_rout(top);
      printf("SUCCEED!\n");
      return EXIT_SUCCESS;
    }

    /* open new nodes and link with arcs */
    for (i = 0; i < OPRNUM; i++)
    {
      /* Using function pointer */
      tmp_cube = (*rotate[i])(top->cube);
      tmp = searchNode(hashTbl, tmp_cube);
      if (tmp == NULL)
      {
        /* is not exist? */
        tmp = addHashTbl(hashTbl, tmp_cube);
        tmp->parent = top;
        /* Extend nodeQue */
        if (size == que_max)
        {
          extend++;
          // add TBLSIZ to que_max
          queswp = (node_t **)malloc(sizeof(node_t *) * (que_max + TBLSIZ));
          memcpy(queswp, nodeQue, sizeof(node_t *) * que_max);
          free(nodeQue);
          que_max = que_max + TBLSIZ;
          nodeQue = queswp;
        }
        nodeQue[size++] = tmp;
      }
      else
      {
        /* is exist */
        if (!tmp->close)
        {
          /* is open */
          if (g(top) + STEPCOST < g(tmp))
          {
            tmp->parent = top;
          }
        }
        else
        {
          /* is close */
          if (g(top) + STEPCOST < g(tmp))
          {
            tmp->parent = top;
            tmp->close = false;
          }
        }
      }
      free(tmp_cube);
    }
  } // End of while

  /* failed to goal */
  printf("FAILED!\n");
  return EXIT_SUCCESS;
}

/** functions **/
void usage(char *out_name)
{
  printf("## SOLVING POCKET CUBE WITH A-STAR ALGORITHM ##\n");
  printf("  Usage:\n");
  printf("    %s [ROTATE] [SEED]\n", out_name);
  printf("    NO PARAMS : Solve pocket-cube which has random faces.\n");
  printf("    [ROTATE]  : set rotating times with 1st param (Min: 1).\n");
  printf("    [SEED]    : set random seed with 2nd param.\n\n");
  return;
}
/** hash **/
int hash(uchar *cube)
{
  int sum, i;
  sum = 0;
  for (i = 0; i < FACES; i++)
  {
    sum += cube[i] * pow(2, i);
  }
  return sum % TBLSIZ;
}
/* newNode */
node_t *newNode(uchar *cube)
{
  node_t *new;
  int i;

  new = (node_t *)malloc(sizeof(node_t));
  if (new == NULL)
  {
    printf("FATAL: Failed to allocate memory.\n");
    exit(1);
  }
  node_sum++;
  memcpy(new->cube, cube, sizeof(uchar) * IDLEN);
  new->close = false;
  new->parent = NULL;
  new->next = NULL;

  return new;
}
/* addHashTbl */
node_t *addHashTbl(node_t **hashTbl, uchar *cube)
{
  node_t *tmp, *pre_tmp;
  int hash_val = hash(cube);

  if (hashTbl[hash_val] == NULL)
  {
    hashTbl[hash_val] = newNode(cube);
    return hashTbl[hash_val];
  }
  tmp = hashTbl[hash_val];
  while (tmp != NULL)
  {
    if (!memcmp(cube, tmp->cube, sizeof(uchar) * IDLEN))
    {
      // printf("Already registered!\n");
      return tmp;
    }
    pre_tmp = tmp;
    tmp = tmp->next;
    unhit_sum++;
  }
  pre_tmp->next = newNode(cube);
  return pre_tmp->next;
}
/* searchNode */
node_t *searchNode(node_t **hashTbl, uchar *cube)
{
  node_t *tmp = hashTbl[hash(cube)];

  if (tmp == NULL)
    return NULL;
  while (tmp != NULL)
  {
    if (!memcmp(tmp->cube, cube, sizeof(uchar) * IDLEN))
    {
      return tmp;
    }
    tmp = tmp->next;
    unhit_sum++;
  }
  return NULL;
}
/* f */
int f(node_t *node)
{
  return g(node) + h(node);
}
/* g */
int g(node_t *node)
{
  int steps = 0;
  while (node->parent != NULL)
  {
    node = node->parent;
    steps++;
  }
  return steps * STEPCOST;
}
/* h */
int h(node_t *node)
{
  int i, sum = 0;
  uchar c, offset;

  for (i = 0; i < FACES; i++)
  {
    c = node->cube[i];
    if (isdigit(c))
      offset = '0';
    else
      offset = 'A' - 10;
    sum += heu_map[c - offset][i];
  }
  return sum;
}
/* init_heu_map */
void init_heu_map(void)
{
  int i, j, k, s;
  uchar *tmp[6];
  uchar before[FACES];
  uchar *(*rotate[])(uchar *) = {
      rotateRU, rotateRD,
      rotateLU, rotateLD,
      rotateBR, rotateBL};

  for (i = 0; i < FACES; i++)
  {
    for (j = 0; j < FACES; j++)
    {
      heu_map[i][j] = UCHAR_MAX;
    }
  }
  for (i = 0; i < FACES; i++)
    before[i] = i;
  for (k = 0; k < 6; k++)
    tmp[k] = (*rotate[k])(before);

  /* 0 step */
  for (i = 0; i < FACES; i++)
    heu_map[i][i] = 0;
  /* 1-4 step */
  for (s = 0; s < 4; s++)
  {
    for (j = 0; j < FACES; j++)
    {
      for (i = 0; i < FACES; i++)
      {
        for (k = 0; k < OPRNUM; k++)
        {
          if (heu_map[i][j] == UCHAR_MAX && heu_map[i][tmp[k][j]] == s)
          {
            heu_map[i][j] = s + 1;
          }
        }
      }
    }
  }
  return;
}

/* rotateXXs */
uchar *rotateRU(uchar *cube)
{
  int i, offset = 12;
  int key[4] = {5, 9, 17, 21};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[i]] = cube[key[(i + 3) % 4]];
    tmp[key[i] + 1] = cube[key[(i + 3) % 4] + 1];
    tmp[i + offset] = cube[(i + 3) % 4 + offset];
  }

  return tmp;
}
uchar *rotateRD(uchar *cube)
{
  int i, offset = 12;
  int key[4] = {5, 9, 17, 21};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[(i + 3) % 4]] = cube[key[i]];
    tmp[key[(i + 3) % 4] + 1] = cube[key[i] + 1];
    tmp[(i + 3) % 4 + offset] = cube[i + offset];
  }

  return tmp;
}
uchar *rotateLU(uchar *cube)
{
  int i, offset = 16;
  int key[4] = {1, 8, 12, 22};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[i]] = cube[key[(i + 3) % 4]];
    tmp[key[i] + 1] = cube[key[(i + 3) % 4] + 1];
    tmp[(i + 3) % 4 + offset] = cube[i + offset];
  }

  return tmp;
}
uchar *rotateLD(uchar *cube)
{
  int i, offset = 16;
  int key[4] = {1, 8, 12, 22};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[(i + 3) % 4]] = cube[key[i]];
    tmp[key[(i + 3) % 4] + 1] = cube[key[i] + 1];
    tmp[i + offset] = cube[(i + 3) % 4 + offset];
  }

  return tmp;
}
uchar *rotateBR(uchar *cube)
{
  int i, offset = 20;
  int key[4] = {0, 6, 13, 16};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[i]] = cube[key[(i + 3) % 4]];
    tmp[key[i] + 1] = cube[key[(i + 3) % 4] + 1];
    tmp[i + offset] = cube[(i + 3) % 4 + offset];
  }

  return tmp;
}
uchar *rotateBL(uchar *cube)
{
  int i, offset = 20;
  int key[4] = {0, 6, 13, 16};
  uchar *tmp;
  tmp = (uchar *)malloc(sizeof(uchar) * IDLEN);
  memcpy(tmp, cube, sizeof(uchar) * IDLEN);
  /* rotate cube */
  for (i = 0; i < 4; i++)
  {
    tmp[key[(i + 3) % 4]] = cube[key[i]];
    tmp[key[(i + 3) % 4] + 1] = cube[key[i] + 1];
    tmp[(i + 3) % 4 + offset] = cube[i + offset];
  }

  return tmp;
}
/* print_cube */
void print_cube(uchar *cube)
{
  // printf("  cube[%s]\n",cube);
  printf("        [%c][%c]              \n", cube[0], cube[1]);
  printf("        [%c][%c]              \n", cube[3], cube[2]);
  printf("  [%c][%c][%c][%c][%c][%c][%c][%c]  \n", cube[7], cube[4], cube[11], cube[8], cube[19], cube[16], cube[23], cube[20]);
  printf("  [%c][%c][%c][%c][%c][%c][%c][%c]  \n", cube[6], cube[5], cube[10], cube[9], cube[18], cube[17], cube[22], cube[21]);
  printf("        [%c][%c]              \n", cube[15], cube[12]);
  printf("        [%c][%c]              \n", cube[14], cube[13]);
  return;
}
/* print_rout */
void print_rout(node_t *node)
{
  int i;
  node_t *tmp;
  uchar *tmp_cube;
  uchar *(*rotate[])(uchar *) = {
      rotateRU, rotateRD,
      rotateLU, rotateLD,
      rotateBR, rotateBL};
  printf("[GOAL]");
  tmp = node;
  while (tmp->parent != NULL)
  {
    for (i = 0; i < OPRNUM; i++)
    {
      tmp_cube = (*rotate[i])(tmp->cube);
      if (!memcmp(tmp_cube, tmp->parent->cube, sizeof(uchar) * IDLEN))
      {
        switch (i)
        {
        case 0:
          printf("<-RD");
          break;
        case 1:
          printf("<-RU");
          break;
        case 2:
          printf("<-LD");
          break;
        case 3:
          printf("<-LU");
          break;
        case 4:
          printf("<-BL");
          break;
        case 5:
          printf("<-BR");
          break;
        }
      }
      free(tmp_cube);
    }
    tmp = tmp->parent;
  }
  printf("<-[START]\n");
  return;
}
