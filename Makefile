# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
include $(GANIL2ROOT)/libs/Makefile.arch

GUSERDIR     := ./

G2RLIB       := $(GANIL2ROOT)/libs/
LIBDIR       := $(GRUDIR)/grulib/
VIGRUDIR     := $(GRUDIR)/vigru/
GTDIR        := $(GRUDIR)/GT/
GGRUDIR      := $(GRUDIR)/GGRU/
GSOAPDIR     := $(GRUDIR)/gsoap/
MFM_DIR      := $(GRUDIR)/MFMlib/
GRUFLAGS     := -I$(GRUDIR)/include/ -I$(LIBDIR) -I$(VIGRUDIR) -I$(GTDIR) -I$(GGRUDIR) -I$(GSOAPDIR) -I$(MFM_DIR) -I$(G2RLIB) -I$(NPTOOL)/NPLib/include/
INCLIST      := General.h GAcq.h GDevice.h
SYSLIBS      := -L$(GRUDIR)/lib -lGRU
SYSLIBS      += `$(NPTOOL)/NPLib/liblist`
SYSLIBS      += -L$(GANIL2ROOT)/libs -lG2rCATS -lG2rCharissa -lG2rChateauCristal -lG2rChio_an -lG2rChio_dig -lG2rDetector -lG2rExl -lG2rExogam -lG2rLaBr3 -lG2rLise -lG2rLiseData -lG2rMaya -lG2rModularLabel -lG2rMust2 -lG2rPlastic -lG2rS1 -lG2rSSSD -lG2rSiLi -lG2rSiRes -lG2rSpegCHIO -lG2rSpegDC -lG2rSpegPlastic -lG2rTiaraBarrel -lG2rTiaraHyball -lG2rTrigger -lG2rVamosCHIO -lG2rVamosDC -lG2rVamosFinger -lG2rVamosPlastic
# Extensions des fichiers
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o 

# Options de ROOT
ROOTCFLAGS   := $(shell root-config --cflags) -I$(ROOTSYS)/xmlparser/inc -I$(ROOTSYS)/io/xmlparser/inc
ROOTLIBS     := $(shell root-config --libs) -lHtml -lSpectrum
ROOTGLIBS    := $(shell root-config --glibs)
OPTFLAGS      = -O3

CXXFLAGS     +=  $(ROOTCFLAGS) $(GRUFLAGS) $(OPTFLAGS) 
LIBS          = $(ROOTLIBS)  $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

CINTOPTFLAGS  = -O0

#------------------------------------------------------------------------------
SHARELIB      = libGUser_online.so libGUser_convert.so
MAIN          = Analysis
all:            $(SHARELIB) $(MAIN)
#------------------------------------------------------------------------------

Analysis:	Analysis.o libGUser_convert.so
	$(LD) $^ $(OutPutOpt) $@ -L./ -lGUser_convert $(LIBS) 

libGUser_online.so: GUser_online.o   GUser_onlineDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@

GUser_onlineDict.cxx:	GUser_online.h
			@echo "Generating dictionary $@..."
			rootcint -f $@ -c -p $(CINTOPTFLAGS) $(GRUFLAGS) $(INCLIST) $^ GUserLinkDef.h

libGUser_convert.so: GUser_convert.o   GUser_convertDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@

GUser_convertDict.cxx:	GUser_convert.h
			@echo "Generating dictionary $@..."
			rootcint -f $@ -c -p $(CINTOPTFLAGS) $(GRUFLAGS) $(INCLIST) $^ GUserLinkDef.h

clean:
	@rm -f core *~ *.o *Dict* 

distclean:
	  make clean; rm -f *.so

.SUFFIXES: .$(SrcSuf)

###
.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<

# dependances
$(CLASSNAME).o:	$(CLASSNAME).C	 $(CLASSNAME).h
$(MAIN).o:	$(MAIN).cxx	 $(MAIN).h
