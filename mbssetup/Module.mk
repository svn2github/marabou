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
MBSSETUPHL        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
MBSSETUPH        := $(patsubst $(MODDIRI)/%.h,%.h,$(MBSSETUPHL))
MBSSETUPS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
MBSSETUPO        := $(MBSSETUPS:.cxx=.o)

MBSSETUPDEP      := $(MBSSETUPO:.o=.d) $(MBSSETUPDO:.o=.d)

MBSSETUPLIB      := $(LPATH)/libTMbsSetup.$(SOEXT)
MBSSETUPLIBDEP   := $(LPATH)/libTMrbUtils.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MBSSETUPHL))
ALLLIBS     += $(MBSSETUPLIB)

# include all dependency files
INCLUDEFILES += $(MBSSETUPDEP)

##### local rules #####
$(info MBSSETUPL $(MBSSETUPL))
$(info MBSSETUPH $(MBSSETUPH))

include/%.h:    $(MBSSETUPDIRI)/%.h
		cp $< $@

$(MBSSETUPLIB):     $(MBSSETUPDO) $(MBSSETUPO) $(MBSSETUPLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMbsSetup.$(SOEXT) $@ "$(MBSSETUPO) $(MBSSETUPDO)" \
		   "$(MBSSETUPLIBEXTRA)"
		@(if [ -f $(MBSSETUPPCM) ] ; then \
			echo "cp  $(MBSSETUPPCM)----------------------" ; \
			cp $(MBSSETUPPCM) $(LPATH); \
		fi)

$(MBSSETUPDS):     $(MBSSETUPHL) $(MBSSETUPL)
		@echo "Generating dictionary $@...for ROOT $(ROOT_MAJOR)"
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -Iinclude $(MBSSETUPH) $(MBSSETUPL)
else
		rootcling -f $@ $(call dictModule,MBSSETUP)  -I$(MARABOU_SRCDIR)/include $(MBSSETUPH) $(MBSSETUPL)
endif

$(MBSSETUPDO):     $(MBSSETUPDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-mbssetup:       $(MBSSETUPLIB)

clean-mbssetup:
		@rm -f $(MBSSETUPO) $(MBSSETUPDO) $(MBSSETUPDS)

clean::         clean-mbssetup

distclean-mbssetup: clean-mbssetup
		@rm -f $(MBSSETUPDEP) $(MBSSETUPDS) $(MBSSETUPDH) $(MBSSETUPLIB)

distclean::     distclean-mbssetup
