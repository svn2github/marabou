/*******************************************************************/
/* File S_VE10_1_SWAP.H  */
/* 29.3.95, Horst Goeringer           */
/*  ================= GSI VME Event header =======================  */
typedef struct 
{
long  l_dlen;   /*  Data length + 4 in words */
short i_subtype; 
short i_type; 
short i_trigger;   /*  Trigger number */
short i_dummy;   /*  Not used yet */
long  l_count;   /*  Current event number */
} s_ve10_1 ;
  /* ------------------------------------------------------------------ */
