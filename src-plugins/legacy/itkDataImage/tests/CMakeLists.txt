################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(itkDataImagePluginTests) #TODO : doesn't work by now

## #############################################################################
## Sources
## #############################################################################
return()
list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

foreach(test ${${PROJECT_NAME}_SOURCES})
    get_filename_component(test_filename ${test} NAME)
    set(${PROJECT_NAME}_TESTS_FILENAME 
      ${test_filename} 
      ${${PROJECT_NAME}_TESTS_FILENAME}
      )
    get_filename_component(test_name ${test} NAME_WE)
    set(${PROJECT_NAME}_TESTS_NAME 
      ${test_name} 
      ${${PROJECT_NAME}_TESTS_NAME}
      )
endforeach()

create_test_sourcelist(${PROJECT_NAME}_TESTS ${PROJECT_NAME}.cxx
  ${${PROJECT_NAME}_TESTS_FILENAME}
  )

if(ITK_DIR)
  set(ITK_DATA_ROOT ${ITK_DIR}/Examples/Data)
endif()

if(MSVC_IDE)
set(PLUGIN_PATH
  ${LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
  )
else()
set(PLUGIN_PATH
  ${LIBRARY_OUTPUT_DIRECTORY}
  )
endif()

## #############################################################################
## Add Exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_TESTS}
  )

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  )

## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_QTTEST_LIBRARY}
  ${QT_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  ITKCommon
  )


## #############################################################################
## Add tests
## #############################################################################

foreach(test_name ${${PROJECT_NAME}_TESTS_NAME})
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${test_name}
    ${PLUGIN_PATH}
    ${ITK_DATA_ROOT}
    )
endforeach()

