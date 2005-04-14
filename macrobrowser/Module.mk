# Module.mk for macrobrowser module
#
# Author: O.Schaile 24/11/2000


MODDIR       := macrobrowser
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

MACROBDIR      := $(MODDIR)
MACROBDIRS     := $(MACROBDIR)/src
MACROBDIRI     := $(MACROBDIR)/inc

MACROB         := MacroBrowser


MACROBH        := $(MODDIRI)/$(MACROB).h
MACROBS        := $(MODDIRS)/$(MACROB).cxx
MACROBO        := $(MACROBS:.cxx=.o)


MACROBDEP      := $(MACROBO:.o=.d) $(MACROBDO:.o=.d)
MBSCDEP     := $(MBSCO:.o=.d) $(MBSCDO:.o=.d)

MACROBEXE      := bin/$(MACROB)

ALLEXECS    += $(MACROBEXE)

# used in the main Makefile
#ALLHDRS     += include/$(MACROB).h

# include all dependency files
INCLUDEFILES += $(MACROBDEP)

##### extra libs needed #####
MRBLIBS       = $(LPATH)/libTMrbUtils.$(SOEXT) $(LPATH)/libTGMrbUtils.$(SOEXT)
##### local rules #####

include/%.h:    $(MACROBDIRI)/%.h
		cp $< $@

$(MACROBEXE):    $(MACROBO) $(MRBLIBS)
		@echo "$(MACROBEXE) start linking"
		$(LD) -g $(LDFLAGS) $(MACROBO) $(MRBLIBS) $(ROOTGLIBS) \
            -o $(MACROBEXE)

$(MACROBDO):     $(MACROBDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-macrobrowser:       $(MACROBEXE) $(MBSCLIB)

clean-macrobrowser:
		@rm -f $(MACROBO) $(MACROBEXE)

clean::         clean-macrobrowser

distclean-macrobrowser: clean-macrobrowser
		@rm -f $(MACROBDEP) $(MACROBDS) $(MACROBDH) $(MACROBEXE)

distclean::     distclean-macrobrowser
