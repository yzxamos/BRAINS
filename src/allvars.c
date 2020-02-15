/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file allvars.c
 *  \brief define all variables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "allvars.h"

/* MPICH */
int thistask, totaltask, namelen;
int roottask = 0;
char proc_name[MPI_MAX_PROCESSOR_NAME];

PARSET parset;


double VelUnit, C_Unit;    /*!< Velocity unit = \f$\sqrt{\frac{G\cdot 10^6M_\odot}{1~\rm light-day}}/10^5\f$km/s, 
                        with \f$ M_\bullet = 10^6M_\odot \f$ and \f$r=1\f$light day.*/

/* Data */
int n_con_data;    /*!< number of continuum data points */
int n_line_data;   /*!< number of emission data points along time axis */
int n_vel_data;    /*!< number of emission data points along velocity axis */
int n_vel_data_incr=5, n_vel_data_ext;
int n_con_max;
double *Tcon_data, *Fcon_data,  *Fcerrs_data;
double *Tline_data, *Fline_data, *Flerrs_data;
double *Vline_data, *Fline2d_data, *Flerrs2d_data;
double *Vline_data_ext;
double con_scale, line_scale;

double con_error_mean, line_error_mean;

char dnest_options_file[BRAINS_MAX_STR_LENGTH];

int which_parameter_update;  /*!< which parameter being updated */
int which_particle_update;   /*!< which particle being updated */
int which_level_update;      /*!< which level of the particle */

/* continuum reconstruction */
int nq;
double *Tcon, *Fcon, *Fcerrs;
double Tcon_min, Tcon_max;
double *PSmat, *PNmat, *USmat, *PSmat_data;
double *PNmat_data;
double *PCmat_data, *IPCmat_data, *PQmat, *PEmat1, *PEmat2;
double *workspace, *workspace_uv;
double *var_param, *var_param_std;
double *con_q;
double logz_con;
double *Larr_data, *Larr_rec;
double *pow_Tcon_data;

double Tspan_data, Tspan_data_con, Tcad_data, Tmed_data;

/* line reconstruction */
double *Fline_at_data;
double *Tline, *Fline, *Flerrs;
double Tline_min, Tline_max;
double logz_line;

/* line 2d reconstruction */
double *Fline2d_at_data;
double *Fline2d, *Flerrs2d;
double logz_line2d;

// BLR
int BLRmodel_size;
int *par_fix, npar_fix;
double *par_fix_val;
double **blr_range_model, **par_range_model;
int *par_prior_model;
double **par_prior_gaussian;
double nlr_range_model[3][2];
int nlr_prior_model[3];
double mass_range[2];
double sys_err_line_range[2];
double resp_range[2][2];

int num_params_blr_tot;
int num_params, num_params_blr, num_params_blr_model, num_params_var, num_params_difftrend, num_params_nlr, num_params_res;
int num_params_drw, num_params_trend;
int num_params_linecenter;

// continuum variation
double var_range_model[15][2]; /*!< define the range of variability parameters */

// epoch dependent spectral broadening
double *instres_epoch, *instres_err_epoch;

/* transfer function / velocity-delay map */
double *TransTau, *TransV, *Trans1D, *Trans2D_at_veldata, *Trans2D;
double rcloud_min_set, rcloud_max_set, time_back_set;

double **Fcon_particles, **Fcon_particles_perturb;
double *prob_con_particles, *prob_con_particles_perturb;
double **con_q_particles, **con_q_particles_perturb;

int force_update;
double **TransTau_particles, **TransTau_particles_perturb;
double **Trans1D_particles, **Trans1D_particles_perturb;
double **Trans2D_at_veldata_particles, **Trans2D_at_veldata_particles_perturb;
double **Fline_at_data_particles, **Fline_at_data_particles_perturb;

double *clouds_tau, *clouds_weight, *clouds_vel;

FILE *fcloud_out = NULL;
int icr_cloud_save = 1;

int postprc;

/* GSL */
const gsl_rng_type * gsl_T;
gsl_rng * gsl_r;

gsl_interp_accel *gsl_acc;
gsl_interp  *gsl_linear;

#ifdef SA

double PhaseFactor;

int num_params_rm;
int num_params_sa, num_params_sa_blr_model, num_params_sa_extpar, num_params_sa_blr;
int n_epoch_sa_data, n_vel_sa_data, n_base_sa_data;
double *vel_sa_data, *base_sa_data, *Fline_sa_data, *Flerrs_sa_data, *phase_sa_data, *pherrs_sa_data;
double *wave_sa_data;

double sa_flux_norm;

double **phase_sa_particles, **Fline_sa_particles;
double **phase_sa_particles_perturb, **Fline_sa_particles_perturb;
double *phase_sa, *Fline_sa;

double *clouds_alpha, *clouds_beta;

double **sa_extpar_range, **sa_blr_range_model;

int SABLRmodel_size;

int *idx_sa_par_mutual, *idx_rm_par_mutual;

double *prob_sa_particles, *prob_sa_particles_perturb;

double *workspace_phase;
#endif