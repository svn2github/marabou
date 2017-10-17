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
/*+ Module      : camacopen                                           */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : _camac_open                                         */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : create client                                       */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ CALLING     : int _camc_open(struct camacent *cam,                */
/*                struct camacadd **net)                              */
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+     cam    : struct camacent *                                    */
/*                                              */
/*+     net    : struct camacadd **                                   */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*       */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : camacopen.c                                         */
/*+ Home direct.: esone/src/libcamr                                   */
/*+ Object libr.: esone/lib/libcamr.a                                 */
/*+ Version     : 1.0                                                 */
/*+ Maintainance: F. Humbert                                          */
/*+ Created     : 09-Dec-1995                                         */
/*+ Updates     : Date         Purpose                                */
/*1- C Procedure *************+****************************************/

#include <stdio.h>
#ifndef Lynx
/* Lynx-OS has malloc declared into the rpc include files! */
#include <stdlib.h>
/* to declare getrpcbyname in netdb.h ! 12.11.95 f.h. */
#define _SUN
#include <netdb.h>
#endif /* Lynx */
#include <rpc/rpc.h>

#include "camlib.h"
#include "camacrpc.h"

extern int _cam_errno;

/* HASH TABLE */
struct dev
{
    char *dev;
    CLIENT *cl;
    struct camacadd *add;
};

struct timeval tv; /* struct for changing client timeout */

static int cmpdev (char*, struct dev*);
static int hput (char*, void*);
static void *hget (char*, int (*)());

int _camac_open (struct camacent *cam, struct camacadd **net)
{
    struct open_in in;
    struct open_out *out;
    struct rpcent *r;
    char name [80];
    struct dev *p ;
    long rnumber;    

/*  changed 06.12.95 f.h.
    r = getrpcbyname (cam->c_serv);

    if (r == NULL)
    {
        _cam_errno = EC_UNKNOWNRPC;
        return (-1);
    }
*/
    rnumber = PROGRPC;

    /*** patch 5/11/93 - see mail miotto ***/
    sprintf(name,"%s@%s-%d",cam->c_name, cam->c_host, rnumber);

    if ((p = hget (name, cmpdev)) == NULL)
    {
        p = (struct dev *)malloc (sizeof (struct dev));
        p->dev = malloc (strlen (name) + 1);
        strcpy (p->dev, name);
	p->add = (struct camacadd *)calloc (1, sizeof (struct camacadd));

	/* create client */
	if (cam->c_prot == NULL)
	{
	    cam->c_prot = "tcp";
	}
	p->cl = clnt_create (cam->c_host, rnumber,
			     PROCVERS, cam->c_prot);
	if (p->cl == NULL)
	{
            _cam_errno = EC_RPCOPENERROR;
	    return (-1);
	}
        
        tv.tv_sec = 300; /* change timeout to 5 minutes */ 
        tv.tv_usec = 0; /* default value */
        clnt_control(p->cl, CLSET_TIMEOUT, (char *) &tv);        

        if (hput (p->dev, p) != 0)
        {
            _cam_errno = EC_DEVTABLEFULL;
            return (-1);
        }
    }
    else
    {
	/*
	  destroy existing connection and create a new one
	  (in order to reconnect when the remote server died
	  and the old client is not valid anymore)
 	*/
        if (p->cl != NULL)
	{
	    clnt_destroy (p->cl);
	}
	p->cl = clnt_create (cam->c_host, rnumber,
			     PROCVERS, cam->c_prot);

        tv.tv_sec = 300; /* change timeout to 5 minutes */ 
        tv.tv_usec = 0; /* default value */
        clnt_control(p->cl, CLSET_TIMEOUT, (char *) &tv); 

	if (p->cl == NULL)
	{
            _cam_errno = EC_RPCOPENERROR;
	    return (-1);
	}
    }

    p->add->n_client = p->cl ;

    in.dev = cam->c_dev;
    in.unit = cam->c_unit;

    /* Call remote procedure */
    out = _r_camac_open_2 (&in, p->add->n_client);

    /* checks */
    if (out == NULL)
    {
	struct rpc_err neterr;

	clnt_geterr ((CLIENT*)p->add->n_client, &neterr);
        _cam_errno = _decode_neterr (neterr.re_status);
	return (-1);
    }
    if (out->errno != 0)
    {
        _cam_errno = out->errno;
	return (-1);
    }

    p->add->n_path = out->path;
    p->add->n_valid = VALID_DEV;

    /* Copy values */
    *net = p->add;

    return (0) ;
}

static int cmpdev (char *k, struct dev *s)
{
     return (strcmp (k, s->dev));
}

/*
** HASH ROUTINES
*/

#define HASH_DIM        64

static void *hash_table [HASH_DIM];
static int hash_count;

static int hash (char*);

static int hput (char *k, void *x)
{
    unsigned int hashval = hash (k);
    unsigned int h = hashval & (HASH_DIM - 1);
    unsigned int inc = ((((hashval / HASH_DIM ) << 1) + 1) & (HASH_DIM - 1));

    if ((hash_count + 1) == (HASH_DIM - 1))
    {
        return (-1);
    }

    while (1)
    {
        if (hash_table [h] == NULL)
        {
            hash_table [h] = x;
            hash_count++;
            return 0;
        }
        h = ((h + inc) & (HASH_DIM - 1));
    }
}

static void *hget (char *k, int (*cmp)())
{
    unsigned int hashval = hash (k);
    unsigned int h = hashval & (HASH_DIM - 1);
    unsigned int inc = ((((hashval / HASH_DIM ) << 1) + 1) & (HASH_DIM - 1));

    while (hash_table [h] != NULL)
    {
        if ((*cmp)(k, hash_table [h]) == 0)
        {
            return (hash_table [h]);
        }
        else
        {
            h = ((h + inc) & (HASH_DIM - 1));
        }
    }
    return NULL;
}

static int hash (char *k)
{
    unsigned int h = 0;
    unsigned int g;

    while (*k != 0)
    {
        h = (h << 4) + *k++;
        if ((g = h & 0xf0000000) != 0)
            h = (h ^ (g >> 24)) ^ g;
    }
    return h;
                          
}

int _decode_neterr (int neterr)
{
    switch (neterr)
    {
        case RPC_SUCCESS:
        return EC_NORMAL;

        /* XDR errors */
        case RPC_CANTENCODEARGS:
        return EC_BADXMSGFMT;
        case RPC_CANTDECODERES:
        return EC_BADRMSGFMT;
        case RPC_CANTDECODEARGS:
        return EC_SRVRBADMSGFMT;

        /* (local) transmission errors */
        case RPC_CANTSEND:
        return EC_NETXMITERR;
        case RPC_CANTRECV:
        return EC_NETRCVERR;
        case RPC_TIMEDOUT:
        return EC_NETTIMEOUT;

        /* incompatible versions */
        case RPC_VERSMISMATCH:
        return EC_RPCVERSION;
        case RPC_PROGVERSMISMATCH:
        return EC_SRVRVERSION;

        /* authentication error (not used) */
        case RPC_AUTHERROR:
        return EC_RPCNOPRIV;

        /* server not available */
        case RPC_PROGUNAVAIL:
        return EC_NOSERVER;

        /* procedure not available */
        case RPC_PROCUNAVAIL:
        return EC_SRVRNOPROC;

        /* RPC create (local) errors */
        case RPC_UNKNOWNHOST:
        return EC_UNKNOWNHOST;
        case RPC_UNKNOWNPROTO:
        return EC_UNKNOWNTRANSPORT;

        /* RPC create (remote) errors */
        case RPC_PMAPFAILURE:
        return EC_RPCPMAPERROR;
        case RPC_PROGNOTREGISTERED:
        return EC_UNKNOWNSRVR;

        /* generic errors */
        case RPC_SYSTEMERROR:
        return EC_SRVRERROR;
        case RPC_FAILED:
        default:
        return EC_RPCERROR;
    }
}
