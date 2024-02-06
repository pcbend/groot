
#this will compile the current library.
add_library(GROOTLIB SHARED GFunctions.cxx GCommands.cxx GCanvas.cxx GMarker.cxx GH1D.cxx GH2D.cxx GGaus.cxx GPeak.cxx GGlobals.cxx GUtils.cxx)
root_generate_dictionary(G__GROOTLIB GFunctions.h GCommands.h GCanvas.h GMarker.h GH1D.h GH2D.h GGaus.h GPeak.h GGlobals.h GUtils.h MODULE GROOTLIB LINKDEF LinkDef.h)
target_link_libraries(GROOTLIB ${ROOT_LIBRARIES} ROOT::MathMore ROOT::Gui ROOT::Spectrum)

#link the compiled libraries above to the final executable.
target_link_libraries(groot GROOTLIB)        

