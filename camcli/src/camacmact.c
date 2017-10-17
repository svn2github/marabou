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

extern int _decode_neterr (int);

int _camac_mact (struct camacadd *ext[], int *f_p, int *data_p, int *q_p, struct ctrlblk *cb)
{
    int i;
    struct mact_in in;
    struct mact_out *out;
    struct camacadd_lcl *pippo;

    pippo = (struct camacadd_lcl *) 
            calloc(cb->rpt_count, sizeof (struct camacadd_lcl));
    in.ext.ext_val = pippo;

    if (cb->rpt_count > MAX_BLOCK)
    {
       _cam_errno = EC_EBLOCK;
        return (-1);
    }

    for(i = 0; i < cb->rpt_count; i++)
    {
     if (!IS_F_VALID(f_p[i]))
     {
        _cam_errno = EC_IFUNCODE;
        pippo = in.ext.ext_val;
        return (-1);
     }

     memcpy (pippo, ext[i], sizeof (struct camacadd_lcl));
     pippo++;
    }

    in.ext.ext_len = cb->rpt_count;
    in.cb = (struct ctrlblk_lcl*)cb;

        in.func.func_len = cb->rpt_count;
        in.func.func_val = f_p;

        in.data.data_len = cb->rpt_count;
        in.data.data_val = data_p;

        in.q_res.q_res_len = cb->rpt_count;
        in.q_res.q_res_val = q_p;

    /* Call remote procedure */
    out = _r_camac_mact_2 (&in, ext[0]->n_branch->n_client);

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
    memcpy (data_p, out->data.data_val, out->data.data_len * sizeof (int));
    memcpy (q_p, out->q_res.q_res_val, out->q_res.q_res_len * sizeof (int));
 
    return (0) ;
}
