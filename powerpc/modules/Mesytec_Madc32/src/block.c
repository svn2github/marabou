#include <stddef.h>

#include <assert.h>
#include <file.h>
#include <ioctl.h>
#ifdef PTHREAD
#include <pthread.h>
#endif
#include <sys/types.h>
#include <errno.h>

#include <stdio.h>

#include <ces/vmelib.h>
#include <ces/vmecmd.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#ifndef HAVE_VME_DMA_MAP
static unsigned int vme_dma_map(unsigned int vaddr, int wsiz, int am, struct pdparam_master *param)
{
	return vaddr;
}
#endif


#ifndef HAVE_BMA_MEM2LOC
static unsigned long bma_mem2loc(unsigned long paddr)
{
#ifdef CPU_TYPE_RIO2
	unsigned long p = paddr | 0x80000000;
	printf("@@@ RIO2 paddr=%#x\n", p); getchar();
	return p;
#else
	printf("@@@ NOT RIO2 paddr=%#x\n", paddr); getchar();
	return paddr;
#endif
}
#endif

struct s_specific {
	uio_mem_t *bufDsc;
	BmaChain *chain;
	BmaChain *c0;
	BmaChain *c1;
	int nElems;
	int elem;
};

static void bmaLogErr(const char * caller, const char *msg, int err)
{
	static const char *errMsg[] = {
		"%s: BMA(%d): SUCCESS",
		"%s: BMA(%d): Unspecific error",
		"%s: BMA(%d): Not (yet) implemented",
		"%s: BMA(%d): no 'User I/O' driver",
		"%s: BMA(%d): Cannot allocate memory",
		"%s: BMA(%d): Cannot allocate mode structure",
		"%s: BMA(%d): Illegal mode ID",
		"%s: BMA(%d): Unsupported mode parameter",
		"%s: BMA(%d): Illegal mode parameter",
		"%s: BMA(%d): Illegal mode parameter value",
		"%s: BMA(%d): PCI Target Abort",
		"%s: BMA(%d): VME Bus Error",
		"%s: BMA(%d): VME Retry",
		"%s: BMA(%d): VME Bus Timeout",
		"%s: BMA(%d): Wordcount not zero",
		"%s: BMA(%d): Chain Overflow",
		"%s: BMA(%d): No status found in chain",
		"%s: BMA(%d): Invalid (chain) pointer",
		"%s: BMA(%d): Unknow error code"
	};
	const int nErrMsg = sizeof(errMsg) / sizeof(char *);

	if (err >= 0) {
		if (err >= nErrMsg) err = nErrMsg - 1;
		sprintf(msg, errMsg[err], msg, err);
	}
	f_ut_send_msg(caller, msg, ERR__MSG_INFO, MASK__PRTT);
}

struct s_bma * bmaAlloc(size_t size)
{
	struct s_bma * bma;
	struct s_specific * spec;
	struct pdparam_master paramS, *param = &paramS;
	int error;

	bma = (struct s_bma *) calloc(1, sizeof(struct s_bma));
	bma->specific = (struct s_specific *) calloc(1, sizeof(struct s_specific));
	spec = bma->specific;

	error = bma_open();
	if (error) {
		bmaLogErr("bmaAlloc", "bma_open", error);
		return NULL;
	}
	error = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_VMESize, 6);
	if (error) {
		bmaLogErr("bmaAlloc", "bma_set_mode", error);
		bma_close();
		return NULL;
	}
	error = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_WordSize, BMA_M_WzD32);
	if (error) {
		bmaLogErr("bmaAlloc", "bma_set_mode", error);
		bma_close();
		return NULL;
	}

	memset(param, 0, sizeof(*param));
	param->iack = 1;
	param->swap = SINGLE_AUTO_SWAP;

	spec->nElems = size / 1024 * 2;
	error = bma_chain_alloc(&spec->chain, spec->nElems);
	if (error) {
		bmaLogErr("bmaAlloc", "bma_chain_alloc", error);
		bma_close();
		return NULL;
	}
	bmaResetChain(bma);

	spec->bufDsc = (uio_mem_t *) calloc(1, sizeof(uio_mem_t));

	if (0 != uio_calloc(spec->bufDsc, size)) {
		bmaLogErr("bmaAlloc", sys_errlist[errno], -1);
		free(spec->bufDsc);
		bma_close();
		return NULL;
	}
	bma->virtBase = (unsigned char *) spec->bufDsc->uaddr;

	return bma;
}

int bmaWriteChain(struct s_bma * bma, int virtOff, unsigned long vmeAddr, unsigned long size, unsigned char addrMod)
{
	struct s_specific * spec = bma->specific;
	int error;

	error = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_AmCode, addrMod);
	if (error) {
		bmaLogErr("bmaWriteChain", "bma_set_mode", error);
		return -1;
	}
	error = bma_chain_write(&spec->c1, &spec->elem, BMA_DEFAULT_MODE, (unsigned long) vmeAddr, bma_mem2loc(spec->bufDsc->paddr) + virtOff, size, &spec->c0);
	if (error) {
		bmaLogErr("bmaWriteChain", "bma_chain_write", error);
		return -1;
	}
	return 0;
}

int bmaReadChain(struct s_bma * bma, int virtOff, unsigned long vmeAddr, unsigned long size, unsigned char addrMod)
{
	struct s_specific * spec = bma->specific;
	int error;

	printf("@@@ before bma_set_mode "); getchar();
	error = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_AmCode, addrMod);
	if (error) {
		bmaLogErr("bmaReadChain", "bma_set_mode", error);
		return -1;
	}

	printf("@@@ before bma_chain_read "); getchar();
	error = bma_chain_read(&spec->c1, &spec->elem, BMA_DEFAULT_MODE, (unsigned long) vmeAddr, bma_mem2loc(spec->bufDsc->paddr) + virtOff, size, &spec->c0);
	if (error) {
		bmaLogErr("bmaReadChain", "bma_chain_read", error);
		return -1;
	}
	printf("@@@ after bma_chain_read %#lx %#lx %#lx", spec->bufDsc->paddr, bma_mem2loc(spec->bufDsc->paddr), virtOff); getchar();
	return 0;
}

int bmaResetChain(struct s_bma * bma)
{
	struct s_specific * spec = bma->specific;

	spec->c0 = 0;
	spec->c1 = spec->chain;
	spec->elem = spec->nElems;
}

int bmaStartChain(struct s_bma * bma)
{
	struct s_specific * spec = bma->specific;
	int error;

	error = bma_chain_start(spec->chain, BMA_DEFAULT_MODE);
	if (error) {
		bmaLogErr("bmaStartChain", "bma_chain_start", error);
		return -1;
	}
	return 0;
}

int bmaWaitChain(struct s_bma * bma)
{
	struct s_specific * spec = bma->specific;
	int error;

	BmaChain *chain = spec->chain;
	int nElems = spec->nElems;

	error = bma_wait(BMA_DEFAULT_MODE);
	if (error) {
		bmaLogErr("bmaWaitChain", "bma_wait", error);
		return -1;
	}
	return 0;
}
