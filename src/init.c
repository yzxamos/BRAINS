/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file init.c
 *  \brief initialize the program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_interp.h>

#include "allvars.h"
#include "proto.h"

/*! 
 * This function initialize the program.
 */
void init()
{

  nq = 1 + parset.flag_trend;
  
  if(parset.flag_trend_diff)
  {
    num_params_trend = 1; // differences of the trends between continuum and line
  }
  else
  {
    num_params_trend = 0;
  }
  
  num_params_var = 4 + parset.flag_trend + num_params_trend;

  // number of parameters for narrow line, only valid for 2d RM.
  num_params_nlr = 0;
  if(parset.flag_narrowline == 2)
    num_params_nlr = 3;
  
  switch(parset.flag_blrmodel)
  {
    case 1:
      BLRmodel_size = sizeof(BLRmodel1);
      set_blr_range_model = set_blr_range_model1;
      break;
    case 2:
      BLRmodel_size = sizeof(BLRmodel2);
      set_blr_range_model = set_blr_range_model2;
      break;
    case 3:
      BLRmodel_size = sizeof(BLRmodel3);
      set_blr_range_model = set_blr_range_model3;
      break;
    case 4:
      BLRmodel_size = sizeof(BLRmodel4);
      set_blr_range_model = set_blr_range_model4;
      break;
    case 5:
      BLRmodel_size = sizeof(BLRmodel5);
      set_blr_range_model = set_blr_range_model5;
      break;
    case 6:
      BLRmodel_size = sizeof(BLRmodel6);
      set_blr_range_model = set_blr_range_model6;
      break;
    case 7:
      BLRmodel_size = sizeof(BLRmodel7);
      set_blr_range_model = set_blr_range_model7;
      break;
    default:
      BLRmodel_size = sizeof(BLRmodel1);
      set_blr_range_model = set_blr_range_model1;
      break;
  }

  allocate_memory();

  /* initialize GSL */
  gsl_T = gsl_rng_default;
  gsl_r = gsl_rng_alloc (gsl_T);

#ifndef Debug 
  gsl_rng_set(gsl_r, time(NULL)+thistask+1350); 
#else
  gsl_rng_set(gsl_r, 6666+thistask+1350); 
  printf("# debugging, task %d brains random seed %d.\n", thistask, 6666+thistask+1350);
#endif

  gsl_acc = gsl_interp_accel_alloc();
  gsl_linear = gsl_interp_alloc(gsl_interp_linear, parset.n_con_recon);
  
  /*if(parset.flag_dim > 0)
  {
    parset.tau_max_set = fmin(parset.tau_max_set, Tline_data[n_line_data -1] - Tcon_data[0]);
  }*/

  /* set the range of cloud radial distribution */
  rcloud_min_set = parset.tau_min_set;
  rcloud_max_set = parset.tau_max_set*1.0e3;
  
  /* set the range of continuum variation  */
  var_range_model[0][0] = log(1.0e-10);; // systematic error in continuum
  var_range_model[0][1] = log(1.0e6);;

  var_range_model[1][0] = -15.0; // log(sigma)
  var_range_model[1][1] = -1.0; 

  var_range_model[2][0] = log(1.0); // log(tau)
  var_range_model[2][1] = log(1.0e4); 

  var_range_model[3][0] = -10.0; // mean value or trend parameter values
  var_range_model[3][1] =  10.0; 

  var_range_model[4][0] = -0.1; // slope of the trend in the differences between contiuum and line
  var_range_model[4][1] =  0.1; 

  var_range_model[5][0] = -10.0; // light curve values
  var_range_model[5][1] = 10.0; 

  
  if(num_params_nlr > 1)
  {
    nlr_range_model[0][0] = parset.flux_narrowline - 10.0 * parset.flux_narrowline_err;
    nlr_range_model[0][1] = parset.flux_narrowline + 10.0 * parset.flux_narrowline_err;

    nlr_range_model[1][0] = parset.width_narrowline - 10.0 * parset.width_narrowline_err;
    nlr_range_model[1][1] = parset.width_narrowline + 10.0 * parset.width_narrowline_err;

    nlr_range_model[2][0] = parset.shift_narrowline - 10.0 * parset.shift_narrowline_err;
    nlr_range_model[2][1] = parset.shift_narrowline + 10.0 * parset.shift_narrowline_err;
  }

  set_blr_range_model();
}

/*!
 * This function allocates memory for variables used throughout the code. 
 */
