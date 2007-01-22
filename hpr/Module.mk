# Module.mk for hpr module
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := hpr
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

HPRDIR      := $(MODDIR)
HPRDIRS     := $(HPRDIR)/src
HPRDIRI     := $(HPRDIR)/inc

##### lib #####
HPRL        := $(MODDIRI)/LinkDef.h
HPRDS       := $(MODDIRS)/G__HistPresentDict.cxx
HPRDO       := $(HPRDS:.cxx=.o)
HPRH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
HPRS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
HPROM       := $(HPRS:.cxx=.o)
HPRMAINO    := $(MODDIRS)/main.o
HPRO        := $(filter-out $(HPRMAINO),$(HPROM))


HPRDH			:= $(HPRDIRI)/defineMarabou.h \
               $(HPRDIRI)/HTCanvas.h \
 					$(HPRDIRI)/HandleMenus.h \
 					$(HPRDIRI)/FitHist.h \
 					$(HPRDIRI)/HistPresent.h \
 					$(HPRDIRI)/TMrbArrayD.h \
 					$(HPRDIRI)/TMrbArrayF.h \
  					$(HPRDIRI)/FhPeak.h \
					$(HPRDIRI)/FhContour.h \
               $(HPRDIRI)/FhRegion.h \
               $(HPRDIRI)/HprImage.h \
               $(HPRDIRI)/GroupOfGObjects.h \
               $(HPRDIRI)/GroupOfHists.h \
 					$(HPRDIRI)/GEdit.h \
 					$(HPRDIRI)/EditMarker.h \
 					$(HPRDIRI)/FhMarker.h \
 					$(HPRDIRI)/TSplineX.h \
 					$(HPRDIRI)/TSplineXDialog.h \
 					$(HPRDIRI)/TSplineXEditor.h \
 					$(HPRDIRI)/HprEditCommands.h \
 					$(HPRDIRI)/Ascii2GraphDialog.h \
 					$(HPRDIRI)/Ascii2HistDialog.h \
 					$(HPRDIRI)/Ascii2NtupleDialog.h \
 					$(HPRDIRI)/FeynmanDiagramDialog.h \
 					$(HPRDIRI)/InsertFunctionDialog.h \
 					$(HPRDIRI)/InsertArcDialog.h \
 					$(HPRDIRI)/FitOneDimDialog.h \
 					$(HPRDIRI)/CalibrationDialog.h \
 					$(HPRDIRI)/InsertTextDialog.h \
					$(HPRDIRI)/InsertTextBoxDialog.h \
					$(HPRDIRI)/TextBox.h \
  					$(HPRDIRI)/TCurlyLineWithArrow.h \
 					$(HPRDIRI)/CurlyLineWithArrowDialog.h \
 					$(HPRDIRI)/Save2FileDialog.h \
 					$(HPRDIRI)/TArcEditor.h \
 					$(HPRDIRI)/LinkDef.h

HPRDEP      := $(HPRO:.o=.d) $(HPRDO:.o=.d)
HPRDEP      += $(MODDIRS)/main.d

HPREXE      := bin/HistPresent
HPRLIB       := $(LPATH)/libHpr.so

ALLEXECS    += $(HPREXE)

ALLLIBS     += $(HPRLIB)
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(HPRH))

# include all dependency files
INCLUDEFILES += $(HPRDEP)

##### extra libs needed #####
OHPRLIBS      := -L$(LPATH) -lTMrbUtils -lTGMrbUtils -lTMrbHelpBrowser

##### local rules #####

include/%.h:    $(HPRDIRI)/%.h
		cp $< $@

$(HPREXE):     $(HPRSO)$(HPRO) $(HPRMAINO) $(MRBLIBS)
#		@echo "sources: $(HPRS)"
		@echo "$(HPREXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(HPRMAINO) $(HPRO) $(HPRDO) $(OHPRLIBS) $(ROOTGLIBS) \
            -o $(HPREXE)

$(HPRLIB):     $(HPRDO) $(HPRO)
#		@echo "objs: $(HPRO)"
		@echo "$(HPREXE) make shared lib ------------------------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libHpr.$(SOEXT) $@ "$(HPRO) $(HPRDO)"

$(HPRDS):     $(HPRDH) $(HPRL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(HPRDH) 

$(HPRDO):     $(HPRDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-hpr:       $(HPREXE)

clean-hpr:
		@rm -f $(HPRO) $(HPRMAINO) $(HPRDO) $(HPRDS)

clean::         clean-hpr

distclean-hpr: clean-hpr
		@rm -f $(HPRDEP) $(HPRDS) $(HPREXE) $(HPRLIB)

distclean::     distclean-hpr
