#ifndef __BLOCK_H__
#define __BLOCK_H__

/*
* DESCRIPTION
*      Theses functions provide access to VME bus hardware via
*      block transfer (BLT), sometimes called block mode access
*      (BMA).
*
*      The constructor *newLBma()* reserves a buffer of _size_
*      bytes length in the virtual address space.  This buffer
*      can be used as source or target for data transferred during
*      BLT transfers. Data in this buffer can be accessed r/w via
*      pointers obtained through the *LBma_getPtr?()* functions.
*      The buffer is released by calling the destructor
*      *delLBma()*.
*
*      Before initiating the block transfer, a "chain" of transfer
*      requests is assembled.  After clearing the chain by calling
*      bmaResetChain(), an arbitrary sequence of read and write
*      requests can be added through the bmaWriteChain()
*      and bmaReadChain() functions. For each transfer request,
*      the start address (vmeAddr) and the address modifier
*      (addrMod) on the VME bus together with the length of the
*      transfer in bytes (size) is passed.  The virtOff
*      gives the address offset inside the buffer that was created
*      during construction.
*
*      bmaStartChain() then starts to perform all transfer
*      requests in the chain.  On systems supporting it, the whole
*      chain will be processed by one system call and the transfer
*      will run asyncronously.  So bmaStartChain() may return
*      before the transfer is actually completed.  To wait for
*      the completion of the BLT, call bmaWaitChain().
*      
* NOTES
*      All addresses, offsets and sizes count in bytes.
*
* BUGS
*      The maximum length of the transfer chain is calculated
*      automatically based on the _size_ parameter of *conLBma()*.
*      When queueing many little requests, the chain elements
*      may be exhausted prematurely.
*
* SEE ALSO
*      lvme.h, lint.h
*      
*****************************************************************/

#include <stddef.h>

struct s_bma {
	unsigned char * virtBase;
	void * specific;
};

struct s_bma * bmaAlloc(size_t size);

int bmaResetChain(struct s_bma *bma);
int bmaWriteChain(struct s_bma *bma, int virtOff, unsigned long vmeAddr, unsigned long  size, unsigned char addrMod);
int bmaReadChain(struct s_bma *bma, int virtOff,  unsigned long vmeAddr, unsigned long  size, unsigned char addrMod);

int bmaStartChain(struct s_bma *bma);
int bmaWaitChain(struct s_bma *bma);

#endif
