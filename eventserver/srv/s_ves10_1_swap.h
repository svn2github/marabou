/***********************************************************************/
/* File S_VES10_1_SWAP.H  */
/* 29.3.95, Horst Goeringer           */
/*  ================= GSI VME Subevent header =======================  */
typedef struct 
{
long  l_dlen;   /*  Data length +2 in words */
short i_subtype; 
short i_type; 
char  h_control;   /*  Processor type code */
char  h_subcrate;   /*  Subcrate number */
short i_procid;   /*  Processor ID [as loaded from VAX] */
} s_ves10_1 ;
  /* ------------------------------------------------------------------ */
