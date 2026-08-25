#this will compile the current library.
add_library(GAPP SHARED Gint.cxx GHelp.cxx)

target_include_directories(GAPP PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
)

root_generate_dictionary(G__GAPP Gint.h MODULE GAPP LINKDEF Linkdef.h)
target_link_libraries(GAPP PUBLIC ${ROOT_LIBRARIES} GCORE GUI GROOTLIB)
