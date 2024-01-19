
#this will compile the current library.
add_library(GObjectManager SHARED GObjectManager.cxx)
root_generate_dictionary(G__GObjectManager GObjectManager.h MODULE GObjectManager LINKDEF GObjectManagerLinkdef.h)
target_link_libraries(GObjectManager ROOT::Core GROOTLIB)

#link the compiled libraries above to the final executable.
target_link_libraries(groot GObjectManager)        

