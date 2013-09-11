#ifndef XVMELIB_H
#define XVMELIB_H 

int xvme_is_rio2( void );

int xvme_is_rio3( void );

int xvme_is_rio4( void ); 

u_int xvme_map(u_int vaddr, int wsiz, int am, int adp);

int xvme_rel(u_int laddr, int wsiz);

#endif
