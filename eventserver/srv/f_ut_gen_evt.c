/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : f_ut_gen_evt                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : l_status = f_ut_gen_evt(ps_buf,l_blen,              */
/*                                       l_subevts,l_max,l_mode)      */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Routine to generate one event.                      */
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+  ps_buf     : INTS4 *                                              */
/*                 Pointer to event buffer to be filled               */
/*+  l_blen     : INTS4                                               */
/*                 Size of buffer [bytes]                             */
/*+  l_subevts  : INTS4                                               */
/*                 Number of subevents to generate [1,2]              */
/*+  l_max      : INTS4                                               */
/*                 Maximum number of channels per subevent [1,5]      */
/*+  l_mode     : INTS4                                               */
/*                 1 : fix length subevents                           */
/*                 2 : variable length subevents                      */
/*                                                                    */
/*+ Return type : INTS4                                               */
/*                  0 : success                                       */
/*                 -1 : not enough space                              */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/* Generates events with gaussian peaks.                              */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : f_ut_gen_evt.c                                      */
/*+ Home direct.: /mbs/prod/src                                       */
/*+ Version     : 1.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 20-Jan-1997                                         */
/*+ Object libr.:                                                     */
/*+ Updates     : Date        Purpose                                 */
/*                6. 4.2001, H.G.: set different procids in subevents */
/*               19.12.2001, H.G.: s_ve....h from /GSI/revservxxx     */
/*1- C Procedure *************+****************************************/
#include "typedefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* in both endian cases same version needed, as endian 
   treated in ptrevmbsdef.h                                           */
#include "s_ve10_1.h"
#include "s_ves10_1.h"

#include "f_ut_gen_rand_event.h"

/* define peak structure */
static REAL8 peak[NUM_PEAK] = { 100., 202., 453., 1024., 3800.};
static REAL8 sigma[NUM_PEAK] = { 10., 22., 153., 104., 38.};
static REAL8 peak2[NUM_PEAK] = { 100., 202., 453., 2500., 2024.};
static REAL8 sigma2[NUM_PEAK] = { 22., 153., 104., 100., 20.};

/* in second subevent, second peaks are shifted by shift */
static REAL8 shift;

INTS4 f_ut_gen_evt(INTS4 *ps_buf, INTS4 l_blen, INTS4 l_subevts, INTS4 l_max, INTS4 l_mode)
{
  INTS4 l_status=0, l_val_num, l_val=0, l_buf_len=0, l_val0 = 0;
  static INTS4 l_trg_cnt = 0;
  int diff;
  s_ve10_1 *ps_vehe;
  s_ves10_1 *ps_veshe, *ps_veshe1;
  
  shift = 0.;
  l_trg_cnt ++;
  
  if(l_subevts > 2 || l_subevts < 1)
    {
      l_status = -1;
      return(l_status);      
    }
  
  if(l_max > 5 || l_max < 1)
    {
      l_status = -1;
      return(l_status);      
    }
  
  if(l_mode > 2 || l_mode < 1)
    {
      l_status = -1;
      return(l_status);      
    }
  
  /* ps_buf points to beginning of header of event */
  ps_vehe = (s_ve10_1 *)ps_buf;
  ps_vehe->i_subtype = 1;
  ps_vehe->i_type = 10;
  ps_vehe->i_trigger = (INTS4)(get_int(1., 10.)+0.5);
  ps_vehe->i_dummy = 2;
  ps_vehe->l_count = l_trg_cnt;
  
  /* points to beginning of header of subevent */
  ps_buf += (sizeof(s_ve10_1)/sizeof(long));
  l_buf_len += sizeof(s_ve10_1);
  if(debug)
    printf("1 l_buf_len %d\n", l_buf_len);
  ps_veshe = (s_ves10_1 *)ps_buf;
  ps_veshe->i_subtype = 1;
  ps_veshe->i_type = 10;
  ps_veshe->h_control = 0;
  ps_veshe->h_subcrate = 2;
  ps_veshe->i_procid = 10;
  
  /* points to beginning of data buffer */
  ps_buf += (sizeof(s_ves10_1)/sizeof(long));
  l_buf_len += sizeof(s_ves10_1);
  if(debug)
    printf("2 l_buf_len %d\n", l_buf_len);
  if(debug)
    printf("Header Sizes: E%d, SE%d, long%d\n", sizeof(s_ve10_1), sizeof(s_ves10_1), sizeof(long));
  
  /* select number of channels in subevent */
  l_val = 0;
  l_val_num = (int)(get_int(1., l_max)+0.5);
  /* write channels to buffer */
  while (l_val_num)
    {
      l_val++;
      *ps_buf++ = rand_event(l_val);
      l_val_num--;
    }
  l_buf_len += l_val*sizeof(long);
  if(debug)
    printf("3 l_buf_len %d l_val %d\n", l_buf_len, l_val);
  ps_veshe->l_dlen = l_val*sizeof(long)/2+2;
  
  if(l_subevts == 2)
    {
      shift = 250.;
      /* next subevent */
      
      ps_veshe1=(s_ves10_1 *)ps_buf;
      ps_veshe1->i_subtype = 1;
      ps_veshe1->i_type = 10;
      ps_veshe1->h_control = 0;
      ps_veshe1->h_subcrate = 2;
      ps_veshe1->i_procid = 20;
      
      ps_buf += sizeof(s_ves10_1)/sizeof(long); 
      l_val0 = sizeof(s_ves10_1)/sizeof(long);
      l_buf_len += l_val0*sizeof(long);
      if(debug)
	printf("4 l_buf_len %d l_val0 %d\n", l_buf_len, l_val0);

      l_val = 0;
      if(l_mode == 2)
	l_val_num = (int)(get_int(1., l_max)+0.5);
      while (l_val_num)
	{
	  l_val++;
	  *ps_buf++ = rand_event(l_val);
	  l_val_num--;
	}
      ps_veshe1->l_dlen = l_val*sizeof(long)/2+2;
      l_buf_len += l_val*sizeof(long);
      if(debug)
	printf("5 l_buf_len %d l_val %d\n", l_buf_len, l_val);
    }
  
  /* ps_vehe->l_dlen = l_buf_len/2+4; */
  if(l_subevts == 2)
    ps_vehe->l_dlen = ps_veshe->l_dlen + ps_veshe1->l_dlen + 12;
  else
    ps_vehe->l_dlen = ps_veshe->l_dlen + 8;
  if(debug)
    printf("6 l_buf_len %d\n", l_buf_len);

  l_status = (l_buf_len < l_blen?0:-1);
  if(debug)
    {
      if(l_subevts == 2)
	{
	  diff = ps_vehe->l_dlen - ps_veshe->l_dlen - ps_veshe1->l_dlen;
	  printf("Diff: %d - %d - %d = %d\n", ps_vehe->l_dlen, ps_veshe->l_dlen, ps_veshe1->l_dlen, diff);
	}
      else
	{
	  diff = ps_vehe->l_dlen - ps_veshe->l_dlen;
	  printf("Diff: %d - %d = %d\n", ps_vehe->l_dlen, ps_veshe->l_dlen, diff);
	}
    }
  return(l_status);
}

/*
**	Function name : gauss_rnd
**
**	Description : randomize in gaussian shape
**	Input : mean and sigma of gaussian
**	Output : float gaussian random number
**               os dependent
**      Replace: with p_dNormal(double mean, double sigma, unsigned int *seed)
**
*/
REAL8 gauss_rnd(REAL8 mean, REAL8 sigma)
{
  static INTS4 iset=0;
  static REAL8 gset;
  REAL8 v1, v2, s, u1;
  
  if(sigma < 0.)
    {
      v1 = drand48();
      return (log(1-v1)/sigma + mean);
    }
  else
    {
      if(iset == 0)
	{
	  do
	    {
	      v1 = 2.*drand48()-1.;
	      v2 = 2.*drand48()-1.;
	      
	      s = v1*v1+v2*v2;
	    } while (s >= 1.0 || s == 0.0);
	  
	  u1 = sigma*v1*(sqrt(-2.*log(s)/s)) + mean;
	  gset = u1;
	  iset = 1;
	  
	  return (sigma*v2*(sqrt(-2.*log(s)/s)) + mean);
	}
      else
	{
	  iset = 0;
	  return gset;
	}
    }
}

/*
**	Function name : get_int
**
**	Description :
**	Input :
**	Output :
*/
REAL8 get_int(REAL8 low, REAL8 high)
{
  return ((high-low)*drand48()+low);
}

/*
**	Function name : rand_event
**
**	Description :
**	Input :
**	Output :
*/
INTS4 rand_event(long choice)
{
  INTS4 cnt;
  
  switch(choice)
    {
  case 1:
    cnt = (INTS4)get_int(0., (REAL8)NUM_PEAK);
    return ((INTS4)(gauss_rnd(peak[cnt]+shift, sigma[cnt])));
    break;
  case 2:
    cnt = (INTS4)get_int(0., (REAL8)NUM_PEAK);
    return ((INTS4)(p_dNormal(peak2[cnt]+shift, sigma2[cnt], &seed)));
    break;
  case 3:
    return ((INTS4)(4096*p_dUniform(&seed)));
    break;
  case 4:
    return ((INTS4)(gauss_rnd(0., -.001)));
    break;
  case 5:
    return ((INTS4)(p_dExponential(100., &seed)));
    break;
  default:
    return 0;
    break;
    }
  
  return 0;
}

