#___________________________________________________________________[MAKEFILE]
#/////////////////////////////////////////////////////////////////////////////
# Makefile:        ExaReadout.mk
# Purpose:         Build MBS readout task for PPC
# Targets:         exa   -- build task m_read_meb
#                  setup  -- copy setup files to user's directory
#                  clean  -- clean-up
# Author:          MBS and ROOT Based Online Offline Utility
# Revision:         
# Date:            
# URL:             
#/////////////////////////////////////////////////////////////////////////////

LEGALCPU	= PPC

RFILE	=	ExaReadout

SRC	= $(MBSROOT)/src
INC	= $(MBSROOT)/inc
OBJ	= $(MBSROOT)/obj_$(GSI_LYNX_PROC_FAM)
LIB	= $(MBSROOT)/lib_$(GSI_LYNX_PROC_FAM)

GD_INCL	= /nfs/mbssys/include

C_CTRL	= CBV

CC	= gcc

POSIX   = -mposix4d9 -mthreads
CFLAGS  = $(POSIX) -DLynx -D$(C_CTRL) -DMBS -DPPC -I. -I$(INC) -I$(GD_INCL) 
LIBS    =	/lib/ces/libvme.a $(LIB)/lib_mbs.a \
			$(LIB)/lib_tools.a $(LIB)/lib_mbs.a \
			$(LIB)/lib_utils.a \
			$(LIB)/libedit.a -lnetinet

all:	exa

exa: if$(LEGALCPU) $(OBJ)/m_read_meb.o \
		$(OBJ)/objects \
		$(RFILE).c
	@echo
	@echo "[Making User Readout m_read_meb from $(RFILE).c]"
	@echo
	rm -f $(OBJ)/m_read_meb.o
	$(CC) $(CFLAGS) -o m_read_meb \
		$(RFILE).c \
		$(OBJ)/m_read_meb.o \
		$(LIBS)
	strip m_read_meb
	rm -f *.unstripped
	chmod og-w m_read_meb
	@echo
	@echo "[Making User Readout m_read_meb_debug (DEBUG version) from $(RFILE).c]"
	@echo
	rm -f $(OBJ)/m_read_meb.o
	$(CC) $(CFLAGS) -DDEBUG=1 -o m_read_meb_debug \
		$(RFILE).c \
		$(OBJ)/m_read_meb.o \
		$(LIBS)
	strip m_read_meb_debug
	rm -f *.unstripped
	chmod og-w m_read_meb_debug

clean:
	rm -f core *.bak *.jou *.o m_read_meb

if$(LEGALCPU):
	@if [ $(MACHINE) != $(LEGALCPU) ];\
	then\
		echo "make: legal on $(LEGALCPU) cpus only - can't be executed on `hostname`";\
		exit 1;\
	fi
