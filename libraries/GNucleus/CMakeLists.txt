
#this will compile the current library.
add_library(GNUCLEUS SHARED GNucleus.cxx GTransition.cxx)

target_include_directories(GNUCLEUS PUBLIC
  ${CMAKE_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__GNUCLEUS 
  GNucleus.h 
  GTransition.h
  MODULE GNUCLEUS 
  LINKDEF Linkdef.h
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.dat
  ${CMAKE_BINARY_DIR}/data/mass.dat
  COPYONLY
)

target_link_libraries(GNUCLEUS PUBLIC ${ROOT_LIBRARIES})


