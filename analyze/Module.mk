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
MRBAL      := $(MODDIRI)/LinkDef.h
MRBADS      := $(MODDIRS)/G__$(MRBA)Dict.cxx
MRBAPCM      := $(MODDIRS)/G__$(MRBA)Dict_rdict.pcm
MRBADO      := $(MRBADS:.cxx=.o)

MRBAS       := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
MRBAHL      := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
MRBAH       := $(patsubst $(MODDIRI)/%.h,%.h,$(MRBAHL))
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
MRBALIBDEP  := $(LPATH)/libTMrbUtils.$(SOEXT)
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MRBAHL))
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

# libTMrbAnalyze cannot be linked with: -Wl,--no-undefined
# because of extra user code

$(MRBALIB):     $(MRBADO) $(MRBAO) $(MRBALIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "-shared -Wl,-soname," lib$(MRBA).$(SOEXT) $@ "$(MRBAO) $(MRBADO) $(MRBANALYSELIBEXTRA)"
		@(if [ -f $(MRBAPCM) ] ; then \
			echo "cp $(MRBAPCM)----------------------" ; \
			cp $(MRBAPCM) $(LPATH); \
		fi)

$(MUTEXLIB):     $(MUTEXO)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libMutex.$(SOEXT) $@ "$(MUTEXO)"

$(MRBADS):     $(MRBAHL) $(MRBAL)
		@echo "Generating dictionary $@..."
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include $(MRBAH) $(MRBAL)
else
		rootcling -f $@ $(call dictModule,MRBA) -I$(MARABOU_SRCDIR)/include $(MRBAH) $(MRBAL)
endif
#		$(ROOTCINT) -f $@ -c -p -Iinclude $(MRBAH) $(MRBADL)

$(MRBADO):     $(MRBADS)
		$(CXX) -g $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

$(ANDO):     $(ANDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-analyze:       $(ANOBJ) $(MRBALIB)

clean-analyze:
		@rm -f $(ANO) $(MRBAO) $(MRBADO) $(MUTEXO) $(MRBADS) $(ANOBJ)

clean::         clean-analyze

distclean-analyze: clean-analyze
		@rm -f $(ANDEP) $(ANDS) $(ANDH) $(ANEXE) $(MUTEXLIB) $(MRBALIB)

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
