
#this will compile the current library.
add_library(GROOTLIB SHARED 
  GFunctions.cxx 
  GCommands.cxx 
  GCanvas.cxx 
  GMarker.cxx 
  GH1D.cxx 
  GH2D.cxx 
  GGaus.cxx 
  GPeak.cxx 
  GPeak2.cxx 
  GGlobals.cxx 
  GROI.cxx
)

target_include_directories(GROOTLIB PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
)

root_generate_dictionary(G__GROOTLIB 
  GFunctions.h 
  GCommands.h 
  GCanvas.h 
  GMarker.h 
  GH1D.h 
  GH2D.h 
  GGaus.h 
  GPeak.h 
  GPeak2.h 
  GGlobals.h 
  GROI.h 
  MODULE GROOTLIB 
  LINKDEF LinkDef.h
)

target_link_libraries(GROOTLIB PUBLIC 
  ${ROOT_LIBRARIES} 
  ROOT::MathMore 
  ROOT::Gui 
  ROOT::Spectrum
)


