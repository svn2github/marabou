# Module.mk for mbssetup module
#
# Author: O.Schaile 24/11/2000

MODDIR       := mbssetup
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

MBSSETUPDIR      := $(MODDIR)
MBSSETUPDIRS     := $(MBSSETUPDIR)/src
MBSSETUPDIRI     := $(MBSSETUPDIR)/inc

##### lib #####
MBSSETUPL        := $(MODDIRI)/LinkDef.h
MBSSETUPDS       := $(MODDIRS)/G__TMbsSetupDict.cxx
MBSSETUPPCM       := $(MODDIRS)/G__TMbsSetupDict_rdict.pcm
MBSSETUPDO       := $(MBSSETUPDS:.cxx=.o)
MBSSETUPH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
MBSSETUPS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
MBSSETUPO        := $(MBSSETUPS:.cxx=.o)

MBSSETUPDEP      := $(MBSSETUPO:.o=.d) $(MBSSETUPDO:.o=.d)

MBSSETUPLIB      := $(LPATH)/libTMbsSetup.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MBSSETUPH))
ALLLIBS     += $(MBSSETUPLIB)

# include all dependency files
INCLUDEFILES += $(MBSSETUPDEP)

##### local rules #####

include/%.h:    $(MBSSETUPDIRI)/%.h
		cp $< $@

$(MBSSETUPLIB):     $(MBSSETUPDO) $(MBSSETUPO) $(MAINLIBS) $(MBSSETUPLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMbsSetup.$(SOEXT) $@ "$(MBSSETUPO) $(MBSSETUPDO)" \
		   "$(MBSSETUPLIBEXTRA)"
		@(if [ -f $(MBSSETUPDIRS)/$(MBSSETUPPCM) ] ; then \
			echo "cp  $(MBSSETUPDIRS)/$(MBSSETUPPCM)----------------------" ; \
			cp $(MBSSETUPDIRS)/$(MBSSETUPPCM) $(LPATH); \
		fi)

$(MBSSETUPDS):     $(MBSSETUPH) $(MBSSETUPL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(MBSSETUPH) $(MBSSETUPL)

$(MBSSETUPDO):     $(MBSSETUPDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-mbssetup:       $(MBSSETUPLIB)

clean-mbssetup:
		@rm -f $(MBSSETUPO) $(MBSSETUPDO) $(MBSSETUPDS)

clean::         clean-mbssetup

distclean-mbssetup: clean-mbssetup
		@rm -f $(MBSSETUPDEP) $(MBSSETUPDS) $(MBSSETUPDH) $(MBSSETUPLIB)

distclean::     distclean-mbssetup
