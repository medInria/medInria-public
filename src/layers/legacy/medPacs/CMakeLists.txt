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

set(TARGET_NAME medPacs)

## #############################################################################
## find requireds
## #############################################################################

# find dtk
find_package(dtk REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## Add definition
## #############################################################################

if (NOT MSVC) #TODO : what is it for ?
  add_definitions(-Wno-write-strings)
  add_definitions(-Wformat=0)
endif()


## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED)
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_CFILES} ${${TARGET_NAME}_QRC} )
target_sources(${TARGET_NAME} PRIVATE ${RELATIVE_${TARGET_NAME}_HEADERS})
set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${RELATIVE_${TARGET_NAME}_HEADERS}")


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core
    Qt5::Widgets
    
    med::Core
    
    # dtk
    dtkLog
    # Legacy
    dtkCoreSupport
    )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${dtk_INCLUDE_DIRS}
  PUBLIC
    "$<BUILD_INTERFACE:${${TARGET_NAME}_INCLUDE_DIRS}>"
    "$<INSTALL_INTERFACE:include/${TARGET_NAME}>"
  )


## #############################################################################
## install
## #############################################################################

#set_lib_install_rules(${TARGET_NAME} ${${TARGET_NAME}_HEADERS} )

set_lib_install_rules(${TARGET_NAME}
    NAMESPACE                  med
    VERSION                    4.0    
    LIBRARY_OUTPUT_NAME_PREFIX med
    DESTINATION                ${MEDINRIA_PATH}
    
    PUBLIC_DEPENDENCIES      
      "medInria REQUIRED COMPONENTS Core"  
      "dtk      REQUIRED COMPONENTS dtkLog dtkCoreSupport "
      "Qt5      REQUIRED COMPONENTS Core Widgets"
    )