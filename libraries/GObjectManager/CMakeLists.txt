
#this will compile the current library.
add_library(GObjectManager SHARED GObjectManager.cxx)

target_include_directories(GObjectManager PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
)

root_generate_dictionary(G__GObjectManager GObjectManager.h 
	MODULE GObjectManager 
	LINKDEF GObjectManagerLinkdef.h
)

target_link_libraries(GObjectManager PUBLIC ROOT::Core GROOTLIB)


