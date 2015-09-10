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

set(TARGET_NAME medPluginGenerator)

## #############################################################################
## Version
## #############################################################################

set(${TARGET_NAME}_VERSION ${${PROJECT_NAME}_VERSION})

## #############################################################################
## Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

set(${TARGET_NAME}_UI
  medPluginGeneratorMainWindow.ui
  )

set(${TARGET_NAME}_QRC
  template/template.qrc
  )


## #############################################################################
## include directorie.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
  )

include_directories(${${TARGET_NAME}_INCLUDE_DIRS}
                    ${QT_HEADERS})

## #############################################################################
## Add Exe
## #############################################################################

qt5_wrap_ui(${TARGET_NAME}_UI ${${TARGET_NAME}_UI})
qt5_add_resources(${TARGET_NAME}_QRC ${${TARGET_NAME}_QRC})

if (APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()

add_executable(${TARGET_NAME} ${DEPLOYMENT_SYSTEM}
  ${${TARGET_NAME}_UI}
  ${${TARGET_NAME}_QRC}
  ${${TARGET_NAME}_CFILES}
  )


## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  )


## #############################################################################
## install
## #############################################################################

set_exe_install_rules(${TARGET_NAME})


## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
endif()
