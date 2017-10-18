/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "camacrpc.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

struct open_out * _r_camac_open_2(struct open_in *argp, CLIENT *clnt)
{
	static struct open_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_OPEN,
		(xdrproc_t) xdr_open_in, (caddr_t) argp,
		(xdrproc_t) xdr_open_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct init_out * _r_camac_init_2(struct init_in *argp, CLIENT *clnt)
{
	static struct init_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_INIT,
		(xdrproc_t) xdr_init_in, (caddr_t) argp,
		(xdrproc_t) xdr_init_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct onl_out * _r_camac_onl_2(struct onl_in *argp, CLIENT *clnt)
{
	static struct onl_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_ONL,
		(xdrproc_t) xdr_onl_in, (caddr_t) argp,
		(xdrproc_t) xdr_onl_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct act_out * _r_camac_act_2(struct act_in *argp, CLIENT *clnt)
{
	static struct act_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_ACT,
		(xdrproc_t) xdr_act_in, (caddr_t) argp,
		(xdrproc_t) xdr_act_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct addscan_out * _r_camac_addscan_2(struct addscan_in *argp, CLIENT *clnt)
{
	static struct addscan_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_ADDSCAN,
		(xdrproc_t) xdr_addscan_in, (caddr_t) argp,
		(xdrproc_t) xdr_addscan_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct blk_out * _r_camac_blk_2(struct blk_in *argp, CLIENT *clnt)
{
	static struct blk_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_BLK,
		(xdrproc_t) xdr_blk_in, (caddr_t) argp,
		(xdrproc_t) xdr_blk_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

struct mact_out * _r_camac_mact_2(struct mact_in *argp, CLIENT *clnt)
{
	static struct mact_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, _R_CAMAC_MACT,
		(xdrproc_t) xdr_mact_in, (caddr_t) argp,
		(xdrproc_t) xdr_mact_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
