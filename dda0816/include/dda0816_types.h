typedef struct dda0816_int_par {
int param;
int error;
unsigned long retval;
} dda0816_int_par_t;

typedef struct dda0816_no_par {
int param;
int error;
unsigned long retval;
} dda0816_no_par_t;

typedef struct SetCurve_par {
struct dda0816_curve curve;
int error;
unsigned long retval;

} SetCurve_par_t ;



