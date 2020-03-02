/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)n AGNs with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file reconstruct_sa.c
 *  \brief reconstruct sa and RM data.
 */

#ifdef SA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <gsl/gsl_interp.h>

#include "brains.h"

void *best_model_sa;      /*!< best model */
void *best_model_std_sa;  /*!< standard deviation of the best model */

/*!
 * postprocessing.
 */
void postprocess_sa()
{
  char posterior_sample_file[BRAINS_MAX_STR_LENGTH];
  int num_ps, i, j, k, nc;
  double *pm, *pmstd;
  double *lag;
  void *posterior_sample, *post_model;
  double mean_lag, mean_lag_std, sum1, sum2;
  int size_of_modeltype = num_params * sizeof(double);
  
  best_model_sa = malloc(size_of_modeltype);
  best_model_std_sa = malloc(size_of_modeltype);

  if(thistask == roottask)
  {
    // initialize smoothing workspace
    char fname[200];
    FILE *fp, *fline, *fsa;

    // get number of lines in posterior sample file
    get_posterior_sample_file(dnest_options_file, posterior_sample_file);

    //file for posterior sample
    fp = fopen(posterior_sample_file, "r");
    if(fp == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s.\n", posterior_sample_file);
      exit(0);
    }
     
    //file for line reconstruction
    sprintf(fname, "%s/%s", parset.file_dir, "data/sa_line_rec.txt");
    fline = fopen(fname, "w");
    if(fline == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
      exit(0);
    }

    //file for phase
    sprintf(fname, "%s/%s", parset.file_dir, "data/sa_phase_rec.txt");
    fsa = fopen(fname, "w");
    if(fsa == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
      exit(0);
    }

    // read number of lines
    if(fscanf(fp, "# %d", &num_ps) < 1)
    {
      fprintf(stderr, "# Error: Cannot read file %s.\n", posterior_sample_file);
      exit(0);
    }
    printf("# Number of points in posterior sample: %d\n", num_ps);

    post_model = malloc(size_of_modeltype);
    posterior_sample = malloc(num_ps * size_of_modeltype);

    force_update = 1;
    which_parameter_update = -1; // force to update the transfer function
    which_particle_update = 0;
    nc = 0;

    Fline_sa = Fline_sa_particles[which_particle_update];
    phase_sa = phase_sa_particles[which_particle_update];
    
    for(i=0; i<num_ps; i++)
    {
      // read lines
      for(j=0; j<num_params; j++)
      {
        if(fscanf(fp, "%lf", (double *)post_model + j) < 1)
        {
          fprintf(stderr, "# Error: Cannot read file %s.\n", posterior_sample_file);
          exit(0);
        }
      }
      fscanf(fp, "\n");

      //store model
      memcpy(posterior_sample+i*size_of_modeltype, post_model, size_of_modeltype);
    
      calculate_sa_from_blrmodel(post_model, 0);
      
      //if( i % (num_ps/10+1) == 0)  
      {
        // output sa line
        for(j=0; j<n_vel_sa_data; j++)
        {
          fprintf(fline, "%e %e\n", wave_sa_data[j], Fline_sa[j]);
        }
        fprintf(fline, "\n");

        // output sa phase
        for(k=0; k<n_base_sa_data; k++)
        {
          for(j=0; j<n_vel_sa_data; j++)
          {
            fprintf(fsa, "%e %e\n", wave_sa_data[j], phase_sa[k*n_vel_sa_data + j]/(PhaseFactor * wave_sa_data[j]) );
          }
        }
        fprintf(fsa, "\n");
      }
    }

    fclose(fp);
    fclose(fline);
    fclose(fsa);

    pm = (double *)best_model_sa;
    pmstd = (double *)best_model_std_sa;
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

    for(j = 0; j<num_params_sa; j++)
      printf("Best params %d %f +- %f\n", j, *((double *)best_model_sa + j), 
                                             *((double *)best_model_std_sa+j) ); 
 
    free(post_model);
    free(posterior_sample);
  }
  return;
}

