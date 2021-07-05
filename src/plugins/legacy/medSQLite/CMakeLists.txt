################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME medSQLitePlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################


#find_package(dtk REQUIRED)
#include_directories(${dtk_INCLUDE_DIRS})
#
#find_package(ITK REQUIRED COMPONENTS ITKIOImageBase ITKIOMeta )
#include(${ITK_USE_FILE})
#
#find_package(VTK REQUIRED COMPONENTS vtkCommonExecutionModel vtkRenderingCore vtkInteractionWidgets)
#include(${VTK_USE_FILE})
#
#if (APPLE OR WIN32)
#    find_package(Boost REQUIRED)
#endif()

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
 )

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
   ${${TARGET_NAME}_HEADERS}
   )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${TARGET_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${TARGET_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
target_link_libraries(${TARGET_NAME}
#  dtkCore
#  dtkLog
        medCore
#        medCoreLegacy
#  medLog
#  medVtkInria
#  medImageIO
  )

set(TARGET_NAME_TEST medSQLiteTest)

enable_testing()
find_package(GTest REQUIRED)


list_header_directories_to_include(${TARGET_NAME_TEST}
        ${${TARGET_NAME}_HEADERS}
        )

include_directories(${${TARGET_NAME_TEST}_INCLUDE_DIRS}
        ${MEDINRIA_INCLUDE_DIRS}
        )


list_source_files(${TARGET_NAME_TEST}
        medSQLite.cpp
        test
        )

include_directories(
        ${MEDINRIA_INCLUDE_DIRS}
        ${GTEST_INCLUDE_DIR}
)


file(GLOB SRC_FILES "medSQLite.cpp")
file(GLOB TEST_SRC_FILES "test/*.cpp")
#message("${SRC_FILES}")
add_executable(${TARGET_NAME_TEST} ${SRC_FILES}  ${TEST_SRC_FILES})
target_link_libraries(${TARGET_NAME_TEST} ${GTEST_LIBRARY_DEBUG} ${GTEST_MAIN_LIBRARY_DEBUG} medCore ${TARGET_NAME})

#add_test(myTest medSQLiteTest)

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
