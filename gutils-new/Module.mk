# Module.mk for gutils module
#
# R.Lutter Mar-2009


MODDIR       := gutils-new
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

NGUTILSDIR      := $(MODDIR)
NGUTILSDIRS     := $(NGUTILSDIR)/src
NGUTILSDIRI     := $(NGUTILSDIR)/inc

##### lib #####
NGUTILSL        := $(MODDIRI)/LinkDef.h
NGUTILSDS       := $(MODDIRS)/G__TNGMrbUtilsDict.cxx
NGUTILSDO       := $(NGUTILSDS:.cxx=.o)
NGUTILSH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
NGUTILSS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
NGUTILSO        := $(NGUTILSS:.cxx=.o)

NGUTILSDEP      := $(NGUTILSO:.o=.d) $(NGUTILSDO:.o=.d)

NGUTILSLIB      := $(LPATH)/libTNGMrbUtils.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(NGUTILSH))
ALLLIBS     += $(NGUTILSLIB)

# include all dependency files
INCLUDEFILES += $(NGUTILSDEP)

##### local rules #####

include/%.h:    $(NGUTILSDIRI)/%.h
		cp $< $@

$(NGUTILSLIB):     $(NGUTILSDO) $(NGUTILSO) $(MAINLIBS) $(NGUTILSLIBDEP)
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTNGMrbUtils.$(SOEXT) $@ "$(NGUTILSDO) $(NGUTILSO)" \
		   "$(NGUTILSLIBEXTRA)"

$(NGUTILSDS):     $(NGUTILSH) $(NGUTILSL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(NGUTILSH) $(NGUTILSL)

$(NGUTILSDO):     $(NGUTILSDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-ngutils:       $(NGUTILSLIB)

clean-ngutils:
		@rm -f $(NGUTILSO) $(NGUTILSDO) $(NGUTILSDS)

clean::         clean-ngutils

distclean-ngutils: clean-ngutils
		@rm -f $(NGUTILSDEP) $(NGUTILSDS) $(NGUTILSDH) $(NGUTILSLIB)

distclean::     distclean-ngutils
