/* BRAINS
 * (B)LR (R)everberation-mapping (A)nalysis (I)ntegrated with (N)ested (S)ampling
 * Yan-Rong Li, liyanrong@ihep.ac.cn
 * Thu, Aug 4, 2016
 */

/*!
 * \file user_blr_model.c
 * \brief allow users to define their owen BLR models.
 * 
 * User should provide: 1) number of parameters in 1D and 2D; 
 *                      2) struct for BLR model;
 *                      3) function for setting parameter range
 *                      4) function for calculating clouds' lag and weight in 1D
 *                      5) function for calculating clouds' lag, velocity, and weight in 2D
 *                      6) function for setting the parameter values used in simulation (if flag_dim<0)
 *
 * The following is an example. The users can make modification with their own BLR models.
 *
 */

#include <gsl/gsl_randist.h>
#include "brains.h"
#include "user_blr_model.h"

const int num_params_MyBLRmodel1d = 8;   /*!< number of parameters for 1D model */
const int num_params_MyBLRmodel2d = 17;  /*!< number of parameters for 2D model */


/*
 * define a BLR model struct.
 */
typedef struct
{
  double mu;            /*!< \brief 1. peak radius*/
  double beta;          /*!< \brief 2. fraction of inner radius*/
  double F;             /*!< \brief 3. fraction of outer radius*/
  double inc;           /*!< \brief 4. inclination */
  double opn;           /*!< \brief 5. opening angle */
  double k;             /*!< \brief 6. kappa */
  double gam;           /*!< \brief 7. gamma */
  double xi;            /*!< \brief 8. obscuration */
  double mbh;           /*!< \brief 9. black hole mass */
  double fellip;        /*!< \brief 10.ellipitic orbits */
  double fflow;         /*!< \brief 11.inflow/outflow */
  double sigr_circ;     /*!< \brief 12. */
  double sigthe_circ;   /*!< \brief 13. */
  double sigr_rad;      /*!< \brief 14. */
  double sigthe_rad;    /*!< \brief 15. */
  double theta_rot;     /*!< \brief 16. */
  double sig_turb;      /*!< \brief 17. turbulence velocity */
}MyBLRmodel;


/*!
 *  set parameter range
 */
void set_blr_range_mymodel()
{
  int i;
  
  i = 0;
  //mu
  blr_range_model[i][0] = log(0.1);
  blr_range_model[i++][1] = log(rcloud_max_set*0.5);
  //beta
  blr_range_model[i][0] = 0.001;
  blr_range_model[i++][1] = 2.0;
  //F
  blr_range_model[i][0] = 0.0;
  blr_range_model[i++][1] = 1.0;
  //inc
  blr_range_model[i][0] = 0.0; //in cosine
  blr_range_model[i++][1] = 1.0;
  //opn
  blr_range_model[i][0] = 0.0;  // in rad
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
  blr_range_model[i++][1] = log(1.0e3);
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
  blr_range_model[i][0] = log(0.0001);
  blr_range_model[i++][1] = log(0.1);

  return;
}

/* 
 * This function caclulate 1d transfer function.
 * 
 * The clouds' lag, and weight are stored in arrays "clouds_tau", "clouds_weight",
 * which must be provided.
 */
