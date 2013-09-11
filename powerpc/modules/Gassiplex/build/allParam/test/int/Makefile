all :
	$(MAKE) depend "CFLAGS=$(CFLAGS)"
	$(MAKE) -f Makefile.dep "CFLAGS=$(CFLAGS)"

depend :
	rm -f Makefile.dep
	../bin/echoobjs.sh > Makefile.dep
	gcc -MM $(CFLAGS) *.c >> Makefile.dep

clean :
	rm -f Makefile.dep *.o

