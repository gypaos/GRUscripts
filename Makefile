include Makefile.arch

#------------------------------------------------------------------------------
SHARELIB      =  libGUser_online.so libGUser_convert.so
MAIN          = Analysis
all:            $(SHARELIB) $(MAIN)
#------------------------------------------------------------------------------
############### Detector ##############
libGUser_convert.so: GUser_convert.o GUser_convertDict.o
	$(LD) $(SOFLAGS) $^ $(LIBRARY) $(OutPutOpt) $@

libGUser_online.so: GUser_online.o GUser_onlineDict.o
	$(LD) $(SOFLAGS) $^ $(LIBRARY) $(OutPutOpt) $@

Analysis: Analysis.o libGUser_convert.so
	 $(LD) $(LDFLAGS) $^ $(LIBS) $(LIBRARY) $(OutPutOpt) $@

GUser_convertDict.cxx: GUser_convert.h
			rootcint -f $@ -c $^ $(INCLUDE) GUserLinkDef.h
GUser_onlineDict.cxx: GUser_online.h
			rootcint -f $@ -c $^ $(INCLUDE) GUserLinkDef.h


# dependances
GUser_convert.o: GUser_convert.cxx GUser_convert.h
GUser_online.o: GUser_online.cxx GUser_online.h
Analysis.o: Analysis.cxx Analysis.h

############# Clean and More ##########
clean:
	@rm -f core *~ *.o *Dict*

distclean:
	  make clean; rm -f *.so

.SUFFIXES: .$(SrcSuf)

###

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<


