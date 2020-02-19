/*
 * BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)n AGNs with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 *  \file read.c
 *  \brief read configuration file and data files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "allvars.h"
#include "proto.h"

/*!
 * read parameter set from parameter file.
 */
void read_parset()
{
  int error_flag = 0;

  if(thistask == roottask)
  {
    FILE *fparam;
    int i, j, nt;
    char str[200], buf1[200], buf2[200], buf3[200];

    nt = 0;
    strcpy(pardict[nt].tag, "FileDir");
    pardict[nt].addr = &parset.file_dir;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "ContinuumFile");
    pardict[nt].addr= &parset.continuum_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "LineFile");
    pardict[nt].addr= &parset.line_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "Line2DFile");
    pardict[nt].addr= &parset.line2d_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "FlagDim");
    pardict[nt].addr= &parset.flag_dim;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "NConRecon");
    pardict[nt].addr= &parset.n_con_recon;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagTrend");
    pardict[nt].addr= &parset.flag_trend;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "ConConstructFileOut");
    pardict[nt].addr= &parset.pcon_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "NLineRecon");
    pardict[nt].addr= &parset.n_line_recon;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "NVelRecon");
    pardict[nt].addr= &parset.n_vel_recon;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "LineConstructFileOut");
    pardict[nt].addr= &parset.pline_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "TranFileOut");
    pardict[nt].addr= &parset.tran_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "Line2DConstructFileOut");
    pardict[nt].addr= &parset.pline2d_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "Line2DDataConstructFileOut");
    pardict[nt].addr= &parset.pline2d_data_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "Tran2DFileOut");
    pardict[nt].addr= &parset.tran2d_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "Tran2DDataFileOut");
    pardict[nt].addr= &parset.tran2d_data_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "NCloudPerCore");
    pardict[nt].addr= &parset.n_cloud_per_task;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "NVPerCloud");
    pardict[nt].addr= &parset.n_vel_per_cloud;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "NTau");
    pardict[nt].addr= &parset.n_tau;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "RCloudMax");
    pardict[nt].addr= &parset.rcloud_max;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "TimeBack");
    pardict[nt].addr= &parset.time_back;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "FlagCloudsOut");
    pardict[nt].addr= &parset.flag_save_clouds;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "CloudsFileOut");
    pardict[nt].addr= &parset.cloud_out_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "FlagCloudsForceUpdate");
    pardict[nt].addr= &parset.flag_force_update;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagConSysErr");
    pardict[nt].addr= &parset.flag_con_sys_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagLineSysErr");
    pardict[nt].addr= &parset.flag_line_sys_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagInstRes");
    pardict[nt].addr= &parset.flag_InstRes;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "InstRes");
    pardict[nt].addr= &parset.InstRes;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "InstResErr");
    pardict[nt].addr= &parset.InstRes_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "InstResFile");
    pardict[nt].addr= &parset.file_instres;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "FlagNarrowLine");
    pardict[nt].addr= &parset.flag_narrowline;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FluxNarrowLine");
    pardict[nt].addr= &parset.flux_narrowline;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "FluxNarrowLineErr");
    pardict[nt].addr= &parset.flux_narrowline_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "WidthNarrowLine");
    pardict[nt].addr= &parset.width_narrowline;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "WidthNarrowLineErr");
    pardict[nt].addr= &parset.width_narrowline_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "ShiftNarrowLine");
    pardict[nt].addr= &parset.shift_narrowline;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "ShiftNarrowLineErr");
    pardict[nt].addr= &parset.shift_narrowline_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "BLRParFix");
    pardict[nt].addr= &parset.str_par_fix;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "BLRParFixVal");
    pardict[nt].addr= &parset.str_par_fix_val;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "FlagFixVar");
    pardict[nt].addr= &parset.flag_fixvar;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagBLRModel");
    pardict[nt].addr= &parset.flag_blrmodel;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagTrendDiff");
    pardict[nt].addr= &parset.flag_trend_diff;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagLineCenter");
    pardict[nt].addr= &parset.flag_linecenter;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "LineCenterErr");
    pardict[nt].addr= &parset.linecenter_err;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "FlagNonlinear");
    pardict[nt].addr= &parset.flag_nonlinear;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "LineCenter");
    pardict[nt].addr= &parset.linecenter;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "Redshift");
    pardict[nt].addr= &parset.redshift;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

#ifdef SA
    strcpy(pardict[nt].tag, "SAFile");
    pardict[nt].addr= &parset.sa_file;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "FlagSABLRModel");
    pardict[nt].addr= &parset.flag_sa_blrmodel;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "FlagSAParMutual");
    pardict[nt].addr= &parset.flag_sa_par_mutual;
    pardict[nt].isset = 0;
    pardict[nt++].id = INT;

    strcpy(pardict[nt].tag, "SALineCenter");
    pardict[nt].addr= &parset.sa_linecenter;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;
    
    strcpy(pardict[nt].tag, "SAInstRes");
    pardict[nt].addr= &parset.sa_InstRes;
    pardict[nt].isset = 0;
    pardict[nt++].id = DOUBLE;

    strcpy(pardict[nt].tag, "SABLRParFix");
    pardict[nt].addr= &parset.sa_str_par_fix;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;

    strcpy(pardict[nt].tag, "SABLRParFixVal");
    pardict[nt].addr= &parset.sa_str_par_fix_val;
    pardict[nt].isset = 0;
    pardict[nt++].id = STRING;
