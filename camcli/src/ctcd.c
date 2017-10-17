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
/*+ Module      : ctcd                                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int ctcd(long EXT, int *L)                          */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Test crate demand enabled                           */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     EXT     : external address of a camac crate in encode format  */
/*                produced by function cdreg                          */
/*+       L     : flag returning demand enable status                 */
/*                                                                    */
/*+ return value: return -1 if X = 0, otherwise return 0              */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure sets the value of L to 0x1 if                      */
/*                crate demand is enabled in the crate specified by                 */
/*                ext and sets L to 0x0 if crate demand is disabled                 */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : ctcd.c                                              */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date          Purpose                               */
/*                Mar 2002 [RL] add support for different controllers */
/*                              (CBV, CC32, _cam_ctrl_type)           */
/*1- C Procedure *************+****************************************/
 
#include <stdio.h>
#include "camdef.h"
#include "camlib.h"

extern int _cam_ctrl_type; 
extern int _cam_notqx;

extern int _camac_act (struct camacadd *, int *);

int ctcd (struct camacadd *ext_p, int *lptr)
{
    struct camacadd ext;
    int q,
        status,
        x;

    ext = *ext_p;
	switch (_cam_ctrl_type) {
		case C_CC32:
		case C_CBV:
			return(-1);
			break;
		default:
			ext.n_station = N_D;
			ext.n_address = A_D;
			ext.n_function = F_TST;
			break;
	}
    status = _camac_act (&ext, 0);
    if (status != 0)
    {
        return (-1);
    }
    q = !(_cam_notqx & NOT_Q); 
    if (lptr != NULL)
    {
        *lptr = q;
    }
    /* return -1 if X = 0, otherwise return Q */
    x = !(_cam_notqx & NOT_X);
    return (x ? q : -1);
}
