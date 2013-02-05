/*
***************************************************************
*
* xvmelib: VME mapping for XPC DMA
*
***************************************************************
*                                                             
*      Copyright 2002, CES Creative Electronic System SA      
*        70, route du Pont-Butin, CH-1213 Petit-Lancy         
*                   All Rights Reserved                       
*                                                             
***************************************************************
*
* $Log: xvmelib.c,v $
* Revision 1.2  2002/04/29 06:17:46  martin
*  Chain support for RIO3 added.
*
* Revision 1.1  2002/02/01 13:14:33  martin
* XPC/VME mapping added.
*
*
***************************************************************
*/
#include <sys/types.h>
#include <ces/absolute.h>
#include <ces/vmelib.h>
/*
*========================================================
* globals
*--------------------------------------------------------
*/
static int is_rio3 = -1;
static struct pdparam_master param = {
	1,	/* no vme iack 			*/
	0,	/* No VME read prefetch 	*/
	0,	/* No VME write posting 	*/
	1,	/* autoswap 			*/
	0,	/* page descriptor number 	*/
	0,	/* shared			*/
	0,	/* ADP type			*/
	0	/* dummy			*/
};
/*
*========================================================
* check board_type();
*--------------------------------------------------------
*/
int
xvme_is_rio3( void ) 
{
  int board_type;

  if (is_rio3 == -1) {
    board_type = uio_board_type();
    is_rio3 = CESRIO3(board_type)?1:0;
  }
  return(is_rio3);
}
/*
*========================================================
* map page
*--------------------------------------------------------
*/
u_int
xvme_map(u_int vaddr, int wsiz, int am, int adp)
{
  u_int ad;

  if (xvme_is_rio3() == 0) return(vaddr);
  if (adp == -1) return(vaddr);
  if (adp == 1) {
    param.dum[1] = 0xa8;
  } else {
    param.dum[1] = adp;
  }
  return (vme_dma_map(vaddr,wsiz,am,&param));
}
/*
*========================================================
* release page
*--------------------------------------------------------
*/
int
xvme_rel(u_int laddr, int wsiz)
{
  if (xvme_is_rio3() == 0) return 0;
  return(vme_dma_unmap(laddr));
}
