/****************************************************************/
/* File S_VES10_1.H  */
/* 29.3.95, Horst Goeringer           */
/*  ================= GSI VME Subevent header =======================  */
typedef struct 
{
long  l_dlen;      /*  Data length +2 in words */
short i_type; 
short i_subtype; 
short i_procid;    /*  Processor ID [as loaded from VAX] */
char  h_subcrate;  /*  Subcrate number */
char  h_control;   /*  Processor type code */
} s_ves10_1 ;
  /* ------------------------------------------------------------------ */
