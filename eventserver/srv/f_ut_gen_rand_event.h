#ifndef __GEN_RAND_EVE
#define __GEN_RAND_EVE 1

#include "f_ut_ini_rand_event.h"

#include "random-coll.h"

REAL8 gauss_rnd(double mean, double sigma);
REAL8 get_int(double low, double high);
INTS4 rand_event(long choice);

INTS4 f_ut_gen_evt(INTS4 *ps_buf, INTS4 l_blen, INTS4 l_subevts, INTS4 l_max, INTS4 l_mode);

#define NUM_PEAK 5

INTS2 debug=0;

#endif