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
/*+ Module      : csga                                                */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int csga(int *FUNC, long EXT, int *DATA, int *Q,    */
/*                         int *CB)                                   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : General multiple CAMAC action (16-bit words)        */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     FUNC    : array of camac function codes (0 - 31)              */
/*+     EXt     : array of external addresses of a camac crates in    */
/*                encode format produced by function cdreg            */
/*+     DATA    : array of camac data words 16-bit(read or write dep. */
/*                of camac function codes)                            */
/*+     Q       : array of Q/X responses. Stored in an integer        */
/*                element as X = bit 1, Q = bit 0, all others are     */
/*                zero.                                               */
/*+     CB      : control block. the contents of the four array       */
/*                elements are:                                       */
/*                 CB(0) -> repeat_count (1 - 32768)                   */
/*                 CB(1) -> tally                                      */
/*                 CB(2) -> lam_id (not implemented !!!)               */
/*                 CB(3) -> mode (not used)                            */
/*                 The repeat count specifies the number of individual */
/*                CAMAC actions or the maximum number of data words   */
/*                to be transfered. The tally is the number of actions*/
/*                actually performed or the number of words actually  */
/*                transfered. If the action is terminated by the      */
/*                exhaustion of the repeat count, the tally will be   */
/*                equal to the repeat count, otherwise it may be less.*/
/*                The LAM identification is not used in this          */
/*                implementation and must be zero.                    */                            
/*                                                                    */
/*+ return value: return -1 if X = 0, otherwise return 0              */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                This procedure causes a sequence of CAMAC functions               */
/*                specified in successive elements of FUNC to be                    */
/*                performed at a corresponding sequence of CAMAC                    */
/*                addresses specified in successive elements of                     */
/*                EXT. Any read or write function in FUNC causes a                  */
/*                CAMAC data word be transfered between the                         */
/*                corresponding element of DATA and the specified                   */
/*                CAMAC register. The X/Q reponse of each individual                */
/*                action is stored in the corresponding element of                  */
/*                Q. The number of actions to be performed and the                  */
/*                minimal dimension of DATA and Q is given by the                   */
/*                value contained in the first element of CB.                       */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : csga.c                                              */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include "camdef.h"
#include "camlib.h"
#include "camdev.h"

extern int _cam_notqx;

int csga (f, ext, data, q, cb)
unsigned int *f;
struct camacadd *ext [];
int *data;
int *q;
struct ctrlblk *cb;
{
    int i, status;

    status = _camac_mact (ext, data, q, cb);

      /* short access, mask high word */
      for (i = 0; i < cb->tally; i++)
      {
       data [i] &= 0xffff;
      }

    return (status);
}