void allocate_memory()
{
  int i;

  Tcon = malloc(parset.n_con_recon * sizeof(double));
  Fcerrs = malloc(parset.n_con_recon * sizeof(double));

  PSmat = malloc(parset.n_con_recon * parset.n_con_recon * sizeof(double));
  PNmat = malloc(parset.n_con_recon * parset.n_con_recon * sizeof(double));
  USmat = malloc(parset.n_con_recon * n_con_data * sizeof(double));
  PSmat_data = malloc(n_con_data * n_con_data * sizeof(double));
  PNmat_data = malloc(n_con_data * n_con_data * sizeof(double));
  PCmat_data = malloc(n_con_data * n_con_data * sizeof(double));
  IPCmat_data = malloc(n_con_data * n_con_data * sizeof(double));
  PQmat = malloc(parset.n_con_recon * parset.n_con_recon * sizeof(double));
  PEmat1 = malloc(parset.n_con_recon * n_con_data * sizeof(double));
  PEmat2 = malloc(parset.n_con_recon * parset.n_con_recon * sizeof(double));
  
  blr_range_model = malloc(BLRmodel_size/sizeof(double) * sizeof(double *));
  for(i=0; i<BLRmodel_size/sizeof(double); i++)
  {
    blr_range_model[i] = malloc(2*sizeof(double));
  }

  workspace = malloc((10*n_con_data + 5*parset.n_con_recon)*sizeof(double));

  var_param = malloc(num_params_var * sizeof(double));
  var_param_std = malloc(num_params_var * sizeof(double));

  return;
}

/*! 
 * This function free memory.
 */
void free_memory()
{
  int i;

  free(Tcon);
  free(Fcerrs);

  free(PSmat);
  free(PNmat);
  free(USmat);
  free(PSmat_data);
  free(PNmat_data);
  free(PCmat_data);
  free(IPCmat_data);
  free(PQmat);
  free(PEmat1);
  free(PEmat2);

  for(i=0; i<BLRmodel_size/sizeof(double); i++)
  {
    free(blr_range_model[i]);
  }
  free(blr_range_model);

  free(workspace);

  free(var_param);
  free(var_param_std);
  
  return;
}

/*!
 * This function normalise the light curves to a scale of unity.
 */
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

  if(thistask == roottask)
    printf("task %d con scale: %e\t%e\n", thistask, con_scale, ave_con);
  
  if(parset.flag_dim <= 0)
  {
    return;
  }

  for(i=0; i<n_line_data; i++)
  {
    ave_line += Fline_data[i];
  }
  ave_line /=n_line_data;

  line_scale = 1.0/ave_line;
  
  if(thistask == roottask)
    printf("task %d line scale: %e\t%e\n", thistask, line_scale, ave_line);

  for(i=0; i<n_line_data; i++)
  {
    // note mask with error >= 1.0e100
    if(Flerrs_data[i] < 0.99e100)
    {
      Fline_data[i] *= line_scale;
      Flerrs_data[i] *= line_scale;
    }

    if(parset.flag_dim==2)
      for(j=0; j<n_vel_data; j++)
      {
        // note mask with error >= 1.0e100
        if(Flerrs2d_data[i*n_vel_data + j] < 0.99e100)
        {
          Fline2d_data[i*n_vel_data + j] *= line_scale;
          Flerrs2d_data[i*n_vel_data + j] *= line_scale;
        } 
      } 
  }
  line_error_mean *= line_scale;

  parset.flux_narrowline *= line_scale;
  parset.flux_narrowline_err *= line_scale;
}

/*!
 * This function copes with parameter fixing.\n
 * Only fix BLR model parameters. 
 */
