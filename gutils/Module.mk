# Module.mk for gutils module
#
# Author: O.Schaile, R.Lutter 20/11/2000


MODDIR       := gutils
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

GUTILSDIR      := $(MODDIR)
GUTILSDIRS     := $(GUTILSDIR)/src
GUTILSDIRI     := $(GUTILSDIR)/inc

##### lib #####
GUTILSL        := $(MODDIRI)/LinkDef.h
GUTILSDS       := $(MODDIRS)/G__TGMrbUtilsDict.cxx
GUTILSPCM       := $(MODDIRS)/G__TGMrbUtilsDict_rdict.pcm
GUTILSDO       := $(GUTILSDS:.cxx=.o)
GUTILSHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
## $(info GUTILSHL = $(GUTILSHL))
GUTILSH        := $(patsubst $(MODDIRI)/%.h,%.h,$(GUTILSHL))
## $(info GUTILSH = $(GUTILSH))
GUTILSS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GUTILSO        := $(GUTILSS:.cxx=.o)

GUTILSDEP      := $(GUTILSO:.o=.d) $(GUTILSDO:.o=.d)
HELPBLIB      := $(LPATH)/libTMrbHelpBrowser.$(SOEXT)
GUTILSLIB      := $(LPATH)/libTGMrbUtils.$(SOEXT)
GUTILSRMAP     := $(LPATH)/libTGMrbUtils.rootmap
GUTILSLIBDEP   := $(HELPBLIB) $(LPATH)/libTMrbUtils.so $(ROOTSYS)/lib/libGraf.so $(ROOTSYS)/lib/libGui.so $(ROOTSYS)/lib/libGed.so


# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(GUTILSHL))
ALLLIBS     += $(GUTILSLIB)
# $(info gutils/Module.mk: ROOTVERS = $(ROOTVERS))
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(GUTILSPCM)
endif

# include all dependency files
INCLUDEFILES += $(GUTILSDEP)

##### local rules #####

include/%.h:    $(GUTILSDIRI)/%.h
		cp $< $@

$(GUTILSLIB):     $(GUTILSDO) $(GUTILSO) $(GUTILSLIBDEP)
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTGMrbUtils.$(SOEXT) $@ "$(GUTILSDO) $(GUTILSO) $(HELPBLIB)" \
		   "$(GUTILSLIBEXTRA)"
		@(if [ -f $(GUTILSPCM) ] ; then \
			echo "cp  $(GUTILSPCM)----------------------" ; \
			cp $(GUTILSPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
			@$(RLIBMAP) -o $(GUTILSRMAP) -l $(GUTILSLIB) -d $(GUTILSLIBDEP) -c $(GUTILSL)
endif

$(GUTILSDS):     $(GUTILSHL) $(GUTILSL)
		@echo "Generating dictionary $@... for ROOT $(ROOT_MAJOR)"
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include defineMarabou.h $(GUTILSH) $(GUTILSL)
else
		rootcling -f $@ $(call dictModule,GUTILS) -I$(MARABOU_SRCDIR)/include $(GUTILSH) $(GUTILSL)
endif

$(GUTILSDO):     $(GUTILSDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-gutils:       $(GUTILSLIB)

clean-gutils:
		@rm -f $(GUTILSO) $(GUTILSDO) $(GUTILSDS)

clean::         clean-gutils

distclean-gutils: clean-gutils
		@rm -f $(GUTILSDEP) $(GUTILSDS) $(GUTILSDH) $(GUTILSLIB)

distclean::     distclean-gutils
