/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#include "allvars.h"
#include "proto.h"

/*! \file run.c
 *  \brief setup and run the program. 
 */

/*!
 * This function setups and runs the program.
 */
void begin_run()
{
  /*   Velocity unit 
   *   This unit should be determined in the beginning because the data velocity
   *   needs to be converted using this unit in read.c. 
   */
  VelUnit = sqrt( GRAVITY * 1.0e6 * SOLAR_MASS / CM_PER_LD ) / 1.0e5; 

  /* read parameter file */
  read_parset();
  /* read data files */
  read_data();
  /* scale continuum and line to an order of unity */
  scale_con_line();

  /* initialization */
  init();
  
  /* now run dnest and reconstruct the model. */
  MPI_Barrier(MPI_COMM_WORLD);
  if(parset.flag_dim == -1)
  {
    sim();
  }

  if(parset.flag_dim == 0) /* only continuum */
  {
    reconstruct_con();
  }

  if(parset.flag_dim == 1) /* 1d line */
  {
    reconstruct_con();
  	reconstruct_line1d();
  }

  if(parset.flag_dim == 2) /* 2d line */
  {
    reconstruct_con();
    reconstruct_line2d();
  }

  return;
}

/*!
 * This function frees the memory and ends the run.
 */
void end_run()
{
  free_memory_data();
  free_memory();
}