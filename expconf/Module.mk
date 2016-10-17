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
EXPCONFPCM      := $(MODDIRS)/G__TMrbConfigDict_rdict.pcm
EXPCONFDO       := $(EXPCONFDS:.cxx=.o)
EXPCONFHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
EXPCONFH        := $(patsubst $(MODDIRI)/%.h,%.h,$(EXPCONFHL))

EXPCONFS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
EXPCONFO        := $(EXPCONFS:.cxx=.o)

EXPCONFDEP      := $(EXPCONFO:.o=.d) $(EXPCONFDO:.o=.d)

EXPCONFLIB      := $(LPATH)/libTMrbConfig.$(SOEXT)
EXPCONFRMAP     := $(LPATH)/libTMrbConfig.rootmap
EXPCONFLIBDEP   := $(LPATH)/libTMrbUtils.so $(LPATH)/libTMbsSetup.so

# used in the main Makefile
# $(info EXPCONFH = $(EXPCONFH))
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(EXPCONFHL))
ALLLIBS     += $(EXPCONFLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(EXPCONFPCM)
endif

# include all dependency files
INCLUDEFILES += $(EXPCONFDEP)

##### local rules #####

include/%.h:    $(EXPCONFDIRI)/%.h
		cp $< $@

$(EXPCONFLIB):    $(EXPCONFDO) $(EXPCONFO) $(EXPCONFLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbConfig.$(SOEXT) $@ "$(EXPCONFO) $(EXPCONFDO)" \
		   "$(EXPCONFLIBEXTRA)"
		@(if [ -f $(EXPCONFPCM) ] ; then \
			echo "cp  $(EXPCONFPCM)----------------------" ; \
			cp $(EXPCONFPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
		@$(RLIBMAP) -o $(EXPCONFRMAP) -l $(EXPCONFLIB) -d $(EXPCONFLIBDEP) -c $(EXPCONFL)
else
##			echo "--- $(ROOTCINT) $(EXPCONFRMAP)----------------------" ;
##			@$(ROOTCINT) -f $(EXPCONFDS) -rmf $(EXPCONFRMAP) -c $(EXPCONFHL) $(EXPCONFL)
endif

$(EXPCONFDS):     $(EXPCONFHL) $(EXPCONFL)
		@echo "Generating dictionary $@...for ROOT $(ROOT_MAJOR)."
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -Iinclude $(EXPCONFH) $(EXPCONFL)
else		
		rootcling -f $@ $(call dictModule,EXPCONF) -I$(MARABOU_SRCDIR)/include $(EXPCONFH) $(EXPCONFL)
endif

$(EXPCONFDO):     $(EXPCONFDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-expconf:       $(EXPCONFLIB)

clean-expconf:
		@rm -f $(EXPCONFO) $(EXPCONFDO) $(EXPCONFDS)

clean::         clean-expconf

distclean-expconf: clean-expconf
		@rm -f $(EXPCONFDEP) $(EXPCONFDS) $(EXPCONFDH) $(EXPCONFLIB)

distclean::     distclean-expconf
