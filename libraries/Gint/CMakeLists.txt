
#this will compile the current library.
add_library(Gint SHARED Gint.cxx)

target_include_directories(Gint PUBLIC
  ${CMAKE_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}
) 

root_generate_dictionary(G__Gint Gint.h MODULE Gint LINKDEF Linkdef.h)
target_link_libraries(Gint PUBLIC ${ROOT_LIBRARIES} GUI)


