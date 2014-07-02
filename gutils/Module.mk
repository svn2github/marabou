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
GUTILSH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
GUTILSS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GUTILSO        := $(GUTILSS:.cxx=.o)

GUTILSDEP      := $(GUTILSO:.o=.d) $(GUTILSDO:.o=.d)

GUTILSLIB      := $(LPATH)/libTGMrbUtils.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(GUTILSH))
ALLLIBS     += $(GUTILSLIB)

# include all dependency files
INCLUDEFILES += $(GUTILSDEP)

##### local rules #####

include/%.h:    $(GUTILSDIRI)/%.h
		cp $< $@

$(GUTILSLIB):     $(GUTILSDO) $(GUTILSO) $(MAINLIBS) $(GUTILSLIBDEP)
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTGMrbUtils.$(SOEXT) $@ "$(GUTILSDO) $(GUTILSO)" \
		   "$(GUTILSLIBEXTRA)"
		@(if [ -f $(GUTILSDIRS)/$(GUTILSPCM) ] ; then \
			echo "cp  $(GUTILSDIRS)/$(GUTILSPCM)----------------------" ; \
			cp $(GUTILSDIRS)/$(GUTILSPCM) $(LPATH); \
		fi)

$(GUTILSDS):     $(GUTILSH) $(GUTILSL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(GUTILSH) $(GUTILSL)

$(GUTILSDO):     $(GUTILSDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-gutils:       $(GUTILSLIB)

clean-gutils:
		@rm -f $(GUTILSO) $(GUTILSDO) $(GUTILSDS)

clean::         clean-gutils

distclean-gutils: clean-gutils
		@rm -f $(GUTILSDEP) $(GUTILSDS) $(GUTILSDH) $(GUTILSLIB)

distclean::     distclean-gutils
