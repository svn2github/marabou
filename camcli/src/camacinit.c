/*
$Copyright
 * |------------------------------------------------------|
 * | Copyright (c) 1991 , 1992 Ibex Computing SA          |
 * | All Rights Reserved                                  |
 * |------------------------------------------------------|
 * |        Ibex Computing                                |
 * |        International Business Park                   |
 * |        F-74160 ARCHAMPS                              |
 * |------------------------------------------------------|
 * $
*/

/*--------------------------------------------------------------

	@(#)__name__        v 1.0 - __date__
	__name__: 

	Author : Alessandro Miotto	Ibex Computing
	Date    : __date__

	Date    :  28/7/93		Author : Ann-Kari Brignet
	Modification ext->n_branch->n_client changed to ext->n_client
    to avoid segmentation fault.

--------------------------------------------------------------*/
#include <stdio.h>
#include <rpc/rpc.h>
#include "camdef.h"
#include "camlib.h"
#include "camacrpc.h"

extern int _cam_errno;
extern int _cam_notqx;

extern int _decode_neterr (int);

int _camac_init (struct camacadd *ext)
{
    struct init_in in;
    struct init_out *out;

    in.ext = (struct camacadd_lcl*)ext;
    /* Call remote procedure */
    out = _r_camac_init_2 (&in, ext->n_client);

    /* checks */
    if (out == NULL)
    {
	struct rpc_err neterr;

	clnt_geterr ((CLIENT*)ext->n_client, &neterr);
        _cam_errno = _decode_neterr (neterr.re_status);
	return (-1);
    }
    if (out->errno != 0)
    {
	_cam_notqx = 0;
        _cam_errno = out->errno;
	return (-1);
    }

    _cam_notqx = out->status;
    _cam_errno = 0;

    return (0) ;
}
