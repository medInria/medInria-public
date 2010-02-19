### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Fri Feb 19 18:22:59 2010 (+0100)
##           By: Julien Wintz
##     Update #: 109
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
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medLoginWidget.h
  medProgressionStack.h
  medRegistrationWidget.h
  medSearchBar.h
  medSearchBox.h
  medToolBox.h
  medToolBoxContainer.h
  medToolBoxConfiguration.h
  medToolBoxDiffusion.h
  medToolBoxLayout.h
  medToolBoxPatient.h
  medToolBoxRegistration.h
  medToolBoxView.h
  medViewContainer.h
  medWorkspaceShifter.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medLoginWidget.h
  medProgressionStack.h
  medRegistrationWidget.h
  medSearchBar.h
  medSearchBox.h
  medToolBox.h
  medToolBoxConfiguration.h
  medToolBoxDiffusion.h
  medToolBoxLayout.h
  medToolBoxPatient.h
  medToolBoxRegistration.h
  medToolBoxView.h
  medViewContainer.h
  medWorkspaceShifter.h
)

set(${PROJECT_NAME}_SOURCES
  medClutEditor.cpp
  medDropSite.cpp
  medFlicker.cpp
  medLayoutChooser.cpp
  medLoginWidget.cpp
  medProgressionStack.cpp
  medRegistrationWidget.cpp
  medSearchBar.cpp
  medSearchBox.cpp
  medToolBox.cpp
  medToolBoxContainer.cpp
  medToolBoxConfiguration.cpp
  medToolBoxDiffusion.cpp
  medToolBoxLayout.cpp
  medToolBoxPatient.cpp
  medToolBoxRegistration.cpp
  medToolBoxView.cpp
  medViewContainer.cpp
  medWorkspaceShifter.cpp
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
