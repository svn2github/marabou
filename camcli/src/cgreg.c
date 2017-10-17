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
/*+ Module      : cgreg                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int cgreg(long EXT, long *B, int *C, int *N, int *A)*/
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Analyse register identifier                         */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     EXT     : external address of a camac crate in encoded format */
/*                produced by function cdreg                          */
/*+       B     : external host address in encoded format produced    */
/*+       C     : crate number (0 - 15)                               */
/*+       N     : station number (0 - 31)                             */
/*+       A     : subaddress (0 - 15)                                 */ 
/*                                                                    */
/*+ return value: return 0                                            */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure decodes the CAMAC address identifier               */
/*                into its component parts host address, crate                      */
/*                number, station number and subaddress. This                       */
/*                procedure exactly reverses the process performed by               */
/*                cdreg, all parameters have the same interpretation                */
/*                and form. This routine is the only function which                 */
/*                don't reset the errno number !!!                                  */                
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cgreg.c                                             */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
  
#include "camlib.h"

int cgreg (struct camacadd *ext, struct camacadd **b, int *c, int *n, int *a)
{
    *b = ext->n_branch;
    *c = ext->n_crate;
    *n = ext->n_station;
    *a = ext->n_address;
    return 0;
}
