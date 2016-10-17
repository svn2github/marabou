# Module.mk for xiadgf module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR         := xiadgf
MODDIRS        := $(MODDIR)/src
MODDIRI        := $(MODDIR)/inc

XIADGFDIR      := $(MODDIR)
XIADGFDIRS     := $(XIADGFDIR)/src
XIADGFDIRI     := $(XIADGFDIR)/inc

XIADGFL        := $(MODDIRI)/LinkDef.h
XIADGFDS       := $(MODDIRS)/G__TMrbDGFDict.cxx
XIADGFPCM       := $(MODDIRS)/G__TMrbDGFDict_rdict.pcm
XIADGFDO       := $(XIADGFDS:.cxx=.o)
XIADGFHL        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
XIADGFH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(XIADGFHL))
XIADGFS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
XIADGFO        := $(XIADGFS:.cxx=.o)

XIADGFDEP      := $(XIADGFO:.o=.d) $(XIADGFDO:.o=.d)

XIADGFLIB      := $(LPATH)/libTMrbDGF.$(SOEXT)
XIADGFLIBDEP   := $(LPATH)/libTMrbUtils.$(SOEXT) $(LPATH)/libTMrbEsone.$(SOEXT) 
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(XIADGFHL))
ALLLIBS     += $(XIADGFLIB)

# include all dependency files
INCLUDEFILES += $(XIADGFDEP)

##### local rules #####

include/%.h:    $(XIADGFDIRI)/%.h
		cp $< $@

$(XIADGFLIB):     $(XIADGFDO) $(XIADGFO) $(XIADGFLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbDGF.$(SOEXT) $@ "$(XIADGFO) $(XIADGFDO)" \
		   "$(XIADGFLIBEXTRA)"
		@(if [ -f $(XIADGFPCM) ] ; then \
			echo "cp  $(XIADGFPCM)----------------------" ; \
			cp $(XIADGFPCM) $(LPATH); \
		fi)

$(XIADGFDS):     $(XIADGFHL) $(XIADGFL)
		@echo "Generating dictionary $@...for $(ROOT_MAJOR)"
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -Iinclude $(XIADGFH) $(XIADGFL)
else 
		rootcling -f $@ $(call dictModule,XIADGF) -I$(MARABOU_SRCDIR)/include $(XIADGFH) $(XIADGFL)
endif

$(XIADGFDO):     $(XIADGFDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-xiadgf:       $(XIADGFLIB)

clean-xiadgf:
		@rm -f $(XIADGFO) $(XIADGFDO)

clean::         clean-xiadgf

distclean-xiadgf: clean-xiadgf
		@rm -f $(XIADGFDEP) $(XIADGFDS) $(XIADGFDH) $(XIADGFLIB)

distclean::     distclean-xiadgf
