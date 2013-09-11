TIME = time

# Channel Access test
LOADCALIBES = -L../ca/client -L$(EPICS_BASE)/lib/$(HOST_ARCH) -lcaParam -lca -lCom
# File test
LOADFILELIBES = -L../file -lfileParam
# Oracle test
#ORACLE_HOME = /usr/local/oracle/product/8.0.5
LOADORALIBES = -L../ora -L$(ORACLE_HOME)/lib \
  -loraParam -lsql8 -lclntst8 -lcommon8 -lcore8 -lnls8 -lpthread -ldl -lm
# PostgreSQL test
LOADPSQLLIBES = -L../psql -lpsqlParam -lpq -lcrypt
# Tcl test
LOADTCLLIBES = -L../tcl -ltclParam -ltcl -lm -ldl

HEADERS = blob/*.h filename/*.h int/*.h string/*.h

VPATH=.:blob:filename:int:string

CFLAGS = -Wall -I../include
OBJS = suite.o blobobjs filenameobjs intobjs stringobjs

LIBS = ca file ora psql tcl

ca_test file_test ora_test psql_test tcl_test : links
	$(RM) $@_suite
	$(MAKE) $@_suite
	bin/pre_$@.sh
	$(TIME) ./$@_suite > $@_protocol.txt 2>&1
	bin/post_$@.sh

ca_test_suite : $(OBJS)
	$(CC) $(LDFLAGS) suite.o blob/*.o filename/*.o int/*.o string/*.o \
	  $(LOADCALIBES) -o $@
file_test_suite : $(OBJS)
	$(CC) $(LDFLAGS) suite.o blob/*.o filename/*.o int/*.o string/*.o \
	  $(LOADFILELIBES) -o $@
ora_test_suite : $(OBJS)
	$(CC) $(LDFLAGS) suite.o blob/*.o filename/*.o int/*.o string/*.o \
	  $(LOADORALIBES) -o $@
psql_test_suite : $(OBJS)
	$(CC) $(LDFLAGS) suite.o blob/*.o filename/*.o int/*.o string/*.o \
	  $(LOADPSQLLIBES) -o $@
tcl_test_suite : $(OBJS)
	$(CC) $(LDFLAGS) suite.o blob/*.o filename/*.o int/*.o string/*.o \
	  $(LOADTCLLIBES) -o $@

suite.o : suite.c $(HEADERS)
suite.c :
	bin/main.sh

blobobjs :
	cd blob ; $(MAKE) "CFLAGS=-Wall -I../../include"
filenameobjs :
	cd filename ; $(MAKE) "CFLAGS=-Wall -I../../include"
intobjs :
	cd int ; $(MAKE) "CFLAGS=-Wall -I../../include"
stringobjs :
	cd string ; $(MAKE) "CFLAGS=-Wall -I../../include"

links :
	bin/make_links.sh

test_clean :
	rm -f *test_protocol.txt sqlnet.log *_s.tcl storage.tcl test?.blob core

clean :
	cd blob ; $(MAKE) $@
	cd filename ; $(MAKE) $@
	cd int ; $(MAKE) $@
	cd string ; $(MAKE) $@
	$(MAKE) test_clean
	rm -f suite.c suite.o

bin_clean : clean
	rm -f *_suite
	bin/remove_links.sh

