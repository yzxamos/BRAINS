/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file dnest_con.h
 *  \brief header file.
 */

#ifndef _DNEST_CON_H

#include <stdbool.h>

/*===========================================*/
// users responsible for following struct definitions 

// model

/*==========================================*/
extern int size_of_modeltype;

/* best model */
extern void *best_model_con, *best_model_std_con;

/* functions */
void from_prior_con(void *model);
void print_particle_con(FILE *fp, const void *model);
double log_likelihoods_cal_con(const void *model);
double perturb_con(void *model);
void copy_model_con(void *dest, const void *src);
void* create_model_con();
int get_num_params_con();

void (*print_particle)(FILE *fp, const void *model);
void (*from_prior)(void *model);
double (*log_likelihoods_cal)(const void *model);
double (*perturb)(void *model);
void (*copy_model)(void *dest, const void *src);
void* (*create_model)();
int (*get_num_params)();
#endif
