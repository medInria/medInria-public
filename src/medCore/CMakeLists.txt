### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Sat Oct  3 18:22:21 2009 (+0200)
## Version: $Id$
## Last-Updated: Sat Oct  3 18:38:00 2009 (+0200)
##           By: Julien Wintz
##     Update #: 12
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medCore)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medAbstractDataImage.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medAbstractDataImage.h
)

set(${PROJECT_NAME}_SOURCES
  medAbstractDataImage.cpp
)

## #################################################################
## Wrapping
## #################################################################

set(${PROJECT_NAME}_WRAP_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/medAbstractDataImage.h)

dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medcore python ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i ${${PROJECT_NAME}_WRAP_DEPENDS})
dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medcore tcl    ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i ${${PROJECT_NAME}_WRAP_DEPENDS})

## #################################################################
## Build rules
## #################################################################

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_WRAP}
)

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
)

## #################################################################
## Handling of generated script modules
## #################################################################

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/medCore.i
  COMMAND ${CMAKE_COMMAND} ARGS -E  make_directory ${CMAKE_BINARY_DIR}/modules
  COMMAND ${CMAKE_COMMAND} ARGS -E  copy ${${PROJECT_NAME}_BINARY_DIR}/medcore.py ${CMAKE_BINARY_DIR}/modules
  COMMENT "-- Moving python modules to ${CMAKE_BINARY_DIR}/modules"
)

## #################################################################
## Installation
## #################################################################

if(NOT dtk_INSTALL_NO_DEVELOPMENT)
  install_files(${dtk_INSTALL_INCLUDE_DIR}/${PROJECT_NAME} .h ${${PROJECT_NAME}_HEADERS})
endif(NOT dtk_INSTALL_NO_DEVELOPMENT)

if(NOT dtk_INSTALL_NO_LIBRARIES)
  install_targets(${dtk_INSTALL_LIB_DIR} RUNTIME_DIRECTORY ${dtk_INSTALL_BIN_DIR} ${PROJECT_NAME})
endif(NOT dtk_INSTALL_NO_LIBRARIES)
