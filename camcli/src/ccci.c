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
/*+ Module      : ccci                                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int ccci(long EXT, int L)                           */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Set or clear Dataway inhibit.                       */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     EXT     : external address of a camac crate in encode format  */
/*                produced by function cdreg                          */
/*+       L     : flag indicating whether inhibit is to set F_ENB or  */
/*                to clear F_DIS (F_ENB, F_DIS defined in camdef.h)   */
/*                                                                    */
/*+ return value: return -1 if X = 0, otherwise return 0              */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure causes the Dataway Inhibit to be set */
/*                in the crate specified by EXT if L is F_ENB         */
/*                and to be clear if L is F_DIS.                      */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : ccci.c                                              */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date          Purpose                               */
/*                Mar 2002 [RL] add support for different controllers */
/*                              (CBV, CC32, _cam_ctrl_type)           */
/*1- C Procedure *************+****************************************/
 
#include "camdef.h"
#include "camlib.h"

extern int _cam_ctrl_type; 
extern int _cam_notqx;

extern int _camac_act (struct camacadd *, int *);

int ccci (struct camacadd *ext_p, int l)
{
    struct camacadd ext;
    int status,
        x;

    ext = *ext_p;
	switch (_cam_ctrl_type) {
		case C_CC32:
    		ext.n_station = N_I_CC32;
    		ext.n_address = l ? A_I_ENB_CC32 : A_I_DIS_CC32;
    		ext.n_function = l ? F_I_ENB_CC32 : F_I_DIS_CC32;
			break;
		case C_CBV:
		default:
    		ext.n_station = N_I_CBV;
    		ext.n_address = l ? A_I_ENB_CBV : A_I_DIS_CBV;
    		ext.n_function = l ? F_I_ENB_CBV : F_I_DIS_CBV;
			break;
	}
    status = _camac_act (&ext, 0);
    if (status != 0)
        return (-1);
    /* return -1 if X = 0, otherwise return 0 */
    x = !(_cam_notqx & NOT_X);
    return (x ? 0 : -1);
}
