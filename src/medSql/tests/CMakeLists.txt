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

project(medSqlTests)

## #############################################################################
## Sources
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
  ${TESTS_FILENAME}
  )


## #############################################################################
## include directorie.
## #############################################################################

#see cmake/includeHeaderDirectories.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
    ${medCore_INCLUDE_DIRS}
    ${medGui_INCLUDE_DIRS}
    ${medTest_INCLUDE_DIRS}
    )
    
    
## #############################################################################
## Add Exe
## #############################################################################

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_TESTS}
  )


## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_QTTEST_LIBRARY}
  ${QT_LIBRARIES}
  dtkCore
  medGui
  medCore
  medSql
  medTest
  )


## #############################################################################
## Add tests
## #############################################################################

foreach (test_name ${TESTS_NAME})
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${test_name})
endforeach()

