/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "camacrpc.h"

bool_t
xdr_camacadd_lcl (XDR *xdrs, camacadd_lcl *objp)
{
	 if (!xdr_int (xdrs, &objp->n_path))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->n_cnaf, 4,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t xdr_ctrlblk_lcl (XDR *xdrs, ctrlblk_lcl *objp)
{
	 long *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = (long *) xdr_inline(xdrs,4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->rpt_count))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->tally))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->lam_id))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
		} else {
			IXDR_PUT_U_LONG(buf, objp->rpt_count);
			IXDR_PUT_U_LONG(buf, objp->tally);
			IXDR_PUT_U_LONG(buf, objp->lam_id);
			IXDR_PUT_U_LONG(buf, objp->mode);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = (long *) xdr_inline(xdrs,4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->rpt_count))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->tally))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->lam_id))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
		} else {
			objp->rpt_count = IXDR_GET_U_LONG(buf);
			objp->tally = IXDR_GET_U_LONG(buf);
			objp->lam_id = IXDR_GET_U_LONG(buf);
			objp->mode = IXDR_GET_U_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_u_int (xdrs, &objp->rpt_count))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->tally))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->lam_id))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->mode))
		 return FALSE;
	return TRUE;
}

bool_t xdr_act_in (XDR *xdrs, act_in *objp)
{
	 if (!xdr_pointer (xdrs, (char **)&objp->ext, sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->data))
		 return FALSE;
	return TRUE;
}

bool_t xdr_act_out (XDR *xdrs, act_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->data))
		 return FALSE;
	return TRUE;
}

bool_t xdr_addscan_in (XDR *xdrs, addscan_in *objp)
{
	 if (!xdr_array (xdrs, (char **)&objp->extarr.extarr_val, (u_int *) &objp->extarr.extarr_len, 2,
		sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t xdr_addscan_out (XDR *xdrs, addscan_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t xdr_onl_in (XDR *xdrs, onl_in *objp)
{
	 if (!xdr_pointer (xdrs, (char **)&objp->ext, sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	return TRUE;
}

bool_t xdr_onl_out (XDR *xdrs, onl_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}

bool_t xdr_init_in (XDR *xdrs, init_in *objp)
{
	 if (!xdr_pointer (xdrs, (char **)&objp->ext, sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	return TRUE;
}

bool_t xdr_init_out (XDR *xdrs, init_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}

bool_t xdr_open_in (XDR *xdrs, open_in *objp)
{
	 if (!xdr_string (xdrs, &objp->dev, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->unit))
		 return FALSE;
	return TRUE;
}

bool_t xdr_open_out (XDR *xdrs, open_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->path))
		 return FALSE;
	return TRUE;
}

bool_t xdr_blk_in (XDR *xdrs, blk_in *objp)
{
	 if (!xdr_pointer (xdrs, (char **)&objp->ext, sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t xdr_blk_out (XDR *xdrs, blk_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t xdr_mact_in (XDR *xdrs, mact_in *objp)
{
	 if (!xdr_array (xdrs, (char **)&objp->ext.ext_val, (u_int *) &objp->ext.ext_len, ~0,
		sizeof (camacadd_lcl), (xdrproc_t) xdr_camacadd_lcl))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->func.func_val, (u_int *) &objp->func.func_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->q_res.q_res_val, (u_int *) &objp->q_res.q_res_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t xdr_mact_out (XDR *xdrs, mact_out *objp)
{
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->cb, sizeof (ctrlblk_lcl), (xdrproc_t) xdr_ctrlblk_lcl))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->q_res.q_res_val, (u_int *) &objp->q_res.q_res_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}