#endif

    num_pardict = nt;
    /* default values */
    parset.flag_dim = 0;
    parset.flag_trend = 0;
    parset.flag_narrowline = 0;
    parset.flag_fixvar = 0;
    parset.flag_blrmodel = 1;
    parset.flag_trend_diff = 0;
    parset.flag_narrowline = 0;
    parset.flag_linecenter = 0;
    parset.flag_force_update= 0;
    parset.flag_con_sys_err = 0;
    parset.flag_line_sys_err = 0;
    parset.flag_nonlinear = 1;
    parset.flag_InstRes = 0;
    parset.InstRes = 0.0;
    parset.InstRes_err = 0.0;
    parset.redshift = 0.0;
    parset.linecenter = 4861.0;
    parset.n_cloud_per_task = 0;
    parset.n_vel_per_cloud = 1;

    strcpy(parset.continuum_file, "");
    strcpy(parset.line_file, "");
    strcpy(parset.line2d_file, "");
    strcpy(parset.file_instres, "");
    strcpy(parset.pcon_out_file, "data/pcon.txt");
    strcpy(parset.pline_out_file, "data/pline.txt");
    strcpy(parset.pline2d_out_file, "data/pline2d_data.txt");
    strcpy(parset.pline2d_data_out_file, "data/pline2d_data.txt");
    strcpy(parset.cloud_out_file, "data/clouds.txt");
    strcpy(parset.tran_out_file, "data/tran.txt");
    strcpy(parset.tran2d_out_file, "data/tran2d.txt");
    strcpy(parset.tran2d_data_out_file, "data/tran2d_data.txt");
    strcpy(parset.str_par_fix,"");
    strcpy(parset.str_par_fix_val,"");

#ifdef SA
    parset.flag_sa_blrmodel = 1;
    parset.flag_sa_par_mutual = 0;
    parset.sa_linecenter = 1.875; 
    parset.sa_InstRes = 0.0;
    strcpy(parset.sa_file, "");
    strcpy(parset.sa_str_par_fix,"");
    strcpy(parset.sa_str_par_fix_val,"");
#endif 

    char fname[200];
    sprintf(fname, "%s", parset.param_file);
    
    fparam = fopen(fname, "r");
    if(fparam == NULL)
    {
      fprintf(stderr, "# Error: Cannot open file %s\n", fname);
      error_flag = 2;
    }

    while(!feof(fparam))
    {
      sprintf(str,"empty");

      fgets(str, 200, fparam);
      if(sscanf(str, "%s%s%s", buf1, buf2, buf3)<2)
        continue;
      if(buf1[0]=='%')
        continue;
      for(i=0, j=-1; i<nt; i++)
        if(strcmp(buf1, pardict[i].tag) == 0 && pardict[i].isset == 0)
        {
          j = i;
          pardict[i].isset = 1;
          //printf("%s %s\n", buf1, buf2);
          break;
        }
      if(j >=0)
      {
        switch(pardict[j].id)
        {
          case DOUBLE:
            *((double *) pardict[j].addr) = atof(buf2);
            break;
          case STRING:
            strcpy(pardict[j].addr, buf2);
            break;
          case INT:
            *((int *)pardict[j].addr) = (int) atof(buf2);
            break;
        }
      }
      else
      {
        fprintf(stderr, "# Error in file %s: Tag '%s' is not allowed or multiple defined.\n", 
                      parset.param_file, buf1);
        error_flag = 3;
      }
    }
    fclose(fparam);

    /* check input options */
#ifndef SA
    if(parset.flag_dim > 2 || parset.flag_dim < -2)
    {
      fprintf(stderr, "# Error in FlagDim: value %d is not allowed.\n# Please specify a value in [-2-2].\n", parset.flag_dim);
      error_flag = 1;
    }
#else 
    if(parset.flag_dim > 5 || parset.flag_dim < -2)
    {
      fprintf(stderr, "# Error in FlagDim: value %d is not allowed.\n# Please specify a value in [-2-5].\n", parset.flag_dim);
      error_flag = 1;
    }
