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
/*+ Module      : cdreg                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int cdreg(long *EXT, long B, int C, int N, int A)   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Declare CAMAC register                              */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     EXT     : external address of a camac crate in encoded format */
/*                produced by function cdreg                          */
/*+       B     : external host address in encoded format produced    */
/*                by function ccopen                                  */
/*+       C     : crate number (0 - 15)                               */
/*+       N     : station number (0 - 31)                             */
/*+       A     : subaddress (0 - 15)                                 */ 
/*                                                                    */
/*+ return value: return 0                                            */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure combines the host address, the       */
/*                crate number, the station number and the subaddress */
/*                into a convenient format for later usage and stores */
/*                the result in the struct camacadd referenced by the */
/*                pointer EXT.                                        */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cdreg.c                                             */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include <stdlib.h>
#include "camdef.h"
#include "camlib.h"

extern int _cam_errno;

cdreg (ext, b, c, n, a)
struct camacadd **ext;
struct camacadd *b;
int c;
int n;
int a;
{
    _cam_errno = EC_NORMAL;
 
   /* check input parameters */
    if (b->n_valid != VALID_DEV)
    {
	_cam_errno = EC_EBRANCH;
	return (-1);     
    }
    if (c < 0 || c > MAX_CRATES)
    {
	_cam_errno = EC_ECRATE;
	return (-1);     
    }
    if (n <= 0 || n > MAX_STATIONS)
    {
	_cam_errno = EC_ESTATION;
	return (-1);     
    }
    if (a < 0 || a > MAX_ADDRESSES)
    {
	_cam_errno = EC_EADDRESS;
	return (-1);       
    }

    /*
      since ext is a pointer, space for the camacadd
      structure must be allocated permanently
    */
    *ext = (struct camacadd *)calloc (sizeof (struct camacadd), 1);
    **ext = *b;
    (*ext)->n_branch = b;
    (*ext)->n_crate = c;
    (*ext)->n_station = n;
    (*ext)->n_address = a;
    (*ext)->n_function = -1;
    return 0;
}
