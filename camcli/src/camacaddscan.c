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

	Date    :			Author :
	Modification

--------------------------------------------------------------*/

#include <stdio.h>
#include <rpc/rpc.h>
#include "camdef.h"
#include "camlib.h"
#include "camacrpc.h"

extern int _cam_errno;
extern int _cam_notqx;

#ifdef __STDC__
int _camac_addscan (struct camacadd *ext[], int *data_p, struct ctrlblk *cb)
#else
int _camac_addscan (ext, data_p, cb)
struct camacadd *ext[];
int *data_p;
struct ctrlblk *cb;
#endif
{
    struct addscan_in in;
    struct addscan_out *out;
    struct camacadd_lcl pippo[2];

    memcpy (&pippo[0], ext[0], sizeof (struct camacadd_lcl));
    memcpy (&pippo[1], ext[1], sizeof (struct camacadd_lcl));
    in.extarr.extarr_len = 2;
    in.extarr.extarr_val = &pippo[0];
/*    in.extarr.extarr_val = (struct camacadd_lcl **)ext;
*/
    in.cb = (struct ctrlblk_lcl*)cb;

    if (cb->rpt_count > MAX_BLOCK)
    {
       _cam_errno = EC_EBLOCK;
        return (-1);
    }

    if (!IS_F_VALID(ext[0]->n_function))
    {
       _cam_errno = EC_IFUNCODE;
        return (-1);
    }

    if (fcode_is_write (ext[0]->n_function))
    {
        in.data.data_len = cb->rpt_count;
        in.data.data_val = data_p;
    }
    else
    {
        in.data.data_len = 0;
        in.data.data_val = data_p;
    }

    /* Call remote procedure */
    out = _r_camac_addscan_2 (&in, ext[0]->n_branch->n_client);

    /* checks */
    if (out == NULL)
    {
	struct rpc_err neterr;

	clnt_geterr ((CLIENT*)ext[0]->n_branch->n_client, &neterr);
        _cam_errno = _decode_neterr (neterr.re_status);
	return (-1);
    }

    if (out->errno != 0)
    {
        _cam_errno = out->errno;
	return (-1);
    }

    _cam_notqx = out->status;
    _cam_errno = 0;
    memcpy (cb, out->cb, sizeof (struct ctrlblk));
    if (fcode_is_read (ext[0]->n_function) || fcode_is_rstatus (ext[0]->n_function))
    {
        memcpy (data_p, out->data.data_val, out->data.data_len * sizeof (int));
    }

    return (0) ;
}
