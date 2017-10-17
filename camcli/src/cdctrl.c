/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : cdctrl                                              */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int cdreg(int ctrl_type)   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Declare type of CAMAC controller                    */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+       ctrl_type     : controller type                             */ 
/*                                                                    */
/*+ return value: return 0                                            */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure defines the camac controller         */
/*                to be used (CBV or CC32)                            */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cdctrl.c                                            */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: R. Lutter                                           */
/*+ Created     : 14-Mar-2002                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include <stdlib.h>
#include "camdef.h"
#include "camlib.h"

int _cam_ctrl_type;

extern int _cam_errno;
int cdctrl (int ctrl_type)
{
    _cam_errno = EC_NORMAL;
	_cam_ctrl_type = ctrl_type; 
	return 0;
}
