# Module.mk for xml module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: R. Lutter Dec-2007

MODDIR       := xml
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

XMLDIR      := $(MODDIR)
XMLDIRS     := $(XMLDIR)/src
XMLDIRI     := $(XMLDIR)/inc

##### lib #####
XMLL        := $(MODDIRI)/LinkDef.h
XMLDS       := $(MODDIRS)/G__TMrbXMLDict.cxx
XMLDO       := $(XMLDS:.cxx=.o)
XMLH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
XMLS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
XMLO        := $(XMLS:.cxx=.o)

XMLDEP      := $(XMLO:.o=.d) $(XMLDO:.o=.d)

XMLLIB      := $(LPATH)/libTMrbXML.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(XMLH))
ALLLIBS     += $(XMLLIB)

# include all dependency files
INCLUDEFILES += $(XMLDEP)

##### local rules #####

include/%.h:    $(XMLDIRI)/%.h
		cp $< $@

$(XMLLIB):     $(XMLDO) $(XMLO)  $(XMLLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbXML.$(SOEXT) $@ "$(XMLO) $(XMLDO)" \
		   "$(XMLLIBEXTRA)"

$(XMLDS):     $(XMLH) $(XMLL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(XMLH) $(XMLL)

$(XMLDO):     $(XMLDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<

all-xml:       $(XMLLIB)

clean-xml:
		@rm -f $(XMLO) $(XMLDO) $(XMLDS)

clean::         clean-xml

distclean-xml: clean-xml
		@rm -f $(XMLDEP) $(XMLDS) $(XMLDH) $(XMLLIB)

distclean::     distclean-xml
