# Module.mk for tidylib module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR       := tidylib
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

TIDYLIBDIR      := $(MODDIR)
TIDYLIBDIRS     := $(TIDYLIBDIR)/src
TIDYLIBDIRI     := $(TIDYLIBDIR)/inc

##### lib #####
TIDYLIBH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
TIDYLIBS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.c))
TIDYLIBO        := $(TIDYLIBS:.c=.o)

TIDYLIBDEP      := $(TIDYLIBO:.o=.d)

TIDYLIBLIB      := $(LPATH)/libTidy.$(SOEXT)

CFLAGS	+=	-D_NO_PROTOS_
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(TIDYLIBH))
ALLLIBS     += $(TIDYLIBLIB)

# include all dependency files
INCLUDEFILES += $(TIDYLIBDEP)

##### local rules #####

include/%.h:    $(TIDYLIBDIRI)/%.h
		cp $< $@

$(TIDYLIBLIB):     $(TIDYLIBDO) $(TIDYLIBO) $(MAINLIBS) $(TIDYLIBLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTidy.$(SOEXT) $@ "$(TIDYLIBO)" \
		   "$(TIDYLIBLIBEXTRA)"

all-tidylib:       $(TIDYLIBLIB)

clean-tidylib:
		@rm -f $(TIDYLIBO) $(TIDYLIBDO)

clean::         clean-tidylib

distclean-tidylib: clean-tidylib
		@rm -f $(TIDYLIBDEP) $(TIDYLIBDS) $(TIDYLIBDH) $(TIDYLIBLIB)

distclean::     distclean-tidylib
