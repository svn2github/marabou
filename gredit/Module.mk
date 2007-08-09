# Module.mk for gredit module
#
# Author: O.Schaile,


MODDIR       := gredit
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

GREDITDIR      := $(MODDIR)
GREDITDIRS     := $(GREDITDIR)/src
GREDITDIRI     := $(GREDITDIR)/inc

##### lib #####
GREDITL        := $(MODDIRI)/LinkDef.h
GREDITDS       := $(MODDIRS)/G__GrEditDict.cxx
GREDITDO       := $(GREDITDS:.cxx=.o)
GREDITH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
GREDITS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GREDITO        := $(GREDITS:.cxx=.o)

GREDITDEP      := $(GREDITO:.o=.d) $(GREDITDO:.o=.d)

GREDITLIB      := $(LPATH)/libGrEdit.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(GREDITH))
ALLLIBS     += $(GREDITLIB)

# include all dependency files
INCLUDEFILES += $(GREDITDEP)

##### local rules #####

include/%.h:    $(GREDITDIRI)/%.h
		cp $< $@

$(GREDITLIB):     $(GREDITDO) $(GREDITO) $(MAINLIBS) $(GREDITLIBDEP)
			@echo "Making libGrEdit $@..."
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
			"$(SOFLAGS)" libGrEdit.$(SOEXT) $@ "$(GREDITDO) $(GREDITO)" \
			"$(GREDITLIBEXTRA)"

$(GREDITDS):     $(GREDITH) $(GREDITL)
		@echo "Generating dictionary: $@...."
		$(ROOTCINT) -f $@ -c -p -Iinclude defineMarabou.h $(GREDITH) $(GREDITL)

$(GREDITDO):     $(GREDITDS)
		@echo "Compiling dictionary $@..."
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-gredit:      $(GREDITLIB)

clean-gredit:
		@rm -f $(GREDITO) $(GREDITDO) $(GREDITDS)

clean::         clean-gredit

distclean-gredit: clean-gredit
		@rm -f $(GREDITDEP) $(GREDITDS) $(GREDITDH) $(GREDITLIB)

distclean::     distclean-gredit
