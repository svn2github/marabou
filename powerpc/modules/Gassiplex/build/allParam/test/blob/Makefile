all :
	$(MAKE) depend "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(MAKE) -f Makefile.dep "CFLAGS=$(CFLAGS)"

depend :
	rm -f Makefile.dep
	../bin/echoobjs.sh > Makefile.dep
	$(CC) -MM $(CFLAGS) *.c >> Makefile.dep

clean :
	rm -f Makefile.dep *.o

