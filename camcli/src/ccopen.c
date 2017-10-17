/*
 * |------------------------------------------------------|
 * | Copyright (c) 1991 , 1992 Ibex Computing SA          |
 * | All Rights Reserved                                  |
 * |------------------------------------------------------|
*/
 
/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : ccopen                                              */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int ccopen(char *HOSTNAME, long *HOSTADD)           */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Enable or Disable Crate Demand                      */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+  HOSTNAME   : hostname as character pointer e.g. e7_19, cvc8      */
/*+   HOSTADD   : external address of host in encoded format          */
/*                returned by function ccopen                         */
/*                                                                    */
/*+ return value: return -1 if error occurs, otherwise return 0       */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure create an encoded address for use    */
/*                with function cdreg from the input parameter        */
/*                HOSTNAME.                                           */
/*                This function has to be called only once for        */
/*                each host at the begin of the user programm !!!     */                               
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : ccopen.c                                            */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camdef.h"
#include "camlib.h"

int     _cam_errno;
int     _cam_notqx;

static void copycamacent (struct camacent*, struct camacent*);
extern int _camac_open (struct camacent *, struct camacadd **);

int ccopen (char *camac, struct camacadd **net_h)
{
    int status;
    char *s;
    struct camacent camcam;  
    struct camacent *cam;
    static struct camacent  cam_dflt;

    /*
      SET DEFAULT NODE AND SERVER - USE ENV. VARIABLE FIRST
    */
  if (cam_dflt.c_name == NULL)
   {
    if ((s = getenv (DFLT_CAMAC)) != NULL)
     {
      camcam.c_dev = (char *)"vcc";
      camcam.c_unit = 1;
      camcam.c_serv = (char *)"camacd";
      camcam.c_name = s;
      camcam.c_host = s;
      camcam.c_prot = (char *)"tcp";
      copycamacent( &cam_dflt, &camcam);
     }
   }

   /*
     SET INPUT PARAMTERS - DEFAULT IF NOT SPECIFIED
   */
   if (camac != NULL)
     {
      camcam.c_dev = (char *)"vcc";
      camcam.c_unit = 1;
      camcam.c_serv = (char *)"camacd";
      camcam.c_name = camac;
      camcam.c_host = camac;
      camcam.c_prot = (char *)"tcp";
      copycamacent( &camcam, &camcam);
      cam = &camcam;
    }
    else
    {
     if (cam_dflt.c_name == NULL)
      {
       _cam_errno = EC_UNKNOWNHOST;
       return(-1);
      }
     else
      {
	cam = &cam_dflt;
      }
    }

    /*
      CALL LIBRARY
    */
    status = _camac_open (cam, net_h);
    if (status == 0)
    {
        _cam_errno = EC_NORMAL;
        return 0;
    }
    else
    {
	/* _cam_errno already filled by called routine */
        return (-1);
    }
}

/*
  copy struct camacent from s to d, and allocate
  permanently space for strings.
*/
static void copycamacent (struct camacent *d, struct camacent *s)
{
    struct camacent t;

    t = *s;             /* allows s == d */
    d->c_name = malloc (strlen (t.c_name) + 1);
    strcpy (d->c_name, t.c_name);
    d->c_host = malloc (strlen (t.c_host) + 1);
    strcpy (d->c_host, t.c_host);
    d->c_serv = malloc (strlen (t.c_serv) + 1);
    strcpy (d->c_serv, t.c_serv);
    d->c_dev = malloc (strlen (t.c_dev) + 1);
    strcpy (d->c_dev, t.c_dev);
    d->c_unit = t.c_unit;
}