/*!
 * this function run dnest sampleing, reconstruct light curves using the best estimates for parameters.
 */
void reconstruct_sa()
{
  int i, argc=0;
  char **argv;

  //configure restart of dnest
  argv = malloc(9*sizeof(char *));
  for(i=0; i<9; i++)
  {
    argv[i] = malloc(BRAINS_MAX_STR_LENGTH*sizeof(char));
  }
  //setup argc and argv
  strcpy(argv[argc++], "dnest");
  strcpy(argv[argc++], "-s");
  strcpy(argv[argc], parset.file_dir);
  strcat(argv[argc++], "/data/restartsa_dnest.txt");

  if(parset.flag_restart == 1)
  {
    strcpy(argv[argc++], "-r");
    strcpy(argv[argc], parset.file_dir);
    strcat(argv[argc], "/");
    strcat(argv[argc++], "data/restartsa_dnest.txt");
  }
  if(parset.flag_postprc == 1)
  {
    strcpy(argv[argc++], "-p");
  }
  if(parset.flag_temp == 1)
  {
    sprintf(argv[argc++], "-t%f", parset.temperature);
  }
  if(parset.flag_sample_info == 1)
  {
    strcpy(argv[argc++], "-c");
  }
  
  //level-dependent sampling
  {
    strcpy(argv[argc++], "-l");
  }
  
  reconstruct_sa_init();

  dnest_sa(argc, argv);

  if(parset.flag_exam_prior != 1 && parset.flag_para_name != 1)
  {
    postprocess_sa();

    if(thistask == roottask)
    {
      FILE *fp;
      char fname[200];
      int j, k;

      force_update = 1;
      which_parameter_update = -1; // force to update the transfer function
      which_particle_update = 0;

      Fline_sa = Fline_sa_particles[which_particle_update];
      phase_sa = phase_sa_particles[which_particle_update];
      
      calculate_sa_from_blrmodel(best_model_sa, 1);

      sprintf(fname, "%s/%s", parset.file_dir, "data/psa_line.txt");
      fp = fopen(fname, "w");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
        exit(0);
      }
      // output sa line
      for(j=0; j<n_vel_sa_data; j++)
      {
        fprintf(fp, "%e %e\n", wave_sa_data[j], Fline_sa[j]);
      }
      fclose(fp);

      //file for phase
      sprintf(fname, "%s/%s", parset.file_dir, "data/psa_phase.txt");
      fp = fopen(fname, "w");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
        exit(0);
      }
      for(k=0; k<n_base_sa_data; k++)
      {
        for(j=0; j<n_vel_sa_data; j++)
        {
          fprintf(fp, "%e %e\n", wave_sa_data[j], phase_sa[k*n_vel_sa_data + j]/(PhaseFactor * wave_sa_data[j]) );
        }
        fprintf(fp, "\n");
      }
      fclose(fp);
    }
  }

  reconstruct_sa_end();

  //clear up argv
  for(i=0; i<9; i++)
  {
    free(argv[i]);
  }
  free(argv);

  return;
}

