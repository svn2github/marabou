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
/*+ Module      : cerror                                              */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : char *cerror(int ERROR, int FLAG)                   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Simple error handling routine                       */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     ERROR   : error number                                        */
/*                if error number < 0 the last internal errno number  */
/*                (_cam_errno) is use for output; for values >= 0     */
/*                the output corresponds to the given error number.   */
/*                error numbers and error messages are defined in     */
/*                esone/include/camdef.h                              */
/*+     FLAG    : if FLAG <> 0 date and time are inserted in the      */
/*                output.                                             */
/*                                                                    */
/*+ return value: char pointer to error message                       */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This function performs a simple error message       */
/*                handling. With the global variable _cam_errno it's  */
/*                possible to create your own error                   */
/*                handling together with this function.               */              
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cerror.c                                            */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>
#include "camdef.h"
#include "camlib.h"
#include "camdev.h"

#define STR_LEN 256

extern int _cam_errno;
char *my_time(char *);
char print_error[STR_LEN];

char *cerror (int my_error, int flag)
{
  char pc_time[STR_LEN];

  if (my_error < 0)
   { 
    my_error = _cam_errno;
   }
  
  sprintf(print_error, "");

  if ((flag != 0) && (my_error != EC_NORMAL)) 
   {
    sprintf(print_error, "%18s: ", my_time(pc_time));
   }

  switch (my_error)
    {
     case EC_NORMAL: strcat(print_error, M_EC_NORMAL);
     break;
     case EC_EBRANCH: strcat(print_error, M_EC_EBRANCH);
     break;
     case EC_ECRATE: strcat(print_error, M_EC_ECRATE);
     break;
     case EC_ESTATION: strcat(print_error, M_EC_ESTATION);
     break;
     case EC_EADDRESS: strcat(print_error, M_EC_EADDRESS);
     break;
     case EC_EFUNCODE: strcat(print_error, M_EC_EFUNCODE);
     break;
     case EC_IFUNCODE: strcat(print_error, M_EC_IFUNCODE);
     break;
     case EC_ELAM: strcat(print_error, M_EC_ELAM);
     break;
     case EC_LAMLNK: strcat(print_error, M_EC_LAMLNK);
     break;
     case EC_LAMNOTLNK: strcat(print_error, M_EC_LAMNOTLNK);
     break;
     case EC_NOLAMINTR: strcat(print_error, M_EC_NOLAMINTR);
     break;
     case EC_UNKNOWNDEV: strcat(print_error, M_EC_UNKNOWNDEV);
     break;
     case EC_DEVERR: strcat(print_error, M_EC_DEVERR);
     break;
     case EC_DEVTABLEFULL: strcat(print_error, M_EC_DEVTABLEFULL);
     break;
     case EC_DEVBUSY: strcat(print_error, M_EC_DEVBUSY);
     break;
     case EC_ADDFAULT: strcat(print_error, M_EC_ADDFAULT);
     break;
     case EC_EPARAM: strcat(print_error, M_EC_EPARAM);
     break;
     case EC_ECNAF: strcat(print_error, M_EC_ECNAF);
     break;
     case EC_SYSERR: strcat(print_error, M_EC_SYSERR);
     break;
     case EC_NODATABASE: strcat(print_error, M_EC_NODATABASE);
     break;
     case EC_REMOTEDEV: strcat(print_error, M_EC_REMOTEDEV);
     break;
     case EC_BLKTIMEOUT: strcat(print_error, M_EC_BLKTIMEOUT);
     break;
     case EC_ADDSCANTIMEOUT: strcat(print_error, M_EC_ADDSCANTIMEOUT);
     break;
     case EC_EBLOCK: strcat(print_error, M_EC_EBLOCK);
     break;
     case EC_UNKNOWNRPC: strcat(print_error, M_EC_UNKNOWNRPC);
     break;
     case EC_UNKNOWNHOST: strcat(print_error, M_EC_UNKNOWNHOST);
     break;
     case EC_UNKNOWNTRANSPORT: strcat(print_error, M_EC_UNKNOWNTRANSPORT);
     break;
     case EC_RPCOPENERROR: strcat(print_error, M_EC_RPCOPENERROR);
     break;
     case EC_NETXMITERR: strcat(print_error, M_EC_NETXMITERR);
     break;
     case EC_NETRCVERR: strcat(print_error, M_EC_NETRCVERR);
     break;
     case EC_NETTIMEOUT: strcat(print_error, M_EC_NETTIMEOUT);
     break;
     case EC_NOPROC: strcat(print_error, M_EC_NOPROC);
     break;
     case EC_BADXMSGFMT: strcat(print_error, M_EC_BADXMSGFMT);
     break;
     case EC_BADRMSGFMT: strcat(print_error, M_EC_BADRMSGFMT);
     break;  
     case EC_SRVRBADMSGFMT: strcat(print_error, M_EC_SRVRBADMSGFMT);
     break; 
     case EC_RPCVERSION: strcat(print_error, M_EC_RPCVERSION);
     break; 
     case EC_SRVRVERSION: strcat(print_error, M_EC_SRVRVERSION);
     break; 
     case EC_RPCNOPRIV: strcat(print_error, M_EC_RPCNOPRIV);
     break; 
     case EC_NOSERVER: strcat(print_error, M_EC_NOSERVER);
     break; 
     case EC_SRVRNOPROC: strcat(print_error, M_EC_SRVRNOPROC);
     break; 
     case EC_RPCPMAPERROR: strcat(print_error, M_EC_RPCPMAPERROR);
     break; 
     case EC_UNKNOWNSRVR: strcat(print_error, M_EC_UNKNOWNSRVR);
     break; 
     case EC_SRVRERROR: strcat(print_error, M_EC_SRVRERROR);
     break; 
     case EC_RPCERROR: strcat(print_error, M_EC_RPCERROR);
     break; 
     default: strcat(print_error, "unknown error !!!");
     break;
    } /* end switch */

  return(print_error);
}

char *my_time(char *g_pc_time)
{
  time_t tp;
  struct tm *p_st_time;

  time(&tp);
  p_st_time = localtime(&tp);

#ifdef Lynx
  p_st_time->tm_mday += 1; /* add 1 day due to POSIX error in localtime ! */
#endif
  
  strftime(g_pc_time, STR_LEN, "%d-%h-%y %T", p_st_time);

  return((char *) g_pc_time);
} 