void set_par_fix(int num_params_blr)
{
  int i;
  char *pstr;
  
  npar_fix = 0;

  if(thistask == roottask)
  {
    pstr = parset.str_par_fix_val;
    // set the default value if not provided.
    for(i=strlen(parset.str_par_fix); i<num_params_blr; i++)
      parset.str_par_fix[i] = '0';

    for(i=0; i<num_params_blr_model; i++)
    {
      if(parset.str_par_fix[i] == '0')
      {
        par_fix[i] = 0;
        par_fix_val[i] = -DBL_MAX;  /* set to be the smallest value */
      }
      else if(parset.str_par_fix[i] == '1')
      {
        if(pstr == NULL)
        {
          printf("# %d-th BLR parameter value is not provided (counting from 0).\n", i);
          exit(0);
        }
        par_fix[i] = 1;
        sscanf(pstr, "%lf", &par_fix_val[i]);
        npar_fix++;
        printf("# %d-th parameter fixed, value= %f.\n", i, par_fix_val[i]);
        pstr = strchr(pstr, ':'); /* values are separated by ":" */
        if(pstr!=NULL)
        {
          pstr++;
        }
      }
      else   // default value
      {
        par_fix[i] = 0;
        par_fix_val[i] = -DBL_MAX;
      }
    }

    // cope with narrow line
    if(parset.flag_narrowline == 2)
    {
      if(parset.flux_narrowline_err == 0.0)
      {
        par_fix[num_params_blr_model - 1] = 1.0;
        par_fix_val[num_params_blr_model -1] = 0.0;
      }

      if(parset.width_narrowline_err == 0.0)
      {
        par_fix[num_params_blr_model-1+1] = 1.0;
        par_fix_val[num_params_blr_model-1+1] = 0.0;
      } 

      if(parset.shift_narrowline_err == 0.0)
      {
        par_fix[num_params_blr_model-1+2] = 1.0;
        par_fix_val[num_params_blr_model-1+2] = 0.0;
      }
    }
  }

  MPI_Bcast(par_fix, num_params_blr, MPI_INT, roottask, MPI_COMM_WORLD);
  MPI_Bcast(par_fix_val, num_params_blr, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
  return;
}

/*
 * setup BLR model parameter range. 
 */

// model 1
void set_blr_range_model1()
{
  int i;

  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //beta
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 1.0;
  //F
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 0.999;
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //lambda
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.5;
  //q
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  return;
}

// model 2
void set_blr_range_model2()
{
  int i;

  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //beta
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 1.0;
  //F
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 0.999;
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //sigr
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //sigtheta
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  return;
}

// model 3
void set_blr_range_model3()
{
  int i;
  
  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //alpha
  blr_range_model[i][0] = -3.0;
  blr_range_model[i++][1] = 3.0;
  //Rin
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //F
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0e2);
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //xi
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //q
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  rcloud_max_set = fmax(rcloud_max_set, exp(blr_range_model[3][1] + blr_range_model[4][1]));
  
  return;
}

// model 3
void set_blr_range_model4()
{
  int i;
  
  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //alpha
  blr_range_model[i][0] = -3.0;
  blr_range_model[i++][1] = 3.0;
  //Rin
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //F
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0e2);
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //xi
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = sqrt(2.0)/2.0;
  //q
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  rcloud_max_set = fmax(rcloud_max_set, exp(blr_range_model[3][1] + blr_range_model[4][1]));

  return;
}

// model 5
void set_blr_range_model5()
{
  int i;
  
  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //Fin
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //Fout
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(10.0);
  //alpha
  blr_range_model[i][0] = 1.0;
  blr_range_model[i++][1] = 3.0;
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //beta
  blr_range_model[i][0] = 1.0;
  blr_range_model[i++][1] = 5.0;
  //xi
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //fellip
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //fflow
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //rf
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(rcloud_max_set);
  //sigr_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //sigr_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //theta_rot
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  rcloud_max_set = fmax(rcloud_max_set, exp(blr_range_model[2][1] + blr_range_model[4][1]));

  return;
}


// model 6
void set_blr_range_model6()
{
  int i;
  
  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //beta
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 1.0;
  //F
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //gamma
  blr_range_model[i][0] = 1.0;
  blr_range_model[i++][1] = 5.0;
  //xi
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //fellip
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //fflow
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //sigr_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //sigr_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //theta_rot
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //sig_turb
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  return;
}

// model 7
void set_blr_range_model7()
{
  int i;
  

  i = 0;
  //A
  blr_range_model[i][0] = log(0.01);
  blr_range_model[i++][1] = log(10.0);
  //Ag
  blr_range_model[i][0] = -1.0;
  blr_range_model[i++][1] = 3.0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //beta
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 1.0;
  //F
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //inc
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //opn
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;
  //k
  blr_range_model[i][0] = -0.5;
  blr_range_model[i++][1] = 0.5;
  //gamma
  blr_range_model[i][0] = 1.0;
  blr_range_model[i++][1] = 5.0;
  //xi
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;

  //fsh
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //mu_un
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(parset.tau_max_set);
  //beta_un
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 1.0;
  //F_un
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //opn_un
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;

  //mbh
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(1000.0);
  //fellip
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //fflow
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //sigr_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_circ
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //sigr_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(0.1);
  //sigthe_rad
  blr_range_model[i][0] = log(0.001);
  blr_range_model[i++][1] = log(1.0);
  //theta_rot
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 90.0;

  //fellip_un
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //fflow_un
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;

  //logse
  blr_range_model[i][0] = log(1.0);
  blr_range_model[i++][1] = log(1.0+10.0);

  return;
}
