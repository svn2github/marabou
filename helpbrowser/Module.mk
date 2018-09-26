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
HELPBPCM       := $(MODDIRS)/G__TMrbHelpBrowserDict_rdict.pcm
HELPBDO       := $(HELPBDS:.cxx=.o)
# all .h files except LinkDef.h: gredit/inc/aaa.h
HELPBHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
# $(info GREDITHL: $(GREDITHL))
# change to aaa.h
HELPBH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(HELPBHL))
# $(info HELPBH: $(HELPBH))
HELPBS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
HELPBO        := $(HELPBS:.cxx=.o)

HELPBDEP      := $(HELPBO:.o=.d) $(HELPBDO:.o=.d)
HELPBLIB      := $(LPATH)/libTMrbHelpBrowser.$(SOEXT)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(HELPBPCM)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(HELPBHL))
ALLLIBS     += $(HELPBLIB)

# include all dependency files
INCLUDEFILES += $(HELPBDEP)

##### local rules #####

include/%.h:    $(HELPBDIRI)/%.h
		cp $< $@

$(HELPBLIB):     $(HELPBDO) $(HELPBO)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbHelpBrowser.$(SOEXT) $@ "$(HELPBO) $(HELPBDO)" \
		   "$(HELPBLIBEXTRA)"
		@(if [ -f $(HELPBPCM) ] ; then \
			echo "cp  $(HELPBPCM)----------------------" ; \
			cp $(HELPBPCM) $(LPATH); \
		fi)

$(HELPBDS):     $(HELPBHL) $(HELPBL)
		@echo "Generating dictionary $@..."
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include $(HELPBH) $(HELPBL)
else
		rootcling -f $@ $(call dictModule,HELPB) -I$(MARABOU_SRCDIR)/include $(HELPBH) $(HELPBL)
endif
# 		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include $(HELPBH) $(HELPBL)
		
$(HELPBDO):     $(HELPBDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-helpb:       $(HELPBLIB)

clean-helpb:
		@rm -f $(HELPBO) $(HELPBDO) $(HELPBDS)

clean::         clean-helpb

distclean-helpb: clean-helpb
		@rm -f $(HELPBDS) $(HELPBDH) $(HELPBLIB)

distclean::     distclean-helpb
