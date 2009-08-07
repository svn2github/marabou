CC	 		=	gcc
CFLAGS		=	-w -g -c -O -I../inc

LD			=	gcc
LFLAGS		=	-g

all:		mbs2asc

mbs2asc:	mbs2asc.o mbsio.o byte_order.o root_env.o
			$(CC) -g -o mbs2asc mbs2asc.o mbsio.o byte_order.o root_env.o
			cp mbs2asc ../../bin

clean:
			rm -f mbs2asc *.o
.c.o:
			$(CC) $(CFLAGS) -o $@ $<
