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
GREDITPCM      := $(MODDIRS)/G__GrEditDict_rdict.pcm
GREDITDO       := $(GREDITDS:.cxx=.o)
# all .h files except LinkDef.h: gredit/inc/aaa.h
GREDITHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
# $(info GREDITHL: $(GREDITHL))
# change to aaa.h
GREDITH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(GREDITHL))
# $(info GREDITH: $(GREDITH))
GREDITS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
GREDITO        := $(GREDITS:.cxx=.o)

GREDITDEP      := $(GREDITO:.o=.d) $(GREDITDO:.o=.d)

GREDITLIB      := $(LPATH)/libGrEdit.$(SOEXT)
GREDITRMAP     := $(LPATH)/libGrEdit.rootmap
CDLMAINO    := $(MODDIRS)/main.o
CDLO        := $(filter-out $(CDLMAINO),$(GREDITO))

CDLEXE      := bin/CreateCDlabel


# used in the main Makefile
# ALLHDRS needs include/
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(GREDITHL))
ALLLIBS     += $(GREDITLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(GREDITPCM)
endif

# include all dependency files
INCLUDEFILES += $(GREDITDEP)

MRBUTILSLIB   := $(LPATH)/libTMrbUtils.$(SOEXT)
MRBGUTILSLIB  := $(LPATH)/libTGMrbUtils.$(SOEXT)

OCDLLIBS      := $(GREDITLIB) $(MRBGUTILSLIB) $(MRBUTILSLIB)
GREDITLIBDEP  := $(MRBGUTILSLIB) $(MRBUTILSLIB)

##### local rules #####

include/%.h:    $(GREDITDIRI)/%.h
		cp $< $@

$(CDLEXE):      $(CDLMAINO) $(CDLLIB) $(OCDLLIBS)
		@echo "other libs: $(OCDLLIBS)"
		@echo "$(CDLEXE) linking exe ----------------------------------"
		$(LD) -g -o $(CDLEXE) $(LDFLAGS) $(CDLMAINO) $(OCDLLIBS) $(ROOTGLIBS)

$(GREDITLIB):     $(GREDITDO) $(GREDITO) $(GREDITLIBDEP)
			@echo "Making libGrEdit $@..."
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
			"$(SOFLAGS)" libGrEdit.$(SOEXT) $@ "$(GREDITDO) $(GREDITO) $(GREDITLIBEXTRA)"
		@(if [ -f $(GREDITPCM) ] ; then \
			echo "cp  $(GREDITPCM)----------------------" ; \
			cp $(GREDITPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
		@$(RLIBMAP) -o $(GREDITRMAP) -l $(GREDITLIB) -d $(GREDITLIBDEP) -c $(GREDITL)
endif

$(GREDITDS):     $(GREDITHL) $(GREDITL)
ifneq ($(ROOTV6), 1)
		@echo "Generating dictionary: $@...."
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include defineMarabou.h $(GREDITH) $(GREDITL)
else
		rootcling -f $@ $(call dictModule,GREDIT) -I$(MARABOU_SRCDIR)/include $(GREDITH) $(GREDITL)
endif

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
