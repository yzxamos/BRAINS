/*
 * BRAINS
 * (B)LR (R)everberation-mapping Analysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_interp.h>

#include "allvars.h"
#include "proto.h"

void init()
{
  allocate_memory();

  int i;
  double dT;

  /* set time array for continuum */
  Tcon_min = Tcon_data[0] - fmax(0.05*(Tcon_data[n_con_data -1] - Tcon_data[0]), 10);
  Tcon_max = Tcon_data[n_con_data-1] + fmax(0.05*(Tcon_data[n_con_data -1] - Tcon_data[0]), 10);
  dT = (Tcon_max - Tcon_min)/(parset.n_con_recon -1);
  
  for(i=0; i<parset.n_con_recon; i++)
  {
    Tcon[i] = Tcon_min + i*dT;
  }

  /* initialize GSL */
  gsl_T = gsl_rng_default;
  gsl_r = gsl_rng_alloc (gsl_T);

#ifndef Debug 
  gsl_rng_set(gsl_r, time(NULL)+thistask+1350); 
#else
  gsl_rng_set(gsl_r, 6666+thistask+1350); 
#endif

  gsl_acc = gsl_interp_accel_alloc();
  gsl_linear = gsl_interp_alloc(gsl_interp_linear, parset.n_con_recon);

  /* set the range of cloud radial distribution */
  rcloud_min_set = parset.tau_min_set;
  rcloud_max_set = parset.tau_max_set;

  range_model[0].mbh = log(0.1);
  range_model[1].mbh = log(1000.0);

  range_model[0].mu = log(0.1);
  range_model[1].mu = log(parset.tau_max_set*0.5);

  range_model[0].beta = 0.001;
  range_model[1].beta = 1.0;

  range_model[0].F = 0.001;
  range_model[1].F = 0.999;

  range_model[0].inc = 0.0;
  range_model[1].inc = 90.0;

  range_model[0].opn = 0.0;
  range_model[1].opn = 90.0;

  range_model[0].A = log(0.01);
  range_model[1].A = log(10.0);

  range_model[0].Ag = -1.0;
  range_model[1].Ag = 3.0;

  range_model[0].k =-0.5;
  range_model[1].k = 0.5;

  range_model[0].lambda = 0.0;
  range_model[1].lambda = 1.0;

  range_model[0].q = 0.0;
  range_model[1].q = 1.0;

  //range_model[0].logse = log(1.0e-12);
  //range_model[1].logse = log(1.0e6);

  range_model[1].mu = fmin(range_model[1].mu, rcloud_max_set);
}

void allocate_memory()
{
  Tcon = malloc(parset.n_con_recon * sizeof(double));
  Fcerrs = malloc(parset.n_con_recon * sizeof(double));

  PSmat = malloc(parset.n_con_recon * parset.n_con_recon * sizeof(double));
  USmat = malloc(parset.n_con_recon * n_con_data * sizeof(double));
  PSmat_data = malloc(n_con_data * n_con_data * sizeof(double));
}

void free_memory()
{
  free(Tcon);
  free(Fcerrs);

  free(PSmat);
  free(USmat);
  free(PSmat_data);
}

void scale_con_line()
{
  int i, j;
  double ave_con, ave_line;
  
  con_scale = 1.0;
  line_scale = 1.0;

  ave_con = 0.0;
  ave_line = 0.0;
  for(i=0; i<n_con_data; i++)
  {
    ave_con += Fcon_data[i];
  }

  ave_con /= n_con_data;
  con_scale = 1.0/ave_con;

  for(i=0; i<n_con_data; i++)
  {
    Fcon_data[i] *=con_scale;
    Fcerrs_data[i] *=con_scale;
  }

  printf("task %d con scale: %e\t%e\n", thistask, con_scale, ave_con);
  
  if(parset.flag_dim == 0)
  {
    return;
  }

  for(i=0; i<n_line_data; i++)
  {
    ave_line += Fline_data[i];
  }
  ave_line /=n_line_data;

  line_scale = 1.0/ave_line;
  
  printf("task %d line scale: %e\t%e\n", thistask, line_scale, ave_line);

  for(i=0; i<n_line_data; i++)
  {
    Fline_data[i] *= line_scale;
    Flerrs_data[i] *= line_scale;

    if(parset.flag_dim==2)
      for(j=0; j<n_vel_data; j++)
      {
        Fline2d_data[i*n_vel_data + j] *= line_scale;
        Flerrs2d_data[i*n_vel_data + j] *= line_scale;
      } 
  }
}