#___________________________________________________________________[MAKEFILE]
#/////////////////////////////////////////////////////////////////////////////
# @(#)%P%
# @(#)Purpose:     Build utility lib to run with MBS
# @(#)Author:      R. Lutter
# @(#)Revision:    SCCS: %W%
# @(#)Date:        %G%
# URL:             
#/////////////////////////////////////////////////////////////////////////////

LEGALOS 	=	LynxOS

CC			=	gcc

HDRS		=

SRCS		=	\
				root_env.c \
				unix_string.c \
				dgf_download.c \
				dgfdown.c \
				cc32_init.c \
				cc32.c

ULIB 		=	lib_utils.a
INC 		=	$(MBSROOT)/inc
GD_INCL		=	/mbsusr/marabou/include
CFLAGS		=	$(POSIX) -DLynx -O2 -I. -I$(INC) -I$(GD_INCL) -DMBS -DPPC

OBJS		=	\
				$(ULIB)(root_env.o) \
				$(ULIB)(unix_string.o) \
				$(ULIB)(dgf_download.o) \
				$(ULIB)(cc32_init.o)

POSIX		=	-mposix4d9 -mthreads
CFLAGS		=	$(POSIX) -DLynx -O2 -I. -I$(INC) -I$(GD_INCL) -DMBS -DPPC

OBJDIR		=	$(MBSROOT)/obj_$(GSI_LYNX_PROC_FAM)
LIBDIR		=	$(MBSROOT)/lib_$(GSI_LYNX_PROC_FAM)
LIBS		=	/lib/ces/libvme.a $(LIBDIR)/lib_mbs.a \
				$(LIBDIR)/lib_tools.a $(LIBDIR)/lib_mbs.a \
				$(LIBDIR)/libedit.a -lnetinet

all:		if$(LEGALOS) $(OBJS) envtst uxtst dgfdown cc32

envtst: 	if$(LEGALOS) envtst.o $(ULIB)
			$(CC) $(CFLAGS) -o envtst envtst.c $(ULIB)
			
uxtst:  	if$(LEGALOS) uxtst.o $(ULIB)
			$(CC) $(CFLAGS) -o uxtst uxtst.c $(ULIB)

cc32: 	  	if$(LEGALOS) cc32.o $(ULIB)
			$(CC) $(CFLAGS) -o cc32 cc32.c $(ULIB) $(LIBS)
			
dgfdown:	dgfdown.o $(ULIB)
			$(CC) $(CFLAGS) -o dgfdown dgfdown.c $(ULIB) $(LIBS)

if$(LEGALOS):
	@if [ $(LEGALOS) != `uname` ];\
    then\
        echo "make: legal under $(LEGALOS) only - can't be executed on `hostname`";\
        exit 1;\
    fi