void transfun_1d_cloud_sample_mymodel(const void *pm, int flag_save)
{
  int i, nc;
  double r, phi, dis, Lopn_cos;
  double x, y, z, xb, yb, zb, zb0;
  double inc, F, beta, mu, k, gam, xi, a, s, rin, sig;
  double Lphi, Lthe, sin_Lphi, cos_Lphi, sin_Lthe, cos_Lthe, sin_inc_cmp, cos_inc_cmp;
  double weight, rnd, rnd_xi;
  MyBLRmodel *model = (MyBLRmodel *)pm;

  Lopn_cos = cos(model->opn*PI/180.0); /* cosine of openning angle */
  inc = acos(model->inc);         /* inclination angle in rad */
  beta = model->beta;         
  F = model->F;
  mu = exp(model->mu);                 /* mean radius */
  k = model->k;  
  gam = model-> gam;
  xi = model->xi;

  a = 1.0/beta/beta;
  s = mu/a;
  rin=mu*F;
  sig=(1.0-F)*s;

  sin_inc_cmp = cos(inc);//sin(PI/2.0 - inc);
  cos_inc_cmp = sin(inc);//cos(PI/2.0 - inc);

  for(i=0; i<parset.n_cloud_per_task; i++)
  {
// generate a direction of the angular momentum of the orbit   
    Lphi = 2.0*PI * gsl_rng_uniform(gsl_r);
    Lthe = acos(Lopn_cos + (1.0-Lopn_cos) * pow(gsl_rng_uniform(gsl_r), gam));
    sin_Lphi = sin(Lphi);
    cos_Lphi = cos(Lphi);
    sin_Lthe = sin(Lthe);
    cos_Lthe = cos(Lthe);

    nc = 0;
    r = rcloud_max_set+1.0;
    while(r>rcloud_max_set || r<rcloud_min_set)
    {
      if(nc > 1000)
      {
        printf("# Error, too many tries in generating ridial location of clouds.\n");
        exit(0);
      }
      rnd = gsl_ran_gamma(gsl_r, a, 1.0);
//    r = mu * F + (1.0-F) * gsl_ran_gamma(gsl_r, 1.0/beta/beta, beta*beta*mu);
      r = rin + sig * rnd;
      nc++;
    }
    phi = 2.0*PI * gsl_rng_uniform(gsl_r);

    /* Polar coordinates to Cartesian coordinates */
    x = r * cos(phi); 
    y = r * sin(phi);
    z = 0.0;

/* right-handed framework
 * first rotate around y axis by an angle of Lthe, then rotate around z axis 
 * by an angle of Lphi
 */
  /*xb = cos(Lthe)*cos(Lphi) * x + sin(Lphi) * y - sin(Lthe)*cos(Lphi) * z;
    yb =-cos(Lthe)*sin(Lphi) * x + cos(Lphi) * y + sin(Lthe)*sin(Lphi) * z;
    zb = sin(Lthe) * x + cos(Lthe) * z; */
    
    xb = cos_Lthe*cos_Lphi * x + sin_Lphi * y;
    yb =-cos_Lthe*sin_Lphi * x + cos_Lphi * y;
    zb = sin_Lthe * x;

    zb0 = zb;
    rnd_xi = gsl_rng_uniform(gsl_r);
    if( (rnd_xi < 1.0 - xi) && zb0 < 0.0)
      zb = -zb;

// counter-rotate around y, LOS is x-axis 
    /*x = xb * cos(PI/2.0-inc) + zb * sin(PI/2.0-inc);
    y = yb;
    z =-xb * sin(PI/2.0-inc) + zb * cos(PI/2.0-inc); */

    x = xb * cos_inc_cmp + zb * sin_inc_cmp;
    y = yb;
    z =-xb * sin_inc_cmp + zb * cos_inc_cmp;

    dis = r - x;
    weight = 0.5 + k*(x/r);    
    clouds_tau[i] = dis;
    clouds_weight[i] = weight;

    if(flag_save && thistask==roottask)
    {
      if(i%(icr_cloud_save) == 0)
        fprintf(fcloud_out, "%f\t%f\t%f\n", x, y, z);
    }
  }

  return;
}

/* 
 * This function caclulate 2d transfer function.
 *
 * The clouds' lag, velocity, and weight are stored in arrays "clouds_tau", "clouds_vel", "clouds_weight",
 * which must be provided.
 */
