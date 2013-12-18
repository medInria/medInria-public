################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medVtkInriaTests-helpers)

## #############################################################################
## Configure files
## #############################################################################

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/simplevolume.vtk
  ${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk
  COPYONLY
  )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/tensors.vtk
  ${CMAKE_CURRENT_BINARY_DIR}/tensors.vtk
  COPYONLY
  )


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

foreach(test ${${PROJECT_NAME}_SOURCES})
    get_filename_component(test_filename ${test} NAME)
    set(TESTS_FILENAME ${test_filename} ${TESTS_FILENAME})
    get_filename_component(test_name ${test} NAME_WE)
    set(TESTS_NAME ${test_name} ${TESTS_NAME})
endforeach()

create_test_sourcelist(${PROJECT_NAME}_TESTS ${PROJECT_NAME}.cxx
  ${${PROJECT_NAME}_TESTS_FILENAME}
  )


## #############################################################################
## Add Exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_TESTS}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  vtkRendering
  vtkIO
  medVtkInria
  )


## #############################################################################
## Add tests
## #############################################################################

foreach (test_name ${${PROJECT_NAME}_TESTS_NAME})
  add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    ${test_name}
    "${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk"
    )
endforeach()

