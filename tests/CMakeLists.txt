add_executable(groot_test_file_readers unit/test_file_readers.cxx)
set_target_properties(groot_test_file_readers PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_file_readers PRIVATE GCORE GROOTLIB)
target_compile_definitions(groot_test_file_readers PRIVATE
  GROOT_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_test(NAME file_readers COMMAND groot_test_file_readers)

add_executable(groot_test_projection unit/test_projection.cxx)
set_target_properties(groot_test_projection PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_projection PRIVATE GROOTLIB)
add_test(NAME projection COMMAND groot_test_projection)

add_executable(groot_test_calibration unit/test_calibration.cxx)
set_target_properties(groot_test_calibration PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_calibration PRIVATE GROOTLIB)
add_test(NAME calibration COMMAND groot_test_calibration)

add_executable(groot_test_commands unit/test_commands.cxx)
set_target_properties(groot_test_commands PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_commands PRIVATE GROOTLIB)
add_test(NAME commands COMMAND groot_test_commands)

add_executable(groot_test_gui_history unit/test_gui_history.cxx)
set_target_properties(groot_test_gui_history PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_gui_history PRIVATE GROOTLIB)
add_test(NAME gui_history COMMAND groot_test_gui_history)

add_executable(groot_test_help unit/test_help.cxx)
set_target_properties(groot_test_help PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_help PRIVATE GAPP)
add_test(NAME help COMMAND groot_test_help)

add_executable(groot_test_roi unit/test_roi.cxx)
set_target_properties(groot_test_roi PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
target_link_libraries(groot_test_roi PRIVATE GROOTLIB)
add_test(NAME roi COMMAND groot_test_roi)

find_program(ROOT_EXECUTABLE root HINTS ${ROOT_BINDIR})
if(ROOT_EXECUTABLE)
  add_test(
    NAME root_load_macro
    COMMAND ${ROOT_EXECUTABLE}
            -l
            -b
            -q
            "${CMAKE_CURRENT_SOURCE_DIR}/macros/load_groot.C(\"${CMAKE_BINARY_DIR}\")"
  )
endif()
