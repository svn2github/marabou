CC	 		=	gcc
CFLAGS		=	-w -g -c -O -I../inc

LD			=	gcc
LFLAGS		=	-g

all:		rmbs

rmbs:		rmbs.o mbsio.o byte_order.o
			$(CC) -o rmbs rmbs.o mbsio.o byte_order.o

.c.o:
			$(CC) $(CFLAGS) -o $@ $<
