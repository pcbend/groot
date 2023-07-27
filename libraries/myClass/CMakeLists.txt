
#this will compile the current library.
add_library(myClass SHARED myClass.cxx)
root_generate_dictionary(G__myClass myClass.h MODULE myClass LINKDEF myClassLinkdef.h)
target_link_libraries(myClass ROOT::Core)

#link the compiled libraries above to the final executable.
target_link_libraries(rootGUI myClass)        

