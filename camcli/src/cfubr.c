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
/*+ Module      : cfubr                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int cfubr(int FUNC, long EXT, int *DATA,            */
/*                         int *CB)                                   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Controller synchronized block transfer              */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     FUNC    : function code (0 - 31)                              */
/*+     EXT     : external addresse of a camac crate in               */
/*                encode format produced by function cdreg            */
/*+     DATA    : array of camac data words (read or write dep.       */
/*                of camac function codes)                            */
/*+     CB      : control block. the contents of the four array       */
/*                elements are:                                       */
/*                 CB(0) -> repeat_count (1 - 32768)                   */
/*                 CB(1) -> tally                                      */
/*                 CB(2) -> lam_id (not implemented !!!)               */
/*                 CB(3) -> mode (fixed !!!)                           */
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
/*                This procedure causes the single CAMAC function                   */
/*                specified by the contents of FUNC to be executed at               */
/*                the CAMAC address specified by the contents of                    */
/*                EXT with the usage of the Q response for the                      */
/*                Repeat mode. In this mode the CAMAC address is never              */
/*                changed, but the single register is expected to                   */
/*                supply or accept many words of data. Q is used as a               */
/*                timing signal. Q=1 indicates that the previously                  */
/*                executed function succeeded; Q=0 indicates that the               */
/*                module was not ready to execute the function and                  */
/*                that the controller should try again. Any data words              */
/*                transfered are placed into or taken from the array                */
/*                data. If the response is Q=0, no transfer took                    */
/*                place, and the index into the array DATA is not                 */
/*                changed. The number of Q=1 reponses expected is                   */
/*                given by the contents of the first element of CB.               */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : cfubr.c                                             */
/*+ Home direct.: esone/src/camcli                                    */
/*+ Object libr.: esone/lib/libcamcli.a                               */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 15-May-1996                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/
 
#include "camdef.h"
#include "camlib.h"

extern int _cam_errno;
extern int _camac_blk (struct camacadd *, int *, struct ctrlblk *);

int cfubr (unsigned int f, struct camacadd *ext_p, int *data, struct ctrlblk *cb)
{
    int status;

    if (!(fcode_is_read (f) || fcode_is_write (f)))
    {
        _cam_errno = EC_IFUNCODE;
        return (-1);
    }
    ext_p->n_function = f;
    cb->mode = MODE_QREPEAT;
    status = _camac_blk (ext_p, data, cb);
    return status;
}
