/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file dnest_line2d.c
 *  \brief run dnest for 2d line analysis.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_interp.h>
#include <mpi.h>

// header file for DNEST
#include "dnestvars.h"

#include "allvars.h"
#include "dnest_line2d.h"
#include "proto.h"


int dnest_line2d(int argc, char **argv)
{
  int i;

  num_params_var = 4; 
  num_params_blr = 12;
  num_params = parset.n_con_recon + num_params_blr + num_params_var;
  size_of_modeltype = num_params * sizeof(double);
  par_fix = (int *) malloc(num_params * sizeof(int));
  par_fix_val = (double *) malloc(num_params * sizeof(double));

  /* setup functions used for dnest*/
  from_prior = from_prior_line2d;
  log_likelihoods_cal = log_likelihoods_cal_line2d;
  log_likelihoods_cal_initial = log_likelihoods_cal_initial_line2d;
  perturb = perturb_line2d;
  print_particle = print_particle_line2d;
  copy_model = copy_model_line2d;
  create_model = create_model_line2d;
  get_num_params = get_num_params_line2d;
  
  /* setup the fixed parameters */
  set_par_fix(num_params_blr);
  for(i=num_params_blr; i<num_params; i++)
  {
    par_fix[i] = 0;
    par_fix_val[i] = -DBL_MAX;
  }

  strcpy(options_file, dnest_options_file);

  if(parset.flag_postprc == 0)
  {
    force_update = 0;
    dnest(argc, argv);
  }
  
  return 0;
}


void from_prior_line2d(void *model)
{
  int i;
  double *pm = (double *)model;

  pm[0] = range_model[0].mu + dnest_rand()*( range_model[1].mu - range_model[0].mu );
  pm[1] = range_model[0].beta + dnest_rand()*( range_model[1].beta - range_model[0].beta );
  pm[2] = range_model[0].F + dnest_rand()*( range_model[1].F - range_model[0].F );
  pm[3] = range_model[0].inc + dnest_rand()*( range_model[1].inc - range_model[0].inc );
  pm[4] = range_model[0].opn + dnest_rand()*( range_model[1].opn - range_model[0].opn );
  pm[5] = range_model[0].A + dnest_rand()*( range_model[1].A - range_model[0].A );
  pm[6] = range_model[0].Ag + dnest_rand()*( range_model[1].Ag - range_model[0].Ag );
  pm[7] = range_model[0].k + dnest_rand()*( range_model[1].k - range_model[0].k );
  pm[8] = range_model[0].mbh + dnest_rand()*( range_model[1].mbh - range_model[0].mbh );
  pm[9] = range_model[0].lambda + dnest_rand()*( range_model[1].lambda - range_model[0].lambda );
  pm[10] = range_model[0].q + dnest_rand()*( range_model[1].q - range_model[0].q );
  // system error, give a large initial value
  pm[11] = range_model[1].logse - dnest_rand()*( range_model[1].logse - range_model[0].logse ) * 0.01;

  for(i=0; i<num_params_blr; i++)
  {
    if(par_fix[i] == 1)
      pm[i] = par_fix_val[i];
  }
  
  pm[12] = var_range_model[0][1] - dnest_rand()*(var_range_model[0][1] - var_range_model[0][0]) * 0.01;
  pm[13] = var_range_model[1][0] + dnest_rand()*(var_range_model[1][1] - var_range_model[1][0]);
  pm[14] =  var_range_model[2][0] + dnest_rand()*(var_range_model[2][1] - var_range_model[2][0]);
  pm[15] = var_range_model[3][0] + dnest_rand()*(var_range_model[3][1] - var_range_model[3][0]);
  for(i=0; i<parset.n_con_recon; i++)
    pm[i + num_params_var + num_params_blr ] = dnest_randn();
  
  which_parameter_update = -1;
}

double log_likelihoods_cal_line2d(const void *model)
{
  double logL;
  logL = prob_line2d(model);
  return logL;
}

double log_likelihoods_cal_initial_line2d(const void *model)
{
  double logL;
  logL = prob_initial_line2d(model);
  return logL;
}

