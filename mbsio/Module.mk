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
MBSIOO        := $(MBSIOS:.c=.o)

MBSIODEP      := $(MBSIOO:.o=.d)

MBSTSTEXE      := bin/mbstst

ALLEXECS    += $(MBSTSTEXE)

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

$(MBSTSTEXE):      $(MBSIOO) $(MODDIRS)/mbstst.o
		@echo "$(MBSTSTEXE) start linking"
		@echo "----------------------------------------------------"
		@echo "$(MBSIOO)"
		@echo "----------------------------------------------------"

		$(LD) -g $(LDFLAGS) $(MBSIOO) $(MRBLIBS) $(MBSCLIB) $(ROOTGLIBS) \
            -o $(MBSTSTEXE)


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
