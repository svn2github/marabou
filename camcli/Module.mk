# Module.mk for camcli module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR       := camcli
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

CAMCLIDIR      := $(MODDIR)
CAMCLIDIRS     := $(CAMCLIDIR)/src
CAMCLIDIRI     := $(CAMCLIDIR)/inc

##### lib #####
CAMCLIH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
CAMCLIS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.c))
CAMCLIO        := $(CAMCLIS:.c=.o)

CAMCLIDEP      := $(CAMCLIO:.o=.d)

CAMCLILIB      := $(LPATH)/libEsoneClient.$(SOEXT)

CFLAGS	+=	-D_NO_PROTOS_
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(CAMCLIH))
ALLLIBS     += $(CAMCLILIB)

# include all dependency files
INCLUDEFILES += $(CAMCLIDEP)

##### local rules #####

include/%.h:    $(CAMCLIDIRI)/%.h
		cp $< $@

$(CAMCLILIB):     $(CAMCLIDO) $(CAMCLIO) $(MAINLIBS) $(CAMCLILIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libEsoneClient.$(SOEXT) $@ "$(CAMCLIO)" \
		   "$(CAMCLILIBEXTRA)"

all-camcli:       $(CAMCLILIB)

clean-camcli:
		@rm -f $(CAMCLIO) $(CAMCLIDO)

clean::         clean-camcli

distclean-camcli: clean-camcli
		@rm -f $(CAMCLIDEP) $(CAMCLIDS) $(CAMCLIDH) $(CAMCLILIB)

distclean::     distclean-camcli
