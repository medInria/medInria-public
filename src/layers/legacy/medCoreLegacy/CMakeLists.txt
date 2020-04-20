################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(TARGET_NAME medCoreLegacy)

#required for querying the GPU, can be removed once using Qt 5.3
#using QOpenGLFunctions::glGetString(), as well as the {OPENGL_LIBRARIES} below.
find_package(OpenGL REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
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
  resources
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC ${${TARGET_NAME}_INCLUDE_DIRS}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  Qt5::Core
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Gui
  Qt5::Sql
  Qt5::Test
  ${OPENGL_LIBRARIES}
  dtkCoreSupport
  dtkGuiSupport
  dtkLog
  )

if (WIN32) #TODO is that for the memory management thing ?
  target_link_libraries(${TARGET_NAME} psapi.lib)
endif()


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )


## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
  #TODO: we will need to change the tests organization because of dependance to medTest...
  #add_subdirectory(gui/tests)
  #add_subdirectory(parameters/tests)
  #add_subdirectory(view/tests)
  #add_subdirectory(data/tests)
  #add_subdirectory(process/tests)
endif()
