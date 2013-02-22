/*
***************************************************************
*
* xvmelib: VME mapping for XPC DMA
*
***************************************************************
*                                                             
*      Copyright 2002, CES Creative Electronic System SA      
*                   All Rights Reserved                       
*                                                             
***************************************************************
*
* $Log: xvmelib.c,v $
* Revision 1.3  2006/08/24 13:15:43  akos
* RIO4 support
*
* Revision 1.2  2004/04/29 07:18:06  akos
* Removed old CES address
*
* Revision 1.1.1.1  2003/04/14 13:40:50  akos
* lynx4.0 project creation. Akos Csilling 14-Apr-2003
*
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
static int is_rio2 = -1;
static int is_rio3 = -1;
static int is_rio4 = -1;
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
xvme_is_rio2( void ) 
{
  int board_type;

  if (is_rio2 == -1) {
    board_type = uio_board_type();
    is_rio2 = CESRIO2(board_type)?1:0;
  }
  return(is_rio2);
}
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
int
xvme_is_rio4( void ) 
{
  int board_type;

  if (is_rio4 == -1) {
    board_type = uio_board_type();
    is_rio4 = CESRIO4(board_type)?1:0;
  }
  return(is_rio4);
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

  if (xvme_is_rio2()) return(vaddr);
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
  if (xvme_is_rio2()) return 0;
  return(vme_dma_unmap(laddr));
}
