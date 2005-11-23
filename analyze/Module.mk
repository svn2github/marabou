# Module.mk for analyze module
#
# Author: O.Schaile 24/11/2000

MODDIR       := analyze
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

ANDIR      := $(MODDIR)
ANDIRS     := $(ANDIR)/src
ANDIRI     := $(ANDIR)/inc

AN         := M_analyze
MRBA       := TMrbAnalyze
MUTEX      := mutex

##### lib #####
MRBADL      := $(MODDIRI)/LinkDef.h
MRBADS      := $(MODDIRS)/G__$(MRBA)Dict.cxx
MRBADO      := $(MRBADS:.cxx=.o)

MRBAS       := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
MRBAH       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
MRBAO       := $(MRBAS:.cxx=.o)

MUTEXS      := $(MODDIRS)/$(MUTEX).cxx
MUTEXO      := $(MUTEXS:.cxx=.o)

ANS         := $(MODDIRS)/$(AN).cxx
ANO         := $(ANS:.cxx=.o)

ANDEP       := $(ANO:.o=.d)
MRBADEP     := $(MRBAO:.o=.d) $(MRBADO:.o=.d)

MRBALIB     := $(LPATH)/lib$(MRBA).$(SOEXT)
MUTEXLIB    := $(LPATH)/libMutex.$(SOEXT)
ANOBJ       := obj/$(AN).o

ALLEXECS    += $(ANEXE)
ALLLIBS     += $(MRBALIB)
ALLLIBS     += $(MUTEXLIB)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MRBAH))
ALLOBJS     += obj/$(AN).o

# include all dependency files
INCLUDEFILES += $(ANDEP)
INCLUDEFILES += $(MRBADEP)

##### extra libs needed #####
##### local rules #####

include/%.h:    $(ANDIRI)/%.h
		cp $< $@

obj/%.o:    $(MODDIRS)/%.o
		cp $< $@


$(MRBALIB):     $(MRBADO) $(MRBAO) $(MAINLIBS) $(MRBALIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" lib$(MRBA).$(SOEXT) $@ "$(MRBAO) $(MRBADO)"
$(MUTEXLIB):     $(MUTEXO)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libMutex.$(SOEXT) $@ "$(MUTEXO)"

$(MRBADS):     $(MRBAH) $(MRBADL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(MRBAH) $(MRBADL)

$(MRBADO):     $(MRBADS)
		$(CXX) -g $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

$(ANDO):     $(ANDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-analyze:       $(ANOBJ) $(MRBALIB)

clean-analyze:
		@rm -f $(ANO) $(MRBAO) $(MRBADO) $(MUTEXO) $(MRBADS) $(ANOBJ) $(MRBALIB)

clean::         clean-analyze

distclean-analyze: clean-analyze
		@rm -f $(ANDEP) $(ANDS) $(ANDH) $(ANEXE)

distclean::     distclean-analyze

install-analyze:	
	@(if [ -d $(BINDIR) ]; then \
	   inode1=`ls -id $(BINDIR) | awk '{ print $$1 }'`; \
	fi; \
	inode2=`ls -id $$PWD/bin | awk '{ print $$1 }'`; \
	if [ -d $(BINDIR) ] && [ $$inode1 -eq $$inode2 ]; then \
		echo "Everything already installed..."; \
	else \
		echo "Installing $(ANOBJ) in $(OBJDIR)"; \
		$(INSTALLDIR) $(OBJDIR); \
		$(INSTALL) $(ANOBJ) $(OBJDIR); \
		echo "Installing $(MRBALIB) in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) $(MRBALIB) $(LIBDIR); \
		echo "Installing $(MUTEXLIB) in $(LIBDIR)"; \
		$(INSTALL) $(MUTEXLIB) $(LIBDIR); \
	fi)
