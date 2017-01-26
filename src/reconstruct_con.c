/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file reconstruct_con.c
 *  \brief reconstruct continuum.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_interp.h>

#include "dnestvars.h"
#include "allvars.h"
#include "proto.h"

#include "dnest_con.h"

void *best_model_con;   /*!< best model */
void *best_model_std_con;  /*!< standard deviation of the best model */

/*!
 *  this function does postprocess for continuum. 
 */
void postprocess_con()
{
  char posterior_sample_file[BRAINS_MAX_STR_LENGTH];
  double temperature = 1.0;
  double *pm, *pmstd;
  int num_ps, i, j;
  void *posterior_sample, *post_model;
  
  best_model_con = malloc(size_of_modeltype);
  best_model_std_con = malloc(size_of_modeltype);

  temperature = parset.temperature;
  dnest_postprocess(temperature);
  
  if(thistask == roottask)
  {
    char fname[200];
    FILE *fp, *fcon;
    // get number of lines in posterior sample file
    get_posterior_sample_file(dnest_options_file, posterior_sample_file);

    //file for posterior sample
    fp = fopen(posterior_sample_file, "r");
    if(fp == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s.\n", posterior_sample_file);
      exit(0);
    }
    //file for continuum reconstruction
    sprintf(fname, "%s/%s", parset.file_dir, "data/con_rec.txt");
    fcon = fopen(fname, "w");
    if(fcon == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
      exit(0);
    }
    // read numbers of points in posterior sample
    if(fscanf(fp, "# %d", &num_ps) < 1)
    {
      fprintf(stderr, "# Error: Cannot read file %s.\n", posterior_sample_file);
      exit(0);
    }
    printf("# Number of points in posterior sample: %d\n", num_ps);

    post_model = malloc(size_of_modeltype);
    posterior_sample = malloc(num_ps * size_of_modeltype);

    which_parameter_update = -1;
    which_particle_update = 0;
    Fcon = Fcon_particles[which_particle_update];

    for(i=0; i<num_ps; i++)
    {
      for(j=0; j<num_params; j++)
      {
        if(fscanf(fp, "%lf", (double *)post_model + j) < 1)
        {
          fprintf(stderr, "# Error: Cannot read file %s.\n", posterior_sample_file);
          exit(0);
        }
      }
      fscanf(fp, "\n");

      memcpy(posterior_sample+i*size_of_modeltype, post_model, size_of_modeltype);

      calculate_con_from_model(post_model);

      if(gsl_rng_uniform(gsl_r) < 1.0)
      {
        for(j=0; j<parset.n_con_recon; j++)
        {
          fprintf(fcon, "%f %f %f\n", Tcon[j], Fcon[j]/con_scale, Fcerrs[j]/con_scale);
        }
        fprintf(fcon, "\n");
      }
    }

    fclose(fp);
    fclose(fcon);

    pm = (double *)best_model_con;
    pmstd = (double *)best_model_std_con;
    for(j=0; j<num_params; j++)
    {
      pm[j] = pmstd[j] = 0.0;
    }
    for(i=0; i<num_ps; i++)
    {
      for(j =0; j<num_params; j++)
        pm[j] += *((double *)posterior_sample + i*num_params + j );
    }

    for(j=0; j<num_params; j++)
      pm[j] /= num_ps;

    for(i=0; i<num_ps; i++)
    {
      for(j=0; j<num_params; j++)
        pmstd[j] += pow( *((double *)posterior_sample + i*num_params + j ) - pm[j], 2.0 );
    }

    for(j=0; j<num_params; j++)
    {
      if(num_ps > 1)
        pmstd[j] = sqrt(pmstd[j]/(num_ps-1.0));
      else
        pmstd[j] = 0.0;
    }  

    for(j = 0; j<num_params_var; j++)
      printf("Best params %d %f +- %f\n", j, *((double *)best_model_con + j), 
                                             *((double *)best_model_std_con+j) ); 

    free(post_model);
    free(posterior_sample);
  }
  return;
}

/*! 
 * This function runs dnest sampling and postprocessing, reconstructs the continuum 
*  using the best estimates of model parameters.
 */
