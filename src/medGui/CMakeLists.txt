### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed Nov 11 15:34:57 2009 (+0100)
##           By: Julien Wintz
##     Update #: 79
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medGui)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)

include(${dtk_USE_FILE})

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medClutEditor.h
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImageList.h
  medImagePreview.h
  medImageStack.h
  medImageReflector.h
  medPatientPreview.h
  medSeriesPreview.h
  medStatusPanel.h
  medStudyPreview.h
  medStyle.h
  medToolBox.h
  medToolBoxContainer.h
  medViewContainer.h
  medWorkspaceSwitcher.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medClutEditor.h
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImageList.h
  medImagePreview.h
  medImageReflector.h
  medImageStack.h
  medPatientPreview.h
  medSeriesPreview.h
  medStatusPanel.h
  medStudyPreview.h
  medStyle.h
  medToolBox.h
  medViewContainer.h
  medWorkspaceSwitcher.h
)

set(${PROJECT_NAME}_SOURCES
  medClutEditor.cpp
  medLayoutChooser.cpp
  medLoginWidget.cpp
  medImageFlow.cpp
  medImageList.cpp
  medImagePreview.cpp
  medImageReflector.cpp
  medImageStack.cpp
  medPatientPreview.cpp
  medSeriesPreview.cpp
  medStatusPanel.cpp
  medStudyPreview.cpp
  medStyle.cpp
  medToolBox.cpp
  medToolBoxContainer.cpp
  medViewContainer.cpp
  medWorkspaceSwitcher.cpp
)

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
  medCore
  medSql
)

## #################################################################
## Installation
## #################################################################

if(NOT med_INSTALL_NO_DEVELOPMENT)
  install_files(${med_INSTALL_INCLUDE_DIR}/${PROJECT_NAME} .h ${${PROJECT_NAME}_HEADERS})
endif(NOT med_INSTALL_NO_DEVELOPMENT)

if(NOT med_INSTALL_NO_LIBRARIES)
  install_targets(${med_INSTALL_LIB_DIR} RUNTIME_DIRECTORY ${med_INSTALL_BIN_DIR} ${PROJECT_NAME})
endif(NOT med_INSTALL_NO_LIBRARIES)
