
#this will compile the current library.
add_library(GUI SHARED Histomatic.cxx GListTree.cxx GG.cxx)
root_generate_dictionary(G__GUI Histomatic.h GListTree.h GG.h MODULE GUI LINKDEF Linkdef.h)
target_link_libraries(GUI ${ROOT_LIBRARIES} ROOT::Gui GROOTLIB)

#link the compiled libraries above to the final executable.
target_link_libraries(groot GUI)        

