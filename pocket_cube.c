// =================================================
// # ** INTELLIGENT INFORMATION PROCESSING **      #
// # [ Solving Pocket-Cube with A-star Algorithm ] #
// # --------------------------------------------- #
// # COMPILE:                                      #
// #    gcc pocket_cube.c [-o (out_name)] -lm      #
// # --------------------------------------------- #
// #   Copyright (C) Y.Sotaro All Right Reserved.  #
// =================================================

/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>     // -lm

/* DEFINES */

/* MACROS */

/* GLOBAL */

/* PROTOTYPE */
void usage ( char out_name );
/* STRUCTURES */

/* MAIN */
int main ( int argc, char *argv[] )
{
  /* Local var */
  
  /* Print Usage */
  if ( argc == 2 ){
     usage( argv[0] );
  }

  //     [ 0][ 1]
  //     [ 2][ 3]
  // [ 4][ 5][ 6][ 7][ 8][ 9][10][11]
  // [12][13][14][15][16][17][18][19]
  //     [20][21]
  //     [22][23]

  return EXIT_SUCCESS;
}

/** Print Usage **
 * [Functon]: usage
 * [Argument]:
 *  - char *out_name : output-file name
 * [return]:
 *  - Always: NULL
 **/
void usage ( char *out_name )
{
  printf("## SOLVING POCKET CUBE WITH A-STAR ALGORITHM ##\n");
  printf("  Usage:\n");
  printf("    ./%s XXX\n",out_name);

  return NULL;
}