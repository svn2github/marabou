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
/*+ Module      : ctstat                                              */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int ctstat(int *L)                                  */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : test status of preceding camac action               */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+       L     : status code                                         */
/*                                                                    */
/*+ return value: L (same as parameter)                               */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure stores an integer status code into the parameter L.*/
/*                The status code reflects the results of the last action executed  */
/*                by another functioncall to the esone library (libcamcli).         */
/*                After a cfga call the status reflects the worst case response of  */
/*                all camac accesses.                                               */
/*                The status code has the following meaning:                        */
/*                 L -> X Q                                                          */
/*                 0    1 1                                                          */
/*                 1    1 0                                                          */
/*                 2    0 1                                                          */
/*                 3    0 0                                                          */
/*                 If some errors occurs the errno number specific to this           */
/*                implementation (errno numbers are defined in                      */
/*                esone/include/camdef.h) is shifted by two bits and coded in the   */
/*                first 30 bit's of the integer L.                                  */
/*                E.g. errno = EC_IFUNCODE (illegal function code) = 6,             */
/*                X = 1, Q = 0 -> L = 0000 0000 0001 1001 = 0x19                    */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : ctstat.c                                            */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date                                                */
/*+ Purpose     :                                                     */
/*1- C Procedure *************+****************************************/
 
#include <stdio.h>
#include "camlib.h"

extern int _cam_errno;
extern int _cam_notqx;

/*
    X Q -> K
    1 1    0
    1 0    1
    0 1    2
    0 0    3
*/

int ctstat (int * kptr)
{
    int k;

    k = _cam_notqx + (_cam_errno << 2);
    if (kptr != NULL)
        *kptr = k;
    return k;
}
