#ifndef __INI_RAND_EVE
#define __INI_RAND_EVE 1

#include <time.h> /* for time structure */
#include <sys/time.h> /* for gettimeofday */
#include <unistd.h> /* for gettimeofday */

INTU4 seed;

INTS4 get_rand_seed();                         /* obtains seed for generator      */

INTS4 f_ut_ini_evt();

INTS4 l_subevts;
INTS4 l_max;
INTS4 l_mode;
INTS4 l_func; /* not yet used */

#endif