#endif

    if(parset.flag_trend > 2 || parset.flag_trend < 0)
    {
      fprintf(stderr, "# Error in FlagTrend: value %d is not allowed.\n# Please specify a value in [0-1].\n", parset.flag_trend);
      error_flag = 1;
    }

    if(parset.flag_blrmodel > 9 || parset.flag_blrmodel < -1)
    {
      fprintf(stderr, "# Error in FlagBLRModel: value %d is not allowed.\n# Please specify a value in [-1-9].\n", parset.flag_blrmodel);
      error_flag = 1;
    }
    
    /* check linecenter */
    if(parset.linecenter <= 0.0)
    {
      fprintf(stderr, "# Error in LineCenter: value %f is not allowed.\n"
        "# Please specify a positive value.\n", parset.linecenter);
        error_flag = 1;
    }

    /* check redshift */
    if(parset.redshift < 0.0)
    {
      fprintf(stderr, "# Error in redshift: value %f is not allowed.\n"
        "# Please specify a non-negative value.\n", parset.redshift);
        error_flag = 1;
    }
      
    if((parset.flag_narrowline > 3 || parset.flag_narrowline < 0) && (parset.flag_dim == 2 || parset.flag_dim == 5))
    {
      fprintf(stderr, "# Error in FlagNarrowLine: value %d is not allowed.\n# Please specify a value in [0-3].\n", parset.flag_narrowline);
      error_flag = 1;
    }

    if((parset.flag_trend_diff < 0 || parset.flag_trend_diff > 10) && parset.flag_dim > 0)
    {
      fprintf(stderr, "# Error in FlagTrendDiff: value %d is not allowed.\n# Please specify a value in [0-10].\n", parset.flag_trend_diff);
      error_flag = 1;
    }

    if((parset.flag_InstRes < 0 || parset.flag_InstRes > 2) && (parset.flag_dim == 2 || parset.flag_dim == 5|| parset.flag_dim < 0))
    {
      fprintf(stderr, "# Error in FlagInstRes: value %d is not allowed.\n# Please specify a value in [0-2].\n", parset.flag_InstRes);
      error_flag = 1;
    }

    if((parset.flag_linecenter < -1 || parset.flag_linecenter > 1) && (parset.flag_dim == 2 || parset.flag_dim == 5))
    {
      fprintf(stderr, "# Error in FlagLineCenter: value %d is not allowed.\n# Please specify a value in [-1-1].\n", parset.flag_linecenter);
      error_flag = 1;
    }
  
    if(parset.InstRes < 0.0 && (parset.flag_dim == 2 || parset.flag_dim == 5 || parset.flag_dim < 0) )
    {
      fprintf(stderr, "# Error in InstRes: value %f is not allowed.\n# Please specify a positive value.\n", parset.InstRes);
      error_flag = 1;
    }

    if(parset.InstRes_err < 0.0 && (parset.flag_dim == 2 || parset.flag_dim == 5 || parset.flag_dim < 0))
    {
      fprintf(stderr, "# Error in InstResErr: value %f is not allowed.\n# Please specify a positive value.\n", parset.InstRes_err);
      error_flag = 1;
    }

    /* check whether necessary files provided */
    if(parset.flag_dim >= -1 && parset.flag_dim != 3)
    {
      if(strlen(parset.continuum_file) == 0)
      {
        fprintf(stderr, "# Please specify continuum data file in parameter file.\n");
        error_flag = 4;
      }
    }
    if(parset.flag_dim == 1)
    {
      if(strlen(parset.line_file) == 0)
      {
        fprintf(stderr, "# Please specify 1D line data file in parameter file.\n");
        error_flag = 4;
      }
    }
    if(parset.flag_dim == -1 || parset.flag_dim == 2 || parset.flag_dim == 5)
    {
      if(strlen(parset.line2d_file) == 0)
      {
        fprintf(stderr, "# Please specify 2D line data file in parameter file.\n");
        error_flag = 4;
      }
    }

    if(parset.flag_dim > 0)
    {
      if(parset.n_cloud_per_task <= 1)
      {
        fprintf(stderr, "# Error in NCloudPerCore: value %d is not allowed.\n# Please specify a larger number.\n", 
                          parset.n_cloud_per_task);
        error_flag = 1;
      }
    }
  
    if( error_flag == 0 )
    {
      /* narrow line, line center, and InstRes only apply in 2D RM */
      if(parset.flag_dim < 2)
      {
        parset.flag_narrowline = 0;
        parset.flag_linecenter = 0;
        parset.flag_InstRes = 0;
      }
      if(parset.flag_dim ==2 || parset.flag_dim == 5)
      {
        if(parset.flag_narrowline == 0)
        {
          printf("# No narrow-line.\n");
          parset.width_narrowline = 0.0;
          
        }
        else if(parset.flag_narrowline == 1)
        {
          printf("# add fixed narrow-line: flux=%e, width=%fkm/s, shift=%fkm/s.\n", parset.flux_narrowline, 
             parset.width_narrowline, parset.shift_narrowline);
        }
        else if(parset.flag_narrowline == 2 )
        {
          printf("# add narrow-line with Gaussian priors: flux=%e, width=%fkm/s, shift=%fkm/s.\n", parset.flux_narrowline, 
             parset.width_narrowline, parset.shift_narrowline);
        }
        else
        {
          printf("# add narrow-line with logrithmic prior of flux and Gaussian priors of width and shift: width=%fkm/s, shift=%fkm/s.\n",
             parset.width_narrowline, parset.shift_narrowline);
        }
  
        if(parset.width_narrowline<=0.0 && parset.flag_narrowline > 0)
        {
          printf("# Error in narrow line width %f.\n", parset.width_narrowline);
          error_flag = 1;
        }
  
        parset.width_narrowline /= VelUnit;
        parset.width_narrowline_err /= VelUnit;
  
        parset.shift_narrowline /= VelUnit;
        parset.shift_narrowline_err /= VelUnit;
  
        if(parset.flag_InstRes > 1) /* epoch-dependent spectral broadening */
        {
          if(strlen(parset.file_instres) == 0)
          {
            printf("# Error in file_instres, not specified.\n");
            error_flag = 4;
          }
          printf("# use epoch dependent spectral resolution, stored at %s.\n", parset.file_instres);
        }
        else
        {
          parset.InstRes /= VelUnit;
          parset.InstRes_err /= VelUnit;
  
          if(parset.width_narrowline > parset.InstRes)
          {
            printf("# Error narrow line width %f should be smaller than InstRes %f. \n", 
                    parset.width_narrowline*VelUnit, parset.InstRes*VelUnit);
            error_flag = 1;
          }
        }
      }
  
      if(parset.flag_dim < 1)
      {
        parset.flag_trend_diff = 0;
        parset.flag_nonlinear = 0;
      }
  
      if(parset.flag_dim < 0) // create mock data
      {
        // set large values
        parset.n_cloud_per_task = fmax(2.0e5, parset.n_cloud_per_task);
        parset.n_vel_per_cloud = fmax(10.0, parset.n_vel_per_cloud);
        printf("# set NCloudPerCore and NVPerCloud: %d %d\n", parset.n_cloud_per_task, parset.n_vel_per_cloud);
  
        parset.n_tau = fmax(500, parset.n_tau);
        parset.n_con_recon = fmax(500, parset.n_con_recon);
        printf("# set NTau and NConRecon: %d %d\n", parset.n_tau, parset.n_con_recon);
        parset.flag_save_clouds = 1;
      }
  
      
      if(parset.flag_blrmodel == 3 || parset.flag_blrmodel == 4 || parset.flag_blrmodel == 8)
      {
        parset.n_vel_per_cloud = 1;
      }
  
      if(parset.flag_linecenter != 0)
      {
        parset.linecenter_err /= VelUnit;
      }
#ifdef SA
      /* check flag_sa_blrmodel */
      if(parset.flag_sa_blrmodel > 9 || parset.flag_sa_blrmodel < -1)
      {
        fprintf(stderr, "# Error in FlagSABLRModel: value %d is not allowed.\n"
          "# Please specify a value in [-1-9].\n", parset.flag_sa_blrmodel);
          error_flag = 1;
      }

      /* check flag_sa_par_mutual */
      if(parset.flag_sa_par_mutual > 1 || parset.flag_sa_par_mutual < 0)
      {
        fprintf(stderr, "# Error in FlagSAParMutual: value %d is not allowed.\n"
          "# Please specify a value either 0 or 1.\n", parset.flag_sa_par_mutual);
          error_flag = 1;
      }

      /* check flag_sa_linecenter */
      if(parset.sa_linecenter <= 0.0)
      {
        fprintf(stderr, "# Error in SALineCenter: value %f is not allowed.\n"
          "# Please specify a positive value.\n", parset.sa_linecenter);
          error_flag = 1;
      }

      /* check sa InstRes */
      if(parset.sa_InstRes < 0.0 && (parset.flag_dim > 2))
      {
        fprintf(stderr, "# Error in SAInstResErr: value %f is not allowed.\n# Please specify a non-negative value.\n", 
                parset.sa_InstRes);
        error_flag = 1;
      }
      
      /* SA + 1D RM, must have the same BLR */
      if(parset.flag_dim == 4)
      {
        parset.flag_sa_par_mutual = 0;
        if(parset.flag_blrmodel != parset.flag_sa_blrmodel)
        {
          fprintf(stderr, "# Error in FlagBLRModel = %d and FlagSABLRModel = %d.\n"
          "# For FlagDim = 4, FlagBLRModel and FlagSABLRModel must be identical.\n",
          parset.flag_blrmodel, parset.flag_sa_blrmodel);
          error_flag = 1;
        }
      }

      if(parset.flag_dim > 3)
      {
        if(parset.flag_sa_par_mutual == 0)
        {
          if(parset.flag_blrmodel != parset.flag_sa_blrmodel)
          {
            fprintf(stderr, "# Error in FlagBLRModel = %d and FlagSABLRModel = %d.\n"
            "# For FlagSAParMutual = 0, FlagBLRModel and FlagSABLRModel must be identical.\n",
            parset.flag_blrmodel, parset.flag_sa_blrmodel);
            error_flag = 1;
          }
        }
      }

      if(parset.flag_dim > 2 )
      {
        if(strlen(parset.sa_file) == 0)
        {
          fprintf(stderr, "# Please specify SA data file in parameter file.\n");
          error_flag = 4;
        }
      }
      
      if(error_flag == 0)
      {
        if(parset.flag_dim > 2)
        {
          parset.sa_InstRes /= VelUnit;
        }
      }
#endif
    }
  }  
    
  MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
  if(error_flag != 0)
  {  
    MPI_Finalize();
    exit(0);
  }
  
  MPI_Bcast(&parset, sizeof(parset), MPI_BYTE, roottask, MPI_COMM_WORLD);
  return;
}  
  
