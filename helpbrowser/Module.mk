# Module.mk for TMrbHelpBrowser module
#
# Author: Otto Schaile  17.7.00

MODDIR       := helpbrowser
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

HELPBDIR      := $(MODDIR)
HELPBDIRS     := $(HELPBDIR)/src
HELPBDIRI     := $(HELPBDIR)/inc

##### lib #####
HELPBL        := $(MODDIRI)/LinkDef.h
HELPBDS       := $(MODDIRS)/G__TMrbHelpBrowserDict.cxx
HELPBDO       := $(HELPBDS:.cxx=.o)
HELPBH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
HELPBS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
HELPBO        := $(HELPBS:.cxx=.o)

HELPBDEP      := $(HELPBO:.o=.d) $(HELPBDO:.o=.d)

HELPBLIB      := $(LPATH)/libTMrbHelpBrowser.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(HELPBH))
ALLLIBS     += $(HELPBLIB)

# include all dependency files
INCLUDEFILES += $(HELPBDEP)

##### local rules #####

include/%.h:    $(HELPBDIRI)/%.h
		cp $< $@

$(HELPBLIB):     $(HELPBDO) $(HELPBO) $(MAINLIBS) $(HELPBLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbHelpBrowser.$(SOEXT) $@ "$(HELPBO) $(HELPBDO)" \
		   "$(HELPBLIBEXTRA)"

$(HELPBDS):     $(HELPBH) $(HELPBL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c $(HELPBH) $(HELPBL)

$(HELPBDO):     $(HELPBDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-helpb:       $(HELPBLIB)

clean-helpb:
		@rm -f $(HELPBO) $(HELPBDO) $(HELPBDS)

clean::         clean-helpb

distclean-helpb: clean-helpb
		@rm -f $(HELPBDS) $(HELPBDH) $(HELPBLIB)

distclean::     distclean-helpb
