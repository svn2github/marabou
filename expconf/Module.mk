# Module.mk for expconf module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR         := expconf
MODDIRS         := $(MODDIR)/src
MODDIRI         := $(MODDIR)/inc

EXPCONFDIR       := $(MODDIR)
EXPCONFDIRS     := $(EXPCONFDIR)/src
EXPCONFDIRI     := $(EXPCONFDIR)/inc

##### lib #####
EXPCONFL        := $(MODDIRI)/LinkDef.h
EXPCONFDS       := $(MODDIRS)/G__TMrbConfigDict.cxx
EXPCONFPCM       := $(MODDIRS)/G__TMrbConfigDict_rdict.pcm
EXPCONFDO       := $(EXPCONFDS:.cxx=.o)
EXPCONFH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
# $(info info EXPCONFH: $(EXPCONFH))
EXPCONFHM    = $(patsubst $(MODDIRI)/%.h,include/%.h,$(EXPCONFH))
# $(info info EXPCONFHM = $(EXPCONFHM))
EXPCONFS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
EXPCONFO        := $(EXPCONFS:.cxx=.o)

EXPCONFDEP      := $(EXPCONFO:.o=.d) $(EXPCONFDO:.o=.d)

EXPCONFLIB      := $(LPATH)/libTMrbConfig.$(SOEXT)
EXPCONFRMAP      := $(LPATH)/libTMrbConfig.rootmap
EXPCONFLIBDEP   := $(LPATH)/libTMrbUtils.so

# used in the main Makefile
EXPCONFHM    = $(patsubst $(MODDIRI)/%.h,include/%.h,$(EXPCONFH))
$info(info EXPCONFHM = $(EXPCONFHM))
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(EXPCONFH))
ALLLIBS     += $(EXPCONFLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(EXPCONFPCM)
endif

# include all dependency files
INCLUDEFILES += $(EXPCONFDEP)

##### local rules #####

include/%.h:    $(EXPCONFDIRI)/%.h
		cp $< $@

$(EXPCONFLIB):     $(EXPCONFHM) $(EXPCONFDO) $(EXPCONFO) $(MAINLIBS) $(EXPCONFLIBDEP)
		@echo "at make EXPCONFLIB,EXPCONFHM = " $(EXPCONFHM)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbConfig.$(SOEXT) $@ "$(EXPCONFO) $(EXPCONFDO)" \
		   "$(EXPCONFLIBEXTRA)"
		@(if [ -f $(EXPCONFPCM) ] ; then \
			echo "cp  $(EXPCONFPCM)----------------------" ; \
			cp $(EXPCONFPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
		@$(RLIBMAP) -o $(EXPCONFRMAP) -l $(EXPCONFLIB) -d $(EXPCONFLIBDEP) -c $(EXPCONFL)
endif

$(EXPCONFDS):     $(EXPCONFH) $(EXPCONFL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(EXPCONFH) $(EXPCONFL)

$(EXPCONFDO):     $(EXPCONFDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-expconf:       $(EXPCONFLIB)

clean-expconf:
		@rm -f $(EXPCONFO) $(EXPCONFDO) $(EXPCONFDS)

clean::         clean-expconf

distclean-expconf: clean-expconf
		@rm -f $(EXPCONFDEP) $(EXPCONFDS) $(EXPCONFDH) $(EXPCONFLIB)

distclean::     distclean-expconf
