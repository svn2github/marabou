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
/*+ Module      : csubc                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int csubc(int FUNC, long EXT, int *DATA,            */
/*                         int *CB)                                   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Controller synchronized block transfer (16-bit)     */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     FUNC    : function code (0 - 31)                              */
/*+     EXT     : external addresse of a camac crate in               */
/*                encode format produced by function cdreg            */
/*+     DATA    : array of camac data words 16-bit(read or write dep. */
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
/*                given by the value FUNC to be executed at the CAMAC               */
/*                address specified by the value of EXT. In this                    */
/*                mode the CAMAC address is never changed, but the                  */
/*                single register is expected to supply or accept many              */
/*                words of data. It is assumed able to supply or                    */
/*                accept a data word whenever the controller addresses              */
/*                it until the block is exhausted or the controller                 */
/*                terminates the process because the number of data                 */
/*                transfers exceeds the limit given by the first                    */
/*                element of CB.                                                    */
/*                The module indicates that the block is exhausted by               */
/*                its Q response. It stops when Q = 0.                              */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : csubc.c                                             */
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

csubc (f, ext_p, data, cb)
unsigned int f;
struct camacadd *ext_p;
int *data;
struct ctrlblk *cb;
{
    int i,
        status;

    if (!(fcode_is_read (f) || fcode_is_write (f)))
    {
        _cam_errno = EC_IFUNCODE;
        return (-1);
    }
    ext_p->n_function = f;
    cb->mode = MODE_QSTOP;
	status = _camac_blk (ext_p, data, cb);
    if (status == 0 && (fcode_is_read (f) || fcode_is_rstatus (f)))
    {
        /* short access, mask high word */
        for (i = 0; i < cb->tally; i++)
            data [i] &= 0xffff;
    }
    return status;
}
