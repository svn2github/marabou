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
/*+ Module      : csmad                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : int csmad(int FUNC, long EXT, int *DATA,            */
/*                         int *CB)                                   */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Address scan mode 16-bit                            */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     FUNC    : function code (0 - 31)                              */
/*+     EXT     : two external addresse of a camac crates in          */
/*                encode format produced by function cdreg            */
/*                (start (ext[0]) and stop (ext[1]) addresses)        */
/*+     DATA    : array of camac data words 16-bit(read or write dep. */
/*                of camac function codes)                            */
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
/*                The address scan mode is used when a block of registers or        */
/*                modules are to be read (or written onto) sequentially. The        */
/*                modules involved need not be located at consecutive addresses,    */
/*                however, subaddresses within a given module must be so.           */
/*                The Q-response is used to determine if an address is occupied or  */
/*                not. For occupied Q = 1 is returned. With N(station nummber)      */
/*                held constant, the subaddress is then incremented and transfers   */
/*                made until Q = 0 is returned. A(subadd.) is then reset to A(0)    */
/*                and N is incremented, etc.                                        */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : csmad.c                                             */
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

csmad (f, ext_h, data, cb)
unsigned int f;
struct camacadd *ext_h [];
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
    ext_h [0]->n_function = ext_h [1]->n_function = f;
    status = _camac_addscan (ext_h, data, cb);
    if (status == 0 && (fcode_is_read (f) || fcode_is_rstatus (f)))
    {
        /* short access, mask high word */
        for (i = 0; i < cb->tally; i++)
            data [i] &= 0xffff;
    }
    return status;
}
