
#this will compile the current library.
add_library(Histomatic SHARED Histomatic.cxx)
root_generate_dictionary(G__Histomatic Histomatic.h MODULE Histomatic LINKDEF Linkdef.h)
target_link_libraries(Histomatic ${ROOT_LIBRARIES} ROOT::Gui)

#link the compiled libraries above to the final executable.
target_link_libraries(groot Histomatic)        

