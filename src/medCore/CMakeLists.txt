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

project(medCore)


## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  data
  database
  data/annotationData
  gui
  gui/commonWidgets
  gui/database
  gui/factories
  gui/lookUpTables
  gui/settingsWidgets
  gui/toolboxes
  gui/viewContainers
  process
  views
  views/interactors
  views/navigators
  parameters
  transformations
  dataSources
  )

set(${PROJECT_NAME}_QRC resources/medCore.qrc)

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})


## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPCH_H}
    ${MEDPCH_CPP}
    ${${PROJECT_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################

qt4_add_resources(${PROJECT_NAME}_QRC
  ${${PROJECT_NAME}_QRC}
  )

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_CFILES}
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################
set(LINK_LIBRARIES
    dtkCore
    dtkGui
    dtkLog
    )

if(DTK_BUILD_COMPOSER)
    set(LINK_LIBRARIES
        dtkComposer
        ${LINK_LIBRARIES}
        )
endif()

target_link_libraries(${PROJECT_NAME}
    ${QT_LIBRARIES}
    ${LINK_LIBRARIES}
    )

if (WIN32) #TODO is that for the memory management thing ?
  target_link_libraries(${PROJECT_NAME} psapi.lib)
endif()


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )


## #############################################################################
## Build tests
## #############################################################################

if(MEDINRIA_BUILD_TESTS)
  add_subdirectory(tests)
  #TODO: we will need to change the tests organization because of dependance to medTest...
  #add_subdirectory(gui/tests)
  #add_subdirectory(parameters/tests)
  #add_subdirectory(view/tests)
  #add_subdirectory(data/tests)
  #add_subdirectory(process/tests)
endif()
