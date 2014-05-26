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

project(medPluginGenerator)

## #############################################################################
## Version
## #############################################################################

set(${PROJECT_NAME}_VERSION ${MEDINRIA_VERSION})

## #############################################################################
## Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

set(${PROJECT_NAME}_UI
  medPluginGeneratorMainWindow.ui
  )

set(${PROJECT_NAME}_QRC
  template/template.qrc
  )


## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADERS}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

## #############################################################################
## Add Exe
## #############################################################################

qt4_wrap_ui(${PROJECT_NAME}_UI ${${PROJECT_NAME}_UI})
qt4_add_resources(${PROJECT_NAME}_QRC ${${PROJECT_NAME}_QRC})

if (APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()

add_executable(${PROJECT_NAME} ${DEPLOYMENT_SYSTEM}
  ${${PROJECT_NAME}_UI}
  ${${PROJECT_NAME}_QRC}
  ${${PROJECT_NAME}_CFILES}
  )


## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  )


## #############################################################################
## install
## #############################################################################

set_exe_install_rules(${PROJECT_NAME})


## #############################################################################
## Build tests
## #############################################################################

if(MEDINRIA_BUILD_TESTS)
  add_subdirectory(tests)
endif()
