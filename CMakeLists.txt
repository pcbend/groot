cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#project information
project(groot 
        VERSION 0.1.0 
        DESCRIPTION "i am a description"
        HOMEPAGE_URL "i am a github page"
        LANGUAGES CXX)

if(APPLE) 
  message("Building for Darwin")
  add_compile_definitions(__darwin__=TRUE)
elseif(LINUX) 
  message("Building for Linux")
  add_compile_definitions(__linux__=TRUE)
endif()

#find the root, set the various cmake variables.
find_package(ROOT REQUIRED) 

include(CMakePrintHelpers)
cmake_print_variables(ROOT_CXX_FLAGS)

#setting output paths in the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})

#create the executablei, we do it 'first' so the exectuable variable is avail in subdirs
add_executable(groot src/groot.cxx)

#setting include paths: TODO, copy headers from library directories(?)
include_directories(include ${ROOT_INCLUDE_DIRS}) 

#This tells cmake to continue into these directories.
add_subdirectory(libraries)


