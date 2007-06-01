CC = gcc 
CFLAGS =  -pipe -g -O3 -ansi -DLOG_PERROR_DOESNT_WORK

CPPFLAGS =

INCDIR = -I../inc -I$(MBSROOT)/inc -I$(MARABOU_PATH)/powerpc/include 

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../lib/$(MBS_VERSION)/libsis_3300.a

libsis_3300.OBJS = sis_3300.o sis_3300_readout.o block.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) -o $@ 

.SUFFIXES: .o .c 

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS)

$(LIB_TARGETS): $(libsis_3300.OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

lib: $(LIB_TARGETS)


clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

sis_3300.o: sis_3300.c ../inc/sis_3300.h ../inc/sis_3300_protos.h
