
#this will compile the current library.
add_library(GROOTLIB SHARED GFunctions.cxx GCanvas.cxx)
root_generate_dictionary(G__GROOTLIB GFunctions.h GCanvas.h MODULE GROOTLIB LINKDEF LinkDef.h)
target_link_libraries(GROOTLIB ${ROOT_LIBRARIES} ROOT::MathMore)

#link the compiled libraries above to the final executable.
target_link_libraries(groot GROOTLIB)        

