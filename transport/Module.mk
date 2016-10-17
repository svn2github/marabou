# Module.mk for transport module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR       := transport
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

TRANSPDIR       := $(MODDIR)
TRANSPDIRS     := $(TRANSPDIR)/src
TRANSPDIRI     := $(TRANSPDIR)/inc

##### lib #####
TRANSPL        := $(MODDIRI)/LinkDef.h
TRANSPDS       := $(MODDIRS)/G__TMrbTransportDict.cxx
TRANSPPCM       := $(MODDIRS)/G__TMrbTransportDict_rdict.pcm
TRANSPDO       := $(TRANSPDS:.cxx=.o)
TRANSPHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
TRANSPH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(TRANSPHL))
TRANSPS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
TRANSPO        := $(TRANSPS:.cxx=.o)

MBSIOOBJS         := obj/mbsio.o obj/byte_order.o

TRANSPDEP      := $(TRANSPO:.o=.d) $(TRANSPDO:.o=.d)

TRANSPLIB      := $(LPATH)/libTMrbTransport.$(SOEXT)
TRANSPLIBDEP      := $(LPATH)/libTMrbUtils.$(SOEXT)
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(TRANSPHL))
ALLLIBS     += $(TRANSPLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(TRANSPPCM)
endif

# $(info in TRANSPLIB MBSIOOBJS: $(MBSIOOBJS))
# include all dependency files
INCLUDEFILES += $(TRANSPDEP)

##### local rules #####

include/%.h:    $(TRANSPDIRI)/%.h
		cp $< $@

$(TRANSPLIB):     $(TRANSPDO) $(TRANSPO) $(MBSIOOBJS) $(TRANSPLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbTransport.$(SOEXT) $@ "$(TRANSPO) $(TRANSPDO)" \
		   "$(MBSIOOBJS) $(TRANSPLIBEXTRA)"
		@(if [ -f $(TRANSPPCM) ] ; then \
			echo "cp  $(TRANSPPCM)----------------------" ; \
			cp $(TRANSPPCM) $(LPATH); \
		fi)

$(TRANSPDS):     $(TRANSPHL) $(TRANSPL)
		@echo "Generating dictionary $@..."
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -Iinclude $(TRANSPH) $(TRANSPL)
else
		rootcling -f $@ $(call dictModule,TRANSP)  -I$(MARABOU_SRCDIR)/include $(TRANSPH) $(TRANSPL)
endif

$(TRANSPDO):     $(TRANSPDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-transport:       $(TRANSPLIB)

clean-transport:
		@rm -f $(TRANSPO) $(TRANSPDS) $(TRANSPDO)

clean::         clean-transport

distclean-transport: clean-transport
		@rm -f $(TRANSPDEP) $(TRANSPDS)  $(TRANSPDO) $(TRANSPLIB)

distclean::     distclean-transport
