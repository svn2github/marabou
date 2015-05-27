# Module.mk for gredit module
#
# Author: O.Schaile,


MODDIR       := gredit
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

GREDITDIR      := $(MODDIR)
GREDITDIRS     := $(GREDITDIR)/src
GREDITDIRI     := $(GREDITDIR)/inc

##### lib #####
GREDITL        := $(MODDIRI)/LinkDef.h
GREDITDS       := $(MODDIRS)/G__GrEditDict.cxx
GREDITPCM       := $(MODDIRS)/G__GrEditDict_rdict.pcm
GREDITDO       := $(GREDITDS:.cxx=.o)
GREDITH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
GREDITS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GREDITO        := $(GREDITS:.cxx=.o)

GREDITDEP      := $(GREDITO:.o=.d) $(GREDITDO:.o=.d)

GREDITLIB      := $(LPATH)/libGrEdit.$(SOEXT)
GREDITRMAP     := $(LPATH)/libGrEdit.rootmap
CDLMAINO    := $(MODDIRS)/main.o
CDLO        := $(filter-out $(CDLMAINO),$(GREDITO))

CDLEXE      := bin/CreateCDlabel
ALLEXECS    += $(CDLEXE)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(GREDITH))
ALLLIBS     += $(GREDITLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(GREDITPCM)
endif

# include all dependency files
INCLUDEFILES += $(GREDITDEP)

MRBUTILSLIB   := $(LPATH)/libTMrbUtils.$(SOEXT)
MRBGUTILSLIB  := $(LPATH)/libTGMrbUtils.$(SOEXT)

OCDLLIBS      := $(GREDITLIB) $(MRBGUTILSLIB) $(MRBUTILSLIB)
GREDITLIBDEP   := $(ROOTSYS)/lib/libGraf.so  $(MRBGUTILSLIB) $(MRBUTILSLIB)

##### local rules #####

include/%.h:    $(GREDITDIRI)/%.h
		cp $< $@

$(CDLEXE):      $(CDLMAINO) $(CDLLIB) $(OCDLLIBS)
		@echo "other libs: $(OCDLLIBS)"
		@echo "$(CDLEXE) linking exe ----------------------------------"
		$(LD) -g -o $(CDLEXE) $(LDFLAGS) $(CDLMAINO) $(OCDLLIBS) $(ROOTGLIBS)

$(GREDITLIB):     $(GREDITDO) $(GREDITO) $(MAINLIBS) $(GREDITLIBDEP)
			@echo "Making libGrEdit $@..."
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
			"$(SOFLAGS)" libGrEdit.$(SOEXT) $@ "$(GREDITDO) $(GREDITO) $(GREDITLIBEXTRA)"
		@(if [ -f $(GREDITPCM) ] ; then \
			echo "cp  $(GREDITPCM)----------------------" ; \
			cp $(GREDITPCM) $(LPATH); \
		fi)
		@$(RLIBMAP) -o $(GREDITRMAP) -l $(GREDITLIB) -d $(GREDITLIBDEP) -c $(GREDITL)

$(GREDITDS):     $(GREDITH) $(GREDITL)
		@echo "Generating dictionary: $@...."
		$(ROOTCINT) -f $@ -c -p -Iinclude defineMarabou.h $(GREDITH) $(GREDITL)

$(GREDITDO):     $(GREDITDS)
		@echo "Compiling dictionary $@..."
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-gredit:      $(GREDITLIB)

clean-gredit:
		@rm -f $(GREDITO) $(GREDITDO) $(GREDITDS)

clean::         clean-gredit

distclean-gredit: clean-gredit
		@rm -f $(GREDITDEP) $(GREDITDS) $(GREDITDH) $(GREDITLIB)

distclean::     distclean-gredit