/*!  
 * read dataset.
 */
void read_data()
{
  FILE *fp;
  int i, error_flag = 0;
  char buf[200], fname[200];

  // first need to determine the number of data points 
  if(thistask == roottask)
  {
    int count;
    
    if(parset.flag_dim >= -1 && parset.flag_dim != 3 && error_flag == 0)
    { 
      // continuum file
      sprintf(fname, "%s/%s", parset.file_dir, parset.continuum_file);
      fp = fopen(fname, "r");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s\n", fname);
        error_flag = 2;
      }
      else
      {
        // count the number of lines
        count = 0;
        while(1)
        {
          fgets(buf, 200, fp);
          if(feof(fp)!=0)
            break;
          count++;
        }
        fclose(fp);
        n_con_data = count;
      
        printf("continuum data points: %d\n", n_con_data);
      }
    }

    if((parset.flag_dim == 1 || parset.flag_dim == 4) && error_flag == 0)
    {
      sprintf(fname, "%s/%s", parset.file_dir, parset.line_file);
    // emission flux line
      fp = fopen(fname, "r");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s\n", fname);
        error_flag = 2;
      }
      else
      {
        // count the number of lines
        count = 0;
        while(1)
        {
          fgets(buf, 200, fp);
          if(feof(fp)!=0)
            break;
          count++;
        }
        fclose(fp);
        n_line_data = count;
        printf("line data points: %d\n", n_line_data);
      }
    }

    if( (parset.flag_dim == 2 || parset.flag_dim == -1 || parset.flag_dim == 5) && error_flag == 0 )
    {
      sprintf(fname, "%s/%s", parset.file_dir, parset.line2d_file);
      fp = fopen(fname, "r");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s\n", fname);
        error_flag = 2;
      }
      else
      {
        fscanf(fp, "%s %d %d\n", buf, &n_line_data, &n_vel_data);
        fclose(fp);
        printf("line2d data points: %d %d\n", n_line_data, n_vel_data);
      }
    }