void transfun_2d_cloud_sample_mymodel(const void *pm, double *transv, double *trans2d, int n_vel, int flag_save)
{
  int i, j, nc;
  double r, phi, cos_phi, sin_phi, dis, Lopn_cos;
  double x, y, z, xb, yb, zb, zb0, vx, vy, vz, vxb, vyb, vzb;
  double V, rhoV, theV, Vr, Vph, Vkep, Rs, g;
  double inc, F, beta, mu, k, gam, xi, a, s, sig, rin;
  double mbh, fellip, fflow, sigr_circ, sigthe_circ, sigr_rad, sigthe_rad, theta_rot, sig_turb;
  double Lphi, Lthe, sin_Lphi, cos_Lphi, sin_Lthe, cos_Lthe, sin_inc_cmp, cos_inc_cmp, linecenter=0.0;
  double weight, rnd, rnd_xi;
  double *pmodel = (double *)pm;
  MyBLRmodel *model = (MyBLRmodel *)pm;

  Lopn_cos = cos(model->opn*PI/180.0); /* cosine of openning angle */
  inc = acos(model->inc);         /* inclination angle in rad */
  beta = model->beta;         
  F = model->F;
  mu = exp(model->mu);                 /* mean radius */
  k = model->k;  
  gam = model-> gam;
  xi = model->xi;

  mbh = exp(model->mbh);
  fellip = model->fellip;
  fflow = model->fflow;
  sigr_circ = exp(model->sigr_circ);
  sigthe_circ = exp(model->sigthe_circ);
  sigr_rad = exp(model->sigr_rad);
  sigthe_rad = exp(model->sigthe_rad);
  theta_rot = model->theta_rot*PI/180.0;
  sig_turb = exp(model->sig_turb);

  Rs = 3.0e11*mbh/CM_PER_LD; // Schwarzchild radius in a unit of light-days

  a = 1.0/beta/beta;
  s = mu/a;
  rin=mu*F + Rs;  // include Scharzschild radius
  sig=(1.0-F)*s;
  
  sin_inc_cmp = cos(inc); //sin(PI/2.0 - inc);
  cos_inc_cmp = sin(inc); //cos(PI/2.0 - inc);

  if(parset.flag_linecenter !=0)
  {
    linecenter = pmodel[num_params_blr - num_params_linecenter - 3] * parset.linecenter_err; 
  }
  
  for(i=0; i<parset.n_cloud_per_task; i++)
  {
// generate a direction of the angular momentum of the orbit   
    Lphi = 2.0*PI * gsl_rng_uniform(gsl_r);
    Lthe = acos(Lopn_cos + (1.0-Lopn_cos) * pow(gsl_rng_uniform(gsl_r), gam));
    sin_Lphi = sin(Lphi);
    cos_Lphi = cos(Lphi);
    sin_Lthe = sin(Lthe);
    cos_Lthe = cos(Lthe);

    nc = 0;
    r = rcloud_max_set+1.0;
    while(r>rcloud_max_set || r<rcloud_min_set)
    {
      if(nc > 1000)
      {
        printf("# Error, too many tries in generating ridial location of clouds.\n");
        exit(0);
      }
      rnd = gsl_ran_gamma(gsl_r, a, 1.0);
//    r = mu * F + (1.0-F) * gsl_ran_gamma(gsl_r, 1.0/beta/beta, beta*beta*mu);
      r = rin + sig * rnd;
      nc++;
    }
    phi = 2.0*PI * gsl_rng_uniform(gsl_r);
    cos_phi = cos(phi);
    sin_phi = sin(phi);

    /* Polar coordinates to Cartesian coordinate */
    x = r * cos_phi; 
    y = r * sin_phi;
    z = 0.0;

/* right-handed framework
 * first rotate around y axis by an angle of Lthe, then rotate around z axis 
 * by an angle of Lphi
 */
  /*xb = cos(Lthe)*cos(Lphi) * x + sin(Lphi) * y - sin(Lthe)*cos(Lphi) * z;
    yb =-cos(Lthe)*sin(Lphi) * x + cos(Lphi) * y + sin(Lthe)*sin(Lphi) * z;
    zb = sin(Lthe) * x + cos(Lthe) * z; */
    
    xb = cos_Lthe*cos_Lphi * x + sin_Lphi * y;
    yb =-cos_Lthe*sin_Lphi * x + cos_Lphi * y;
    zb = sin_Lthe * x;

    zb0 = zb;
    rnd_xi = gsl_rng_uniform(gsl_r);
    if( (rnd_xi < 1.0 - xi) && zb0 < 0.0)
      zb = -zb;

// counter-rotate around y, LOS is x-axis 
    /* x = xb * cos(PI/2.0-inc) + zb * sin(PI/2.0-inc);
       y = yb;
       z =-xb * sin(PI/2.0-inc) + zb * cos(PI/2.0-inc); */
    
    x = xb * cos_inc_cmp + zb * sin_inc_cmp;
    y = yb;
    z =-xb * sin_inc_cmp + zb * cos_inc_cmp;

    dis = r - x;
    weight = 0.5 + k*(x/r);
    clouds_tau[i] = dis;
    clouds_weight[i] = weight;

    Vkep = sqrt(mbh/r);
    
    for(j=0; j<parset.n_vel_per_cloud; j++)
    {
      rnd = gsl_rng_uniform(gsl_r);

      if(rnd < fellip)
      {
        rhoV = (gsl_ran_ugaussian(gsl_r) * sigr_circ  + 1.0) * Vkep;
        theV =  (gsl_ran_ugaussian(gsl_r) * sigthe_circ + 0.5)*PI;
      }
      else
      {
        if(fflow <= 0.5) /* inflow */
        {
          rhoV = (gsl_ran_ugaussian(gsl_r) * sigr_rad   + 1.0) * Vkep;
          theV = (gsl_ran_ugaussian(gsl_r) * sigthe_rad + 1.0) * PI + theta_rot;
        }
        else     /* outflow */
        {
          rhoV = (gsl_ran_ugaussian(gsl_r) * sigr_rad  + 1.0) * Vkep;
          theV = (gsl_ran_ugaussian(gsl_r) * sigthe_rad) * PI + theta_rot;
        }
      }
      
      Vr = sqrt(2.0) * rhoV * cos(theV);
      Vph = rhoV * sin(theV);

      vx = Vr * cos_phi - Vph * sin_phi;
      vy = Vr * sin_phi + Vph * cos_phi;
      vz = 0.0;    
      
    /*vxb = cos(Lthe)*cos(Lphi) * vx + sin(Lphi) * vy - sin(Lthe)*cos(Lphi) * vz;
      vyb =-cos(Lthe)*sin(Lphi) * vx + cos(Lphi) * vy + sin(Lthe)*sin(Lphi) * vz;
      vzb = sin(Lthe) * vx + cos(Lthe) * vz;*/

      vxb = cos_Lthe*cos_Lphi * vx + sin_Lphi * vy;
      vyb =-cos_Lthe*sin_Lphi * vx + cos_Lphi * vy;
      vzb = sin_Lthe * vx;


      if((rnd_xi < 1.0-xi) && zb0 < 0.0)
        vzb = -vzb;
    
      /*vx = vxb * cos(PI/2.0-inc) + vzb * sin(PI/2.0-inc);
      vy = vyb;
      vz =-vxb * sin(PI/2.0-inc) + vzb * cos(PI/2.0-inc);*/

      vx = vxb * cos_inc_cmp + vzb * sin_inc_cmp;
      vy = vyb;
      vz =-vxb * sin_inc_cmp + vzb * cos_inc_cmp;

      V = -vx;  //note the definition of the line-of-sight velocity. postive means a receding 
                // velocity relative to the observer.

      V += gsl_ran_ugaussian(gsl_r) * sig_turb * Vkep; // add turbulence velocity

      if(fabs(V) >= C_Unit) // make sure that the velocity is smaller than speed of light
        V = 0.9999*C_Unit * (V>0.0?1.0:-1.0);

      g = sqrt( (1.0 + V/C_Unit) / (1.0 - V/C_Unit) ) / sqrt(1.0 - Rs/r); //relativistic effects
      V = (g-1.0)*C_Unit;

      V += linecenter;
      clouds_vel[i*parset.n_vel_per_cloud + j] = V;

      if(flag_save && thistask==roottask)
      {
        if(i%(icr_cloud_save) == 0)
          fprintf(fcloud_out, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n", x, y, z, vx*VelUnit, vy*VelUnit, vz*VelUnit, weight);
      }
    }
  }

  return;
}

/*!
 *  set parameter values used in simulation (if flag_dim < 0).
 */
void set_par_value_mymodel_sim(double *pm)
{
  int i;

	i=0;
  pm[i++] = log(4.0);   // mu
  pm[i++] = 1.0;        // beta
  pm[i++] = 0.25;        // F
  pm[i++] = cos(20.0/180.0*PI); // inc
  pm[i++] = 40.0;       // opn
  pm[i++] = -0.4;        // kappa
  pm[i++] = 5.0;        // gamma
  pm[i++] = 0.5;        // obscuration
  pm[i++] = log(2.0);  //mbh
  pm[i++] = 0.5;       //fellip
  pm[i++] = 0.4;       //fflow
  pm[i++] = log(0.01); //
  pm[i++] = log(0.1);  //
  pm[i++] = log(0.01); //
  pm[i++] = log(0.1);  //
  pm[i++] = 0.0;       // theta_rot
  pm[i++] = log(0.001);  // sig_turb
  pm[i++] = 0.0;       // parameter for spectral broadening 
}

