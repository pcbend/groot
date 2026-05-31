cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#project information
project(groot 
        VERSION 0.2.0 
        DESCRIPTION "brave badger"
        HOMEPAGE_URL "https://github.com/pcbend/groot"
        LANGUAGES CXX)

if(APPLE) 
  message("Building for Darwin")
  add_compile_definitions(__DARWIN__=TRUE)
elseif(LINUX) 
  message("Building for Linux")
  add_compile_definitions(__LINUX__=TRUE)
endif()

#set(CMAKE_BUILD_TYPE Debug)

#find the root, set the various cmake variables.
find_package(ROOT REQUIRED) 

#include(CMakePrintHelpers)
#cmake_print_variables(ROOT_CXX_FLAGS)
#cmake_print_variables(ROOT_CXX_STANDARD)

set (CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)


#setting output paths in the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})

#create the executable, we do it 'first' so the exectuable variable is avail in subdirs
add_executable(groot src/groot.cxx)

#setting include paths: TODO, copy headers from library directories(?)
target_include_directories(groot PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}
) 

#This tells cmake to continue into these directories.
add_subdirectory(libraries)

target_link_libraries(groot PRIVATE
  GROOTLIB
  GUI
  Gint
  GObjectManager
)

string(ASCII 27 Esc)
set(RED     "${Esc}[31m")
set(GREEN   "${Esc}[32m")
set(YELLOW  "${Esc}[33m")
set(BLUE    "${Esc}[34m")
set(MAGENTA "${Esc}[35m")
set(CYAN    "${Esc}[36m")
set(RESET   "${Esc}[0m")

add_custom_command(
  TARGET groot
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
  COMMAND ${CMAKE_COMMAND} -E echo "${CYAN} goot build successfully, ${YELLOW} woo hoo! ${RESET}"
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
)








