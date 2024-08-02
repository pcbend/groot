
#this will compile the current library.
add_library(Gint SHARED Gint.cxx)
root_generate_dictionary(G__Gint Gint.h MODULE Gint LINKDEF Linkdef.h)
target_link_libraries(Gint ${ROOT_LIBRARIES} GUI)

#link the compiled libraries above to the final executable.
target_link_libraries(groot Gint)        

