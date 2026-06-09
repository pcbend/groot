
#this will compile the current library.
add_library(GTOOLS SHARED GNucleus.cxx GTransition.cxx)

target_include_directories(GTOOLS PUBLIC
  ${CMAKE_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GTOOLS 
  GNucleus.h 
  GTransition.h
  MODULE GTOOLS 
  LINKDEF Linkdef.h
)

target_link_libraries(GTOOLS PUBLIC ${ROOT_LIBRARIES})




file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.dat
  ${CMAKE_BINARY_DIR}/data/mass.dat
  COPYONLY
)
