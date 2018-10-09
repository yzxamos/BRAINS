/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file dnest_line1d.h
 *  \brief header file.
 */

#ifndef _DNEST_LINE1D_H
#define _DNEST_LINE1D_H

#include <stdbool.h>

/* functions */
void from_prior_line1d(void *model);
void print_particle_line1d(FILE *fp, const void *model);
double log_likelihoods_cal_line1d(const void *model);
double log_likelihoods_cal_initial_line1d(const void *model);
double log_likelihoods_cal_restart_line1d(const void *model);
double perturb_line1d(void *model);
double log_likelihoods_cal_line1d_exam(const void *model);

#endif