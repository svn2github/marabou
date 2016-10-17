# Module.mk for mbsio module
#
# Author: O.Schaile 24/11/2000
MODDIR       := mbsio
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc


MBSIODIR      := $(MODDIR)
MBSIODIRS     := $(MBSIODIR)/src
MBSIODIRI     := $(MBSIODIR)/inc

MBSIOH        := $(wildcard $(MODDIRI)/*.h)
MBSIOS        := $(wildcard $(MODDIRS)/*.c)
MBSIOO        := $(MBSIODIRS)/byte_order.o $(MBSIODIRS)/mbsio.o

MBSIODEP      := $(MBSIOO:.o=.d)

MBSTSTEXE      := bin/mbstst

MBSWRITEXE     := bin/mbswrite

# ALLEXECS    += $(MBSTSTEXE)
ALLEXECS    += $(MBSTSTEXE) $(MBSWRITEXE)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MBSIOH))

ALLOBJS     += obj/mbsio.o obj/byte_order.o

# include all dependency files
INCLUDEFILES += $(MBSIODEP)

##### extra libs needed #####
MRBLIBS       = $(LPATH)/libTMrbUtils.$(SOEXT) $(LPATH)/libTGMrbUtils.$(SOEXT)\
                $(LPATH)/libTMbsSetup.$(SOEXT)
##### local rules #####

include/%.h:    $(MBSIODIRI)/%.h
		cp $< $@

obj/%.o:    $(MODDIRS)/%.o
		cp $< $@

$(MBSTSTEXE):    $(LPATH)/libTMbsSetup.$(SOEXT) $(MBSIODIRS)/mbstst.o
		@echo "$(MBSTSTEXE) start linking"
		@echo "----------------------------------------------------"
		@echo "$(MBSIOO)"
		@echo "----------------------------------------------------"

		$(LD) -g $(LDFLAGS) $(MBSIODIRS)/mbstst.o $(MBSIOO)  $(MBSCLIB) $(ROOTGLIBS) \
            -o $(MBSTSTEXE)

$(MBSWRITEXE):    $(LPATH)/libTMbsSetup.$(SOEXT) $(MBSIODIRS)/mbswrite.o
					@echo "$(MBSWRITEXE) start linking"
					@echo "----------------------------------------------------"
					@echo "$(MBSIOO)"
					@echo "----------------------------------------------------"

		$(LD) -g $(LDFLAGS) $(MBSIODIRS)/mbswrite.o $(MBSIOO) $(MBSCLIB) $(ROOTGLIBS) \
										            -o $(MBSWRITEXE)


all-mbsio:       $(MBSIOEXE)

clean-mbsio:
#		@echo "------in clean-mbsio---------------------"
#		@echo "$(MBSIOO)"
#		@echo "$(MBSIOS)"
#		@echo "$(MBSIODEP)"
#		@echo "----------------------------------------------------"
		@rm -f $(MBSIOO) $(MBSTSTEXE)
      
clean::         clean-mbsio

distclean-mbsio: clean-mbsio
		@rm -f $(MBSIODEP)  $(MBSTSTEXE)

distclean::     distclean-mbsio
