/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : f_ut_ini_evt                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : l_status = f_ut_ini_evt()      */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Routine to initialize event generator.              */
/*                                                                    */
/*+ ARGUMENTS   : None                                                */
/*                                                                    */
/*+ Return type : INTS4                                               */
/*                  0 : success                                       */
/*                 -1 : failure                                       */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/* Generates events with gaussian peaks.                              */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : f_ut_ini_evt.c                                      */
/*+ Home direct.: /mbs/prod/src                                       */
/*+ Version     : 1.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 20-Jan-1997                                         */
/*+ Object libr.:                                                     */
/*+ Updates     : Date        Purpose                                 */
/*1- C Procedure *************+****************************************/
#include "typedefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_ut_ini_rand_event.h"

INTS4 f_ut_ini_evt(char *fname)
{
  INTS4 l_status=0;
  FILE *fp;
  
  l_status = get_rand_seed();
  
  fp = fopen(fname, "r");
  if(fp == NULL)
    {
      fprintf(stderr, "put-file does not exist !\n");
      return (-1);
    }
  fscanf(fp, "%d %d %d %d\n", &l_subevts, &l_max, &l_mode, &l_func);
  fclose(fp);
  
  return(l_status);
}

/*
**	Function name : get_rand_seed
**
**	Description : generate random seed for drand48()
**	Input : None
**	Output : 0=success, -1=failure
*/
INTS4 get_rand_seed()                         /* set seed for generator      */
{
  INTS4 ret;
  struct timeval tv;
  struct timezone tz;
  
  ret = gettimeofday(&tv, &tz);          /* may not be supported in your C  */

  seed=tv.tv_sec + tv.tv_usec;     /* if not, replace with clock()    */
  
  srand48(seed);
  
  return (ret);
}