void reconstruct_con()
{
  char *argv[]={""};

  reconstruct_con_init();
  dnest_con(0, argv);

  postprocess_con();

  if(thistask == roottask)
  {
    which_parameter_update = -1;
    which_particle_update = 0;
    Fcon = Fcon_particles[which_particle_update];

    calculate_con_from_model(best_model_con);

    FILE *fp;
    char fname[200];
    int i;
    sprintf(fname, "%s/%s", parset.file_dir, parset.pcon_out_file);
    fp = fopen(fname, "w");
    if(fp == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s\n", fname);
      exit(-1);
    }

    for(i=0; i<parset.n_con_recon; i++)
    {
      fprintf(fp, "%f %f %f\n", Tcon[i], Fcon[i] / con_scale, Fcerrs[i]/con_scale);
    }
    fclose(fp);

    memcpy(var_param, best_model_con, num_params_var*sizeof(double));
    memcpy(var_param_std, best_model_std_con, num_params_var*sizeof(double));
  }

  MPI_Bcast(var_param, num_params_var, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
  MPI_Bcast(var_param_std, num_params_var, MPI_DOUBLE, roottask, MPI_COMM_WORLD);

  reconstruct_con_end();
  return;
}

/*!
 *  this function calculates continuum light curves form model parameters 
 *  and stores it into Fcon.
 */
void calculate_con_from_model(const void *model)
{
  double *Larr, *ybuf, *y, *yu;
  double lambda, ave_con, syserr;

  double *pm = (double *)model;
  double sigma, tau, alpha, mu;
  int i, info;
  
  syserr = 0.0;//exp(pm[0]);
  tau = exp(pm[2]);
  sigma = exp(pm[1]) * sqrt(tau);
  alpha = 1.0;
  mu = pm[3];
  
  Larr = workspace; 
  ybuf = Larr + n_con_data; 
  y = ybuf + n_con_data;
  yu = y + n_con_data; 

  set_covar_Pmat_data(sigma, tau, alpha, syserr);
  set_covar_Umat(sigma, tau, alpha);

  inverse_mat(PCmat_data, n_con_data, &info);

  for(i=0;i<n_con_data;i++)
    Larr[i]=1.0;

  multiply_matvec(PCmat_data, Larr, n_con_data, ybuf);
  lambda = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  multiply_matvec(PCmat_data, Fcon_data, n_con_data, ybuf);
  ave_con = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  ave_con /= lambda;

  ave_con += mu / sqrt(lambda);
  
  for(i=0; i<n_con_data; i++)
    y[i] = Fcon_data[i] - ave_con;

  multiply_matvec(PCmat_data, y, n_con_data, ybuf);
  multiply_matvec_MN(USmat, parset.n_con_recon, n_con_data, ybuf, Fcon);

  multiply_mat_MN(USmat, PCmat_data, PEmat1, parset.n_con_recon, n_con_data, n_con_data);
  multiply_mat_MN_transposeB(PEmat1, USmat, PEmat2, parset.n_con_recon, parset.n_con_recon, n_con_data);

  set_covar_Pmat(sigma, tau, alpha);
  inverse_mat(PSmat, parset.n_con_recon, &info);

  memcpy(PQmat, PSmat, parset.n_con_recon*parset.n_con_recon*sizeof(double));
  for(i=0; i<parset.n_con_recon; i++)
    PQmat[i*parset.n_con_recon+i] += 1.0/(sigma*sigma + syserr*syserr - PEmat2[i*parset.n_con_recon + i]);  
  
  inverse_mat(PQmat, parset.n_con_recon, &info);
  Chol_decomp_L(PQmat, parset.n_con_recon, &info);
  multiply_matvec(PQmat, &pm[num_params_var], parset.n_con_recon, yu);

  // add back the mean of continuum
  for(i=0; i<parset.n_con_recon; i++)
  {
    Fcon[i] += yu[i] + ave_con;
    Fcerrs[i] = sqrt(sigma*sigma + syserr*syserr - PEmat2[i*parset.n_con_recon + i]);
  }

  return;
}

/*!
 * This function calculate continuum ligth curves from varibility parameters.
 */
void reconstruct_con_from_varmodel(double sigma, double tau, double alpha, double syserr)
{
  double *Larr, *ybuf, *y;
  double lambda, ave_con;
  int i, info;
   
  Larr = malloc(n_con_data * sizeof(double));
  ybuf = malloc(n_con_data* sizeof(double));
  y = malloc(n_con_data* sizeof(double));
  
  set_covar_Pmat_data(sigma, tau, alpha, syserr);
  set_covar_Umat(sigma, tau, alpha);

  inverse_mat(PSmat_data, n_con_data, &info);
  
  for(i=0;i<n_con_data;i++)
    Larr[i]=1.0;
  multiply_matvec(PSmat_data, Larr, n_con_data, ybuf);
  lambda = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  multiply_matvec(PSmat_data, Fcon_data, n_con_data, ybuf);
  ave_con = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  ave_con /= lambda;
  
  for(i=0; i<n_con_data; i++)
    y[i] = Fcon_data[i] - ave_con;
  multiply_matvec(PSmat_data, y, n_con_data, ybuf);
  multiply_matvec_MN(USmat, parset.n_con_recon, n_con_data, ybuf, Fcon);

  for(i=0; i<parset.n_con_recon; i++)
    Fcon[i] += ave_con;

  free(Larr);
  free(y);
  free(ybuf);
}

/*!
 * this function calculates likelihood 
 */
double prob_con_variability(const void *model)
{
  double prob = 0.0, fcon, var2;
  int i, param, info;
  double *pm = (double *)model;
  double tau, sigma, alpha, lndet, lambda, ave_con, mu, syserr;
  double *Larr, *ybuf, *y;
  
  if(perturb_accept[which_particle_update] == 1)
  {
    param = which_parameter_update_prev[which_particle_update];
    prob_con_particles[which_particle_update] = prob_con_particles_perturb[which_particle_update];
  }

  if( which_parameter_update < num_params_var)
  {
    syserr = exp(pm[0]);
    tau = exp(pm[2]);
    sigma = exp(pm[1]) * sqrt(tau);
    alpha = 1.0;
    mu = pm[3];
  
    Larr = workspace;
    ybuf = Larr + n_con_data;
    y = ybuf + n_con_data;

    set_covar_Pmat_data(sigma, tau, alpha, syserr);
    memcpy(IPCmat_data, PCmat_data, n_con_data*n_con_data*sizeof(double));

    lndet = lndet_mat(PCmat_data, n_con_data, &info);

    inverse_mat(IPCmat_data, n_con_data, &info);

    for(i=0;i<n_con_data;i++)
      Larr[i]=1.0;

    multiply_matvec(IPCmat_data, Larr, n_con_data, ybuf);
    lambda = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
    multiply_matvec(IPCmat_data, Fcon_data, n_con_data, ybuf);
    ave_con = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
    ave_con /= lambda;

    ave_con += mu / sqrt(lambda);

    for(i=0; i<n_con_data; i++)
      y[i] = Fcon_data[i] - ave_con;
  
    multiply_matvec(IPCmat_data, y, n_con_data, ybuf);
    prob = -0.5 * cblas_ddot(n_con_data, y, 1, ybuf, 1);
    prob += -0.5*lndet;

    prob_con_particles_perturb[which_particle_update] = prob;
  }
  else
  {
    prob = prob_con_particles[which_particle_update];
  }
  /* record the parameter being updated */
  which_parameter_update_prev[which_particle_update] = which_parameter_update;
  return prob;
}

/*!
 * this function calculates likelihood at initital step.
 */
double prob_con_variability_initial(const void *model)
{
  double prob = 0.0, fcon, var2;
  int i, param, info;
  double *pm = (double *)model;
  double tau, sigma, alpha, lndet, lambda, ave_con, mu, syserr;
  double *Larr, *ybuf, *y;
  
  syserr = exp(pm[0]);
  tau = exp(pm[2]);
  sigma = exp(pm[1]) * sqrt(tau);
  alpha = 1.0;
  mu = pm[3];
  
  Larr = workspace; 
  ybuf = Larr + n_con_data; 
  y = ybuf + n_con_data;

  set_covar_Pmat_data(sigma, tau, alpha, syserr);
  memcpy(IPCmat_data, PCmat_data, n_con_data*n_con_data*sizeof(double));

  lndet = lndet_mat(PCmat_data, n_con_data, &info);

  inverse_mat(IPCmat_data, n_con_data, &info);

  for(i=0;i<n_con_data;i++)
    Larr[i]=1.0;

  multiply_matvec(IPCmat_data, Larr, n_con_data, ybuf);
  lambda = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  multiply_matvec(IPCmat_data, Fcon_data, n_con_data, ybuf);
  ave_con = cblas_ddot(n_con_data, Larr, 1, ybuf, 1);
  ave_con /= lambda;

  ave_con += mu / sqrt(lambda);

  for(i=0; i<n_con_data; i++)
    y[i] = Fcon_data[i] - ave_con;
  
  multiply_matvec(IPCmat_data, y, n_con_data, ybuf);
  prob = -0.5 * cblas_ddot(n_con_data, y, 1, ybuf, 1);
  prob += -0.5*lndet;

  prob_con_particles[which_particle_update] = prob;
  return prob;
}

/*!
 * this function sets the covariance matrix at time points for reconstruction.
 * store into PSmat.
 */
void set_covar_Pmat(double sigma, double tau, double alpha)
{
  double t1, t2;
  int i, j;
 
  for(i=0; i<parset.n_con_recon; i++)
  {
    t1 = Tcon[i];
    for(j=0; j<=i; j++)
    {
      t2 = Tcon[j];
      PSmat[i*parset.n_con_recon+j] = sigma*sigma* exp (- pow (fabs(t1-t2) / tau, alpha));
      PSmat[j*parset.n_con_recon+i] = PSmat[i*parset.n_con_recon+j];
    }
  }
  return;
}

/*!
 * this function sets the covariance matrix at data time points 
 */
void set_covar_Pmat_data(double sigma, double tau, double alpha, double syserr)
{
  double t1, t2;
  int i, j;
 
  for(i=0; i<n_con_data; i++)
  { 
    t1 = Tcon_data[i];

    for(j=0; j<i; j++)
    {
      t2 = Tcon_data[j];
      PSmat_data[i*n_con_data+j] = sigma*sigma* exp (- pow (fabs(t1-t2) / tau, alpha));
      PSmat_data[j*n_con_data+i] = PSmat_data[i*n_con_data+j];

      PNmat_data[i*n_con_data+j] = PNmat_data[j*n_con_data+i] = 0.0;

      PCmat_data[i*n_con_data+j] = PCmat_data[j*n_con_data+i] = PSmat_data[i*n_con_data+j];
    }

    PSmat_data[i*n_con_data+i] = sigma * sigma;
    PNmat_data[i*n_con_data+i] = Fcerrs_data[i]*Fcerrs_data[i];// + syserr*syserr;
    PCmat_data[i*n_con_data+i] = PSmat_data[i*n_con_data+i] + PNmat_data[i*n_con_data+i];
  }
  return;
}

/*!
 * this function sets the covariance matrix at time of data points and reconstruction points
 */
void set_covar_Umat(double sigma, double tau, double alpha)
{
  double t1, t2;
  int i, j;
 
  for(i=0; i<parset.n_con_recon; i++)
  {
    t1 = Tcon[i];
    for(j=0; j<n_con_data; j++)
    {
      t2 = Tcon_data[j];
      USmat[i*n_con_data+j] = sigma*sigma * exp (- pow (fabs(t1-t2) / tau, alpha) );
    }
  }
  return;
}

/*!
 * this function initializes the continuum reconstruction.
 */
void reconstruct_con_init()
{
  int i;
  double dT, Tspan;

  Tspan = Tcon_data[n_con_data -1] - Tcon_data[0];
  /* set time array for continuum */
  Tcon_min = Tcon_data[0] - fmax(0.05*Tspan, 10.0);
  Tcon_max = Tcon_data[n_con_data-1] + fmax(0.05*Tspan, 10.0);
  dT = (Tcon_max - Tcon_min)/(parset.n_con_recon -1);
  
  for(i=0; i<parset.n_con_recon; i++)
  {
    Tcon[i] = Tcon_min + i*dT;
  }

  sprintf(dnest_options_file, "%s/%s", parset.file_dir, "src/OPTIONSCON");
  if(thistask == roottask)
  {
    get_num_particles(dnest_options_file);
  }
  MPI_Bcast(&parset.num_particles, 1, MPI_INT, roottask, MPI_COMM_WORLD);

  perturb_accept = malloc(parset.num_particles * sizeof(int));
  which_parameter_update_prev = malloc(parset.num_particles * sizeof(int));
  for(i=0; i<parset.num_particles; i++)
  {
    perturb_accept[i] = 0;
    which_parameter_update_prev[i] = -1;
  }

  Fcon_particles = malloc(parset.num_particles * sizeof(double *));
  Fcon_particles_perturb = malloc(parset.num_particles * sizeof(double *));
  for(i=0; i<parset.num_particles; i++)
  {
    Fcon_particles[i] = malloc(parset.n_con_recon * sizeof(double));
    Fcon_particles_perturb[i] = malloc(parset.n_con_recon * sizeof(double));
  }
  prob_con_particles = malloc(parset.num_particles * sizeof(double));
  prob_con_particles_perturb = malloc(parset.num_particles * sizeof(double));
  return;
}

/*!
 * this function finalize the continuum reconstruction.
 */
void reconstruct_con_end()
{
  int i;

  for(i=0; i<parset.num_particles; i++)
  {
    free(Fcon_particles[i]);
    free(Fcon_particles_perturb[i]);
  }
  free(Fcon_particles);
  free(Fcon_particles_perturb);
  
  free(prob_con_particles);
  free(prob_con_particles_perturb);

  free(perturb_accept);
  free(which_parameter_update_prev);
  free(best_model_con);
  free(best_model_std_con);
  return;
}
