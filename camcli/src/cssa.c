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
/*+ Module      : cssa                                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int cssa(int FUNC, long EXT, int *DATA, int *Q)     */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Perform single CAMAC action with 16 bit data words  */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     FUNC    : camac function code (0 - 31)                        */
/*+     EXT     : external address of a camac crate in encode format  */
/*                produced by function cdreg                          */
/*+     DATA    : camac data word 16 bit (read or write dep. of camac */
/*                function code)                                      */
/*+     Q       : Q/X response                                        */
/*                                                                    */
/*+ return value: return -1 if X = 0, otherwise return 0              */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure causes the CAMAC specified by the                  */
/*                function code to be performed at the CAMAC address                */
/*                specified by EXT. If the function is a read or                    */
/*                write code, a 16-bit data transfer occurs                         */
/*                between the CAMAC register and the given variable.                */
/*                The state of X and Q resulting from the operation                 */
/*                is stored in q.                                                   */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cssa.c                                              */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include <stdio.h>
#include "camdef.h"
#include "camlib.h"

extern int _cam_notqx;

extern int _camac_act (struct camacadd *, int *);

int cssa (unsigned int f, struct camacadd *ext_p, int *data, int *qptr)
{
    int q,
        status,
        x;

    ext_p->n_function = f;
    status = _camac_act (ext_p, data);
    if (status != 0)
    {
        return (-1);
    }
    if (fcode_is_read (f) || fcode_is_rstatus (f))
    {
        /* short access, mask high word */
        *data &= 0xffff;
    }
    q = !(_cam_notqx & NOT_Q);
    if (qptr != NULL)
    {
        *qptr = q;
    }
    /* return -1 if X = 0, otherwise return Q */
    x = !(_cam_notqx & NOT_X);
    return (x ? q : -1);
}
