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

set(TARGET_NAME medAPHPPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${TARGET_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${TARGET_NAME} TARGET_NAME_UP)
add_definitions(-D${TARGET_NAME_UP}_VERSION="${${TARGET_NAME}_VERSION}")


## #############################################################################
## Resolve dependencies
## #############################################################################

#enable_testing(true)
#FIND_PACKAGE(GTest REQUIRED)
#FIND_PACKAGE(Qt${QT_VERSION_MAJOR}Test REQUIRED)

find_package(DCMTK REQUIRED)

find_package(QtDCM REQUIRED)

find_package(ITK REQUIRED)

find_package(dtk REQUIRED)
include_directories(${dtk_INCLUDE_DIRS})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  sphereDicomWeb
 )

## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
   ${${TARGET_NAME}_HEADERS}
   )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  ${QTDCM_INCLUDE_DIRS}
  ${QTDCM_DIR}
  ${DCMTK_INCLUDE_DIR}
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
link_directories(${QTDCM_LIBRARY_DIR})

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_PCH}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
target_link_libraries(${TARGET_NAME}
  ${QTDCM_LIBRARIES}
  ${DCMTK_LIBRARIES}
  medCore
  )


## #############################################################################
## Tests
## #############################################################################

## #############################################################################
## GTEST
## #############################################################################
#set(TARGET_NAME_TEST ${TARGET_NAME}GTest)
#
#add_executable(${TARGET_NAME_TEST})
#target_sources(${TARGET_NAME_TEST} PUBLIC medAPHP.cpp sphereDicomWeb/medAnnotation.cpp test/medAPHPGTest.cpp )
#target_include_directories(${TARGET_NAME_TEST} PUBLIC "." "./test/")
#set_target_properties(${TARGET_NAME_TEST} PROPERTIES COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1")
#target_link_libraries(${TARGET_NAME_TEST} PUBLIC
#        GTest::gtest_main
#        GTest::gtest
#        GTest::gmock_main
#        GTest::gmock
#        medCore
#        ${TARGET_NAME}
#        ${QTDCM_LIBRARIES}
#        ${DCMTK_LIBRARIES}
#        )
#
### #############################################################################
### QTEST
### #############################################################################
#set(TARGET_NAME_QTEST ${TARGET_NAME}QTest)
#add_executable(${TARGET_NAME_QTEST})
#target_sources(${TARGET_NAME_QTEST} PUBLIC medAPHP.cpp sphereDicomWeb/medAnnotation.cpp test/medAPHPQTest.cpp)
#target_include_directories(${TARGET_NAME_QTEST} PUBLIC "." "./test/")
#target_link_libraries(${TARGET_NAME_QTEST} PUBLIC
#        Qt${QT_VERSION_MAJOR}::Test
#        medCore
#        ${TARGET_NAME}
#        ${QTDCM_LIBRARIES}
#        ${DCMTK_LIBRARIES}
#        )

## #############################################################################
## Install rules
## #############################################################################

set_plugin_install_rules_legacy(${TARGET_NAME})