void reconstruct_sa_init()
{
  int i, j;
  
  sprintf(dnest_options_file, "%s/%s", parset.file_dir, "src/OPTIONSSA");
  if(thistask == roottask)
  {
    get_num_particles(dnest_options_file);
  }
  MPI_Bcast(&parset.num_particles, 1, MPI_INT, roottask, MPI_COMM_WORLD);

  phase_sa_particles = malloc(parset.num_particles * sizeof(double *));
  phase_sa_particles_perturb = malloc(parset.num_particles * sizeof(double *));
  for(i=0; i<parset.num_particles; i++)
  {
    phase_sa_particles[i] = malloc(n_base_sa_data * n_vel_sa_data * sizeof(double));
    phase_sa_particles_perturb[i] = malloc(n_base_sa_data * n_vel_sa_data * sizeof(double));
  }

  Fline_sa_particles = malloc(parset.num_particles * sizeof(double *));
  Fline_sa_particles_perturb = malloc(parset.num_particles * sizeof(double *));
  for(i=0; i<parset.num_particles; i++)
  {
    Fline_sa_particles[i] = malloc(n_vel_sa_data * sizeof(double));
    Fline_sa_particles_perturb[i] = malloc(n_vel_sa_data * sizeof(double));
  }

  /* cloud sample related */
  clouds_weight = malloc(parset.n_cloud_per_task * sizeof(double));
  clouds_alpha = malloc(parset.n_cloud_per_task * sizeof(double));
  clouds_beta = malloc(parset.n_cloud_per_task * sizeof(double));
  clouds_vel = malloc(parset.n_cloud_per_task * parset.n_vel_per_cloud * sizeof(double));

  workspace_phase = malloc( (3*n_vel_sa_data)* sizeof(double));

  if(parset.flag_save_clouds && thistask == roottask)
  {
    if(parset.n_cloud_per_task <= 1000)
      icr_cloud_save = 1;
    else
      icr_cloud_save = parset.n_cloud_per_task/1000;

    char fname[200];
    sprintf(fname, "%s/%s", parset.file_dir, parset.cloud_out_file);
    fcloud_out = fopen(fname, "w");
    if(fcloud_out == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s\n", fname);
      exit(-1);
    }
  }
  return;
}

void reconstruct_sa_end()
{
  int i;

  for(i=0; i<parset.num_particles; i++)
  {
    free(phase_sa_particles[i]);
    free(phase_sa_particles_perturb[i]);

    free(Fline_sa_particles[i]);
    free(Fline_sa_particles_perturb[i]);
  }
  free(phase_sa_particles);
  free(phase_sa_particles_perturb);

  free(Fline_sa_particles);
  free(Fline_sa_particles_perturb);

  for(i=0; i<num_params; i++)
  {
    free(par_range_model[i]);
    free(par_prior_gaussian[i]);
  }
  free(par_range_model);
  free(par_prior_gaussian);
  free(par_prior_model);

  free(par_fix);
  free(par_fix_val);
  free(best_model_sa);
  free(best_model_std_sa);
  
  /* clouds sample related */
  free(clouds_weight);
  free(clouds_alpha);
  free(clouds_beta);
  free(clouds_vel);
  
  free(workspace_phase);

  if(parset.flag_save_clouds && thistask==roottask)
  {
    fclose(fcloud_out);
  }
  if(thistask == roottask)
  {
    printf("Ends reconstruct_sa.\n");
  }
  return;
}  

/*!
 * this function calculate probability.
 * 
 * At each MCMC step, only one parameter is updated, which only changes some values; thus,
 * optimization that reuses the unchanged values can improve computation efficiency.
 */
double prob_sa(const void *model)
{
  double prob_sa = 0.0, var2, dy, var2_se;
  int i, j;
  double *pm = (double *)model;
  
  which_particle_update = dnest_get_which_particle_update();
  phase_sa = phase_sa_particles_perturb[which_particle_update];
  Fline_sa = Fline_sa_particles_perturb[which_particle_update];
  
  calculate_sa_from_blrmodel(model, 0);

  for(j=0; j<n_epoch_sa_data; j++)
  {
    for(i=0; i<n_vel_sa_data; i++)
    {
      dy = Fline_sa[i] - Fline_sa_data[i + j*n_vel_sa_data];
      var2 = Flerrs_sa_data[i+j*n_vel_sa_data]*Flerrs_sa_data[i+j*n_vel_sa_data];
      prob_sa += -0.5 * (dy*dy)/var2 - 0.5*log(var2 * 2.0*PI);
    }
  }

  for(j=0; j<n_base_sa_data; j++)
  {
    for(i=0; i<n_vel_sa_data; i++)
    {
      dy = phase_sa[j*n_vel_sa_data + i] - phase_sa_data[j*n_vel_sa_data + i];
      var2 = pherrs_sa_data[j*n_vel_sa_data + i] * pherrs_sa_data[j*n_vel_sa_data + i];
      prob_sa += -0.5 * (dy*dy)/var2 - 0.5*log(var2 * 2.0*PI);
    }
  }

  return prob_sa;
}

#endif