#ifdef SA
    if( (parset.flag_dim > 2 || parset.flag_dim == -1) && error_flag == 0 )
    {
      sprintf(fname, "%s/%s", parset.file_dir, parset.sa_file);
      fp = fopen(fname, "r");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s\n", fname);
        error_flag = 2;
      }
      else
      {
        fscanf(fp, "%s %d %d %d\n", buf, &n_epoch_sa_data, &n_vel_sa_data, &n_base_sa_data);
        fclose(fp);
        printf("sa data points: %d %d %d\n", n_epoch_sa_data, n_vel_sa_data, n_base_sa_data);
      }
    }
#endif 

  }

  MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
  if(error_flag != 0)
  {
    MPI_Finalize();
    exit(0);
  }

  if(parset.flag_dim >= -1 && parset.flag_dim != 3)
  {
    MPI_Bcast(&n_con_data, 1, MPI_INT, roottask, MPI_COMM_WORLD);
  }

  if(parset.flag_dim == 1 || parset.flag_dim == 4)
  {
    MPI_Bcast(&n_line_data, 1, MPI_INT, roottask, MPI_COMM_WORLD);
  }

  if(parset.flag_dim == 2 || parset.flag_dim == -1 || parset.flag_dim == 5)
  {
    MPI_Bcast(&n_line_data, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    MPI_Bcast(&n_vel_data, 1, MPI_INT, roottask, MPI_COMM_WORLD);

    n_vel_data_ext = n_vel_data + 2 * n_vel_data_incr;
  }

#ifdef SA
  if(parset.flag_dim > 2 || parset.flag_dim == -1)
  {
    MPI_Bcast(&n_epoch_sa_data, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    MPI_Bcast(&n_vel_sa_data,   1, MPI_INT, roottask, MPI_COMM_WORLD);
    MPI_Bcast(&n_base_sa_data,  1, MPI_INT, roottask, MPI_COMM_WORLD);
  }
#endif

  // now allocate memory for data
  allocate_memory_data();

  // now read data
  if(parset.flag_dim >= -1 && parset.flag_dim != 3)
  {
    if(thistask == roottask)
    {
      // continuum data
      sprintf(fname, "%s/%s", parset.file_dir, parset.continuum_file);
      fp = fopen(fname, "r");
      for(i=0; i<n_con_data; i++)
      {      
        if(fscanf(fp, "%lf %lf %lf \n", &Tcon_data[i], &Fcon_data[i], &Fcerrs_data[i])<3)
        {
          fprintf(stderr, "# Error in continuum data file %s.\n"
            "# Too few columns in line %d.\n", fname, i);
          error_flag = 5;
          break;
        }
      }
      fclose(fp);
      
      if(error_flag == 0)
      {
        /* convert time to rest frame */
        for(i=0; i<n_con_data; i++)
        {
          Tcon_data[i] /= (1.0+parset.redshift);
        }
  
        /* cal mean continuum error */
        con_error_mean = 0.0;
        for(i=0; i<n_con_data; i++)
        {
          con_error_mean += Fcerrs_data[i];
        }
        con_error_mean /= n_con_data;
      }
    }
    MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    if(error_flag != 0)
    {
      MPI_Finalize();
      free_memory_data();
      exit(0);
    }

    MPI_Bcast(Tcon_data, n_con_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Fcon_data, n_con_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Fcerrs_data, n_con_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(&con_error_mean, 1, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    
  }

  // read line
  if(parset.flag_dim == 1 || parset.flag_dim == 4)
  {
    if(thistask == roottask)
    {
      // line flux data
      sprintf(fname, "%s/%s", parset.file_dir, parset.line_file);
      fp = fopen(fname, "r");
      for(i=0; i<n_line_data; i++)
      {
        if(fscanf(fp, "%lf %lf %lf \n", &Tline_data[i], &Fline_data[i], &Flerrs_data[i]) < 3)
        {
          fprintf(stderr, "# Error in line data file %s.\n"
            "# Too few columns in line %d.\n", fname, i);
          error_flag = 5;
          break;
        }
      }
      fclose(fp);
      
      if(error_flag == 0)
      {
        /* convert time to rest frame */
        for(i=0; i<n_line_data; i++)
        {
          Tline_data[i] /= (1.0+parset.redshift);
        }
  
        if(Tline_data[0] - Tcon_data[n_con_data-1] > 0.0)
        {
          fprintf(stderr, "# Error: No time overlap between ontinuum and line time series.\n");
          fprintf(stderr, "# Error: continuum, %f-%f; line, %f-%f.\n", Tcon_data[0], Tcon_data[n_con_data-1], 
            Tline_data[0], Tline_data[n_line_data-1]);
          error_flag = 4;
        }
  
        /* cal mean line error */
        line_error_mean = 0.0;
        for(i=0; i<n_line_data;i++)
        {
          // note mask with error < 0.0
          if(Flerrs_data[i] > 0.0)
            line_error_mean += Flerrs_data[i];
        }
        line_error_mean /= n_line_data;
      }
    }  

    MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    if(error_flag != 0)
    {
      MPI_Finalize();
      free_memory_data();
      exit(0);
    }

    MPI_Bcast(Tline_data, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Fline_data, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Flerrs_data, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);

    MPI_Bcast(&line_error_mean, 1, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
  }

  // read 2d line data
  if(parset.flag_dim == 2 || parset.flag_dim == -1 || parset.flag_dim == 5)
  {
    if(thistask == roottask)
    {
      int j;

      sprintf(fname, "%s/%s", parset.file_dir, parset.line2d_file);
      fp = fopen(fname, "r");

      fgets(buf, 200, fp);

      for(i=0; i<n_line_data; i++)
      {
        /* read date */
        if(fscanf(fp, "# %lf\n", &Tline_data[i]) < 1)
        {
          fprintf(stderr, "# Error in line2d data file %s.\n"
            "# Too few columns.\n", fname);
            error_flag = 5; 
            break;
        }
        
        /* read spectrum */
        for(j=0; j<n_vel_data; j++)
        {
          if(fscanf(fp, "%lf%lf%lf\n", &Wline_data[j], 
                 &Fline2d_data[i*n_vel_data + j], &Flerrs2d_data[i*n_vel_data + j]) < 3)
          {
            fprintf(stderr, "# Error in line2d data file %s.\n"
            "# Too few columns.\n", fname);
            error_flag = 5; 
            break;
          }
        }
        fscanf(fp, "\n");
      }
      fclose(fp);
      
      if(error_flag == 0)
      {
        /* convert time to rest frame */
        for(i=0; i<n_line_data; i++)
        {
          Tline_data[i] /= (1.0+parset.redshift);
        }
  
        if(Tline_data[0] - Tcon_data[n_con_data-1] > 0.0)
        {
          fprintf(stderr, "# Error: No time overlap between ontinuum and line time series.\n");
          fprintf(stderr, "# Error: continuum, %f-%f; line, %f-%f.\n", Tcon_data[0], Tcon_data[n_con_data-1], 
            Tline_data[0], Tline_data[n_line_data-1]);
          error_flag = 4;
        }

        /* convert wavelength to velocity */
        for(j=0; j<n_vel_data; j++)
        {
          Vline_data[j] = (Wline_data[j]/(1.0+parset.redshift) - parset.linecenter)/parset.linecenter * C_Unit;
        }
        
        // cal mean line error
        line_error_mean = 0.0;
        for(i=0; i<n_line_data;i++)
          for(j=0; j<n_vel_data; j++)
          {
            // note mask with error < 0.0
            if(Flerrs2d_data[i*n_vel_data + j] > 0.0)
              line_error_mean += Flerrs2d_data[i*n_vel_data + j];
          }
  
        line_error_mean /= (n_line_data*n_vel_data);
      }
    }
    
    MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    if(error_flag != 0)
    {
      MPI_Finalize();
      free_memory_data();
      exit(0);
    }

    // broadcast 2d data
    MPI_Bcast(Vline_data, n_vel_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Tline_data, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Fline2d_data, n_line_data*n_vel_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Flerrs2d_data, n_line_data*n_vel_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);

    MPI_Bcast(&line_error_mean, 1, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    // each task calculates line fluxes
    cal_emission_flux();

    /* extend velocity grid */
    double dVel = Vline_data[1] - Vline_data[0];
    for(i=n_vel_data_incr-1; i>=0; i--)
    {
      /* left-hand side */
      Vline_data_ext[i] = Vline_data_ext[i+1] - dVel;  
      /* right-hand side */
      Vline_data_ext[n_vel_data_ext - 1 - i] = Vline_data_ext[n_vel_data_ext - 1 - i - 1] + dVel;
    }

  }

  /* read instrument broadening data */
  if(parset.flag_InstRes == 2 && (parset.flag_dim == 2 || parset.flag_dim == 5))
  {
    if(thistask == roottask)
    {
      sprintf(fname, "%s/%s", parset.file_dir, parset.file_instres);
      fp = fopen(fname, "r");
      if(fp == NULL)
      {
        fprintf(stderr, "# Error: Cannot open file %s.\n", fname);
        error_flag = 2;
      }
      else 
      {
        for(i=0; i<n_line_data; i++)
        {
          if(fscanf(fp, "%lf %lf\n", &instres_epoch[i], &instres_err_epoch[i]) < 2)
          {
            fprintf(stderr, "Error: Cannot read file %s.\n", fname);
            error_flag = 5;
            break;
          }
          //printf("%d %f %f\n", i, instres_epoch[i], instres_err_epoch[i]);
          instres_epoch[i] /= VelUnit;
          instres_err_epoch[i] /= VelUnit;
  
          if(instres_epoch[i] < parset.width_narrowline)
          {
            printf("# Error narrow line width %f should be smaller than InstRes %f at %d epoch.\n",
              parset.width_narrowline*VelUnit, parset.InstRes*VelUnit, i);    
            error_flag = 1;   
            break;   
          }
        }
        fclose(fp);
      }
    }

    MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    if(error_flag != 0)
    {
      MPI_Finalize();
      free_memory_data();
      exit(0);
    }

    MPI_Bcast(instres_epoch, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(instres_err_epoch, n_line_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
  }
  /*uniform prior of line broadening*/
  if(parset.flag_InstRes == 1 && (parset.flag_dim == 2 || parset.flag_dim == 5))
  {
    for(i=0; i<n_line_data; i++)
    {
      instres_epoch[i] = parset.InstRes;
      instres_err_epoch[i] = parset.InstRes_err;
    }
  }

#ifdef SA
/* read SA data */
  if(parset.flag_dim > 2 || parset.flag_dim == -1)
  {
    int j;
    if(thistask == roottask)
    {
      sprintf(fname, "%s/%s", parset.file_dir, parset.sa_file);
      fp = fopen(fname, "r");
  
      fgets(buf, 200, fp);
  
      /* read line profile */
      for(j=0; j<n_epoch_sa_data; j++)
      {
        for(i=0; i<n_vel_sa_data; i++)
        {
          if(fscanf(fp, "%lf %lf %lf\n", &wave_sa_data[i], 
                     &Fline_sa_data[i+j*n_vel_sa_data], &Flerrs_sa_data[i+j*n_vel_sa_data]) < 3)
          {
            fprintf(stderr, "# Error in SA data file %s.\n"
            "# Too few columns.\n", fname);
            error_flag = 5;
            break;
          }
        }
        fscanf(fp, "\n");
      }
      
      for(j=0; j<n_base_sa_data; j++)
      {
        if(fscanf(fp, "# %lf %lf\n", &base_sa_data[j*2+0], &base_sa_data[j*2+1]) < 2)
        {
          fprintf(stderr, "# Error in SA data file %s.\n"
            "# Too few columns in base %d.\n", fname, i);
          error_flag = 5;
          break;
        }

        for(i=0; i<n_vel_sa_data; i++)
        {
          if(fscanf(fp, "%lf %lf %lf\n", &wave_sa_data[i], &phase_sa_data[i+j*n_vel_sa_data], 
                      &pherrs_sa_data[i+j*n_vel_sa_data]) < 3)
          {
            fprintf(stderr, "# Error in SA data file %s.\n"
            "# Too few columns.\n", fname);
            error_flag = 5;
            break;
          }
        }
        fscanf(fp, "\n");
      }
      fclose(fp);
    }
    
    MPI_Bcast(&error_flag, 1, MPI_INT, roottask, MPI_COMM_WORLD);
    if(error_flag != 0)
    {
      MPI_Finalize();
      free_memory_data();
      exit(0);
    }

    MPI_Bcast(wave_sa_data, n_vel_sa_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(base_sa_data, n_base_sa_data*2, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(phase_sa_data, n_vel_sa_data*n_base_sa_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(pherrs_sa_data, n_vel_sa_data*n_base_sa_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Fline_sa_data, n_vel_sa_data * n_epoch_sa_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);
    MPI_Bcast(Flerrs_sa_data, n_vel_sa_data * n_epoch_sa_data, MPI_DOUBLE, roottask, MPI_COMM_WORLD);

    /* convert wavelength to velocity */
    for(i=0; i<n_vel_sa_data; i++)
    {
      vel_sa_data[i] = (wave_sa_data[i]/(1.0+parset.redshift) - parset.sa_linecenter)/parset.sa_linecenter;
      vel_sa_data[i] *= C_Unit;
    }

    /* normalize phase */
    for(j=0; j<n_base_sa_data; j++)
    {
      for(i=0; i<n_vel_sa_data; i++)
      {
        phase_sa_data[i+j*n_vel_sa_data] *= (PhaseFactor * wave_sa_data[i]);
        pherrs_sa_data[i+j*n_vel_sa_data] *= (PhaseFactor * wave_sa_data[i]);
      }
    }

    /* calculate sa flux norm */
    sa_flux_norm = 0.0;
    for(i=0; i<n_vel_sa_data; i++)
    {
      sa_flux_norm += Fline_sa_data[i];
    }
    sa_flux_norm /= n_vel_sa_data;
  }
#endif  

  return;
}

/*!
 * allocate data memory. 
 */
void allocate_memory_data()
{
  if(parset.flag_dim >=-1 && parset.flag_dim != 3)
  {
    Tcon_data = malloc(n_con_data * sizeof(double));
    Fcon_data = malloc(n_con_data * sizeof(double));
    Fcerrs_data = malloc(n_con_data * sizeof(double));
  }

  if(parset.flag_dim == 1 || parset.flag_dim == 4)
  {
    Tline_data = malloc(n_line_data * sizeof(double));
    Fline_data = malloc(n_line_data * sizeof(double));
    Flerrs_data = malloc(n_line_data * sizeof(double));
  }

  if(parset.flag_dim == 2 || parset.flag_dim == -1 || parset.flag_dim == 5)
  {
    Vline_data_ext = malloc(n_vel_data_ext * sizeof(double));
    Wline_data_ext = malloc(n_vel_data_ext * sizeof(double));
    Vline_data = Vline_data_ext + n_vel_data_incr;
    Wline_data = Wline_data_ext + n_vel_data_incr;
    Tline_data = malloc(n_line_data * sizeof(double));
    Fline_data = malloc(n_line_data * sizeof(double));
    Flerrs_data = malloc(n_line_data * sizeof(double));

    Fline2d_data = malloc(n_line_data * n_vel_data * sizeof(double *));
    Flerrs2d_data = malloc(n_line_data * n_vel_data * sizeof(double *));
  }

  if(parset.flag_InstRes > 0 && (parset.flag_dim == 2 || parset.flag_dim == 5))
  {
    instres_epoch = malloc(n_line_data * sizeof(double));
    instres_err_epoch = malloc(n_line_data * sizeof(double));
  }

#ifdef SA
  if(parset.flag_dim > 2 || parset.flag_dim == -1)
  {
    wave_sa_data = malloc(n_vel_sa_data*sizeof(double));
    vel_sa_data = malloc(n_vel_sa_data*sizeof(double));
    base_sa_data = malloc(n_base_sa_data * 2 * sizeof(double));
    Fline_sa_data = malloc(n_vel_sa_data*n_epoch_sa_data*sizeof(double));
    Flerrs_sa_data = malloc(n_vel_sa_data*n_epoch_sa_data*sizeof(double));
    phase_sa_data = malloc(n_vel_sa_data * n_base_sa_data * sizeof(double));
    pherrs_sa_data = malloc(n_vel_sa_data * n_base_sa_data * sizeof(double));
  }
#endif
}

/*!
 * free data memory.
 */
void free_memory_data()
{
  if(parset.flag_dim >=-1 && parset.flag_dim != 3)
  {
    free(Tcon_data);
    free(Fcon_data);
    free(Fcerrs_data);
  }

  if(parset.flag_dim == 1 || parset.flag_dim == 4)
  {
    free(Tline_data);
    free(Fline_data);
    free(Flerrs_data);
  }

  if(parset.flag_dim == 2 || parset.flag_dim == -1 || parset.flag_dim == 5)
  {
    free(Vline_data_ext);
    free(Wline_data_ext);
    free(Tline_data);
    free(Fline_data);
    free(Flerrs_data);

    free(Fline2d_data);
    free(Flerrs2d_data);
  }


  if(parset.flag_InstRes > 0 && (parset.flag_dim == 2 || parset.flag_dim == 5))
  {
    free(instres_epoch); 
    free(instres_err_epoch);
  }

#ifdef SA
  if(parset.flag_dim > 2 || parset.flag_dim == -1)
  {
    free(wave_sa_data);
    free(vel_sa_data);
    free(base_sa_data);
    free(Fline_sa_data);
    free(Flerrs_sa_data);
    free(phase_sa_data);
    free(pherrs_sa_data); 
  }
#endif
}

/*! 
 * calculate the integrated emission line flux.
 */
void cal_emission_flux()
{
  int i, j;
  double dV;
  
  // assume that velocity grid is equally spaced 
  dV = (Vline_data[n_vel_data-1]-Vline_data[0])/(n_vel_data-1);

// using trapezoid formula.
  for(j=0; j<n_line_data; j++)
  { 
    Fline_data[j] = Fline2d_data[j*n_vel_data + 0]/2.0;
    Flerrs_data[j] = (Flerrs2d_data[j*n_vel_data + 0] * Flerrs2d_data[j*n_vel_data + 0])/2.0;
    for(i=1; i<n_vel_data-1; i++)
    {
      Fline_data[j] += Fline2d_data[j*n_vel_data + i];
      Flerrs_data[j] += Flerrs2d_data[j*n_vel_data + i]*Flerrs2d_data[j*n_vel_data + i];
    }
    Fline_data[j] += Fline2d_data[j*n_vel_data + n_vel_data-1]/2.0;
    Flerrs_data[j] += (Flerrs2d_data[j*n_vel_data + n_vel_data-1]*Flerrs2d_data[j*n_vel_data + n_vel_data-1])/2.0;

    Fline_data[j] *= dV;
    Flerrs_data[j] *= dV*dV;
    Flerrs_data[j] = sqrt(Flerrs_data[j]);
  }
}

/*!
 * get number of particles from the option file.
 */
void get_num_particles(char *fname)
{
  FILE *fp;
  char buf[BRAINS_MAX_STR_LENGTH], buf1[BRAINS_MAX_STR_LENGTH];
  fp = fopen(fname, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "# Error: Cannot open file %s\n", fname);
    exit(-1);
  }

  buf[0]='#';
  while(buf[0]=='#')
  {
    fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
    if(sscanf(buf, "%s", buf1) < 1)  // a blank line
    {
      buf[0] = '#';
    }
  }
  sscanf(buf, "%d", &parset.num_particles);
  fclose(fp);
}

/*!
 * get file name of posterior sample. 
 */
void get_posterior_sample_file(char *fname, char *samplefile)
{
  FILE *fp;
  char buf[BRAINS_MAX_STR_LENGTH], buf1[BRAINS_MAX_STR_LENGTH];
  fp = fopen(fname, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "# Error: Cannot open file %s\n", fname);
    exit(-1);
  }

  buf[0]='#';
  while(buf[0]=='#')
  {
    fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
    if(sscanf(buf, "%s", buf1) < 1)  // a blank line
    {
      buf[0] = '#';
    }
  }
  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%d", &options.new_level_interval);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%d", &options.save_interval);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%d", &options.thread_steps);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%d", &options.max_num_levels);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%lf", &options.lambda);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%lf", &options.beta);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%d", &options.max_num_saves);

  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%s", options.sample_file);
  
  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%s", options.sample_info_file);
  
  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%s", options.levels_file);
  
  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
//  sscanf(buf, "%s", options.sampler_state_file);
  
  fgets(buf, BRAINS_MAX_STR_LENGTH, fp);
  sscanf(buf, "%s", samplefile);
  fclose(fp);
}

/* 
 * print out parameters into a file
 * 
 */
void fprint_param()
{
  int i;
  FILE *fp;
  char fname[BRAINS_MAX_STR_LENGTH];
  
  sprintf(fname, "%s/%s", parset.file_dir, "data/BRAINS_param.txt");
  fp = fopen(fname, "w");
  fprintf(fp, "#*************************************************\n");
  fprint_version(fp);
  fprintf(fp, "#*************************************************\n");
  
  fprintf(fp, "%-30s  %-s\n", "ParameterFile", parset.param_file);
  for(i=0; i<num_pardict; i++)
  {
    if(pardict[i].isset == 0)
      continue;

    switch(pardict[i].id)
    {
      case INT:
        fprintf(fp, "%-30s  %-4d\n", pardict[i].tag, *((int *)pardict[i].addr));
        break;
      case DOUBLE:
        fprintf(fp, "%-30s  %-10.4f\n", pardict[i].tag, *((double *)pardict[i].addr));
        break;
      case STRING:
        fprintf(fp, "%-30s  %-s\n", pardict[i].tag, ((char *)pardict[i].addr));
        break;
    }
  }

  fclose(fp);
}