double perturb_line2d(void *model)
{
  double *pm = (double *)model;
  double logH = 0.0, limit1, limit2, width, rnd;
  int which; 

  /* fixed parameters need not to update */
  do
  {
    rnd = dnest_rand();
    if(rnd < 0.5)
      which = dnest_rand_int(num_params_blr + num_params_var);
    else
      which = dnest_rand_int(parset.n_con_recon) + num_params_blr + num_params_var;
  }while(par_fix[which] == 1);
 

  if(which >= num_params || which < 0)
  {
    printf("# Error: Incorrect which.\n");
    exit(0);
  }
  
  which_parameter_update = which;
  
  /* level-dependent width */
  which_level_update = which_level_update > (size_levels-10)?(size_levels-10):which_level_update;
  which_level_update = which_level_update <0?0:which_level_update;

  if( which_level_update != 0)
  {
    limit1 = limits[(which_level_update-1) * num_params *2 + which *2];
    limit2 = limits[(which_level_update-1) * num_params *2 + which *2 + 1];
    width = limit2 - limit1;
  }
  
  switch(which)
  {
  	case 0: // mu
      if(which_level_update == 0)
      {
        limit1 = range_model[0].mu;
        limit2 = range_model[1].mu;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].mu, range_model[1].mu);
      break;
    
    case 1: // beta
      if(which_level_update == 0)
      {
        limit1 = range_model[0].beta;
        limit2 = range_model[1].beta;
        width =  ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].beta, range_model[1].beta);
      break;

    case 2: // F
      if(which_level_update == 0)
      {
        limit1 = range_model[0].F;
        limit2 = range_model[1].F;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].F, range_model[1].F );
      break;

    case 3: // inclination
      if(which_level_update == 0)
      {
        limit1 = range_model[0].inc;
        limit2 = range_model[1].inc;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].inc, range_model[1].inc );
      break;

    case 4: // openning angle
      if(which_level_update == 0)
      {
        limit1 = range_model[0].opn;
        limit2 = range_model[1].opn;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].opn, range_model[1].opn );
      break;

    case 5: // A
      if(which_level_update == 0)
      {
        limit1 = range_model[0].A;
        limit2 = range_model[1].A;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].A, range_model[1].A);
      break;

    case 6: // Ag
      if(which_level_update == 0)
      {
        limit1 = range_model[0].Ag;
        limit2 = range_model[1].Ag;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].Ag, range_model[1].Ag);
      break;

    case 7: // k
      if(which_level_update == 0)
      {
        limit1 = range_model[0].k;
        limit2 = range_model[1].k;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].k, range_model[1].k);
      break;

    case 8: // black hole mass
      if(which_level_update == 0)
      {
        limit1 = range_model[0].mbh;
        limit2 = range_model[1].mbh;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].mbh, range_model[1].mbh);
      break;

    case 9: // lambda
      if(which_level_update == 0)
      {
        limit1 = range_model[0].lambda;
        limit2 = range_model[1].lambda;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].lambda, range_model[1].lambda);
      break;

    case 10: // q
      if(which_level_update == 0)
      {
        limit1 = range_model[0].q;
        limit2 = range_model[1].q;
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), range_model[0].q, range_model[1].q);
      break;

    case 11: // systematic error of line
      if(which_level_update == 0)
      {
        limit1 = range_model[0].logse;
        limit2 = range_model[1].logse;
        width =  ( limit2 - limit1 );
      }
      /* limit the step size of systematic error, like simulated annealing */
      pm[which] += dnest_randh() * fmin(width, (range_model[1].logse - range_model[0].logse)*0.001 );
      wrap_limit(&(pm[which]), range_model[0].logse, range_model[1].logse);
      break;

    case 12: // systematic error of continuum
      if(which_level_update == 0)
      {
        limit1 = var_range_model[0][0];
        limit2 =  var_range_model[0][1];
        width = ( limit2 - limit1 );
      }
      /* limit the step size of systematic error, like simulated annealing */
      pm[which] += dnest_randh() * fmin(width, (var_range_model[0][1] - var_range_model[0][0]) * 0.1);
      wrap(&(pm[which]), var_range_model[0][0], var_range_model[0][1] );
      break;
    
    case 13: // sigma
      if(which_level_update == 0)
      {
        limit1 = var_range_model[1][0];
        limit2 = var_range_model[1][1];
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), var_range_model[1][0], var_range_model[1][1]);
      break;

    case 14: // mu
      if(which_level_update == 0)
      {
        limit1 = var_range_model[2][0];
        limit2 = var_range_model[2][1];
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), var_range_model[2][0], var_range_model[2][1]);
      break;

    case 15: // mean value of continuum
      if(which_level_update == 0)
      {
        limit1 = var_range_model[3][0];
        limit2 = var_range_model[3][1];
        width = ( limit2 - limit1 );
      }
      pm[which] += dnest_randh() * width;
      wrap(&(pm[which]), var_range_model[3][0], var_range_model[3][1]);
      break;

    default: // light curve points
      if(which_level_update == 0)
      {
        limit1 = var_range_model[4][0];
        limit2 = var_range_model[4][1];
        width = ( limit2 - limit1 );
      }
      logH -= (-0.5*pow(pm[which], 2.0) );
      pm[which] += dnest_randh() * width;
      wrap(&pm[which], var_range_model[4][0], var_range_model[4][1]);
      logH += (-0.5*pow(pm[which], 2.0) );
      break;
  }

  return logH;
}

void print_particle_line2d(FILE *fp, const void *model)
{
  int i;
  double *pm = (double *)model;

  for(i=0; i<num_params; i++)
  {
    fprintf(fp, "%f ", pm[i] );
  }
  fprintf(fp, "\n");
}

void copy_model_line2d(void *dest, const void *src)
{
  memcpy(dest, src, size_of_modeltype);
}

void *create_model_line2d()
{
  return (void *)malloc(size_of_modeltype);
}

int get_num_params_line2d()
{
  return num_params;
}
