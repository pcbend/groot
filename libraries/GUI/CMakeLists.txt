
#this will compile the current library.
add_library(GUI SHARED 
  Histomatic.cxx 
  GListTree.cxx 
  GG.cxx
)

target_include_directories(GUI PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
)

root_generate_dictionary(G__GUI 
  Histomatic.h 
  GListTree.h 
  GG.h 
  MODULE GUI 
  LINKDEF Linkdef.h)

target_link_libraries(GUI PUBLIC 
  ${ROOT_LIBRARIES} 
  ROOT::Gui 
  GROOTLIB
)


