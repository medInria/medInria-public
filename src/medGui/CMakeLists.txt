### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed Nov 10 11:12:36 2010 (+0100)
##           By: Julien Wintz
##     Update #: 155
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
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medProgressionStack.h
  medStatusQuitButton.h
  medToolBox.h
  medToolBoxBody.h
  medToolBoxHeader.h
  medToolBoxTab.h
  medToolBoxContainer.h
  medToolBoxLayout.h
  medToolBoxPacsHost.h
  medToolBoxPacsNodes.h
  medToolBoxPacsSearch.h
  medToolBoxPatient.h
  medToolBoxRegistration.h
  medToolBoxSource.h
  medToolBoxView.h
  medViewPool.h
  medViewContainer.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medWorkspaceShifter.h)

set(${PROJECT_NAME}_HEADERS_MOC
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medProgressionStack.h
  medStatusQuitButton.h
  medToolBox.h
  medToolBoxBody.h
  medToolBoxHeader.h
  medToolBoxTab.h
  medToolBoxLayout.h
  medToolBoxPacsHost.h
  medToolBoxPacsNodes.h
  medToolBoxPacsSearch.h
  medToolBoxPatient.h
  medToolBoxRegistration.h
  medToolBoxSource.h
  medToolBoxView.h
  medViewPool.h
  medViewContainer.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medWorkspaceShifter.h)

set(${PROJECT_NAME}_SOURCES
  medClutEditor.cpp
  medDropSite.cpp
  medFlicker.cpp
  medLayoutChooser.cpp
  medProgressionStack.cpp
  medStatusQuitButton.cpp
  medToolBox.cpp
  medToolBoxBody.cpp
  medToolBoxHeader.cpp
  medToolBoxTab.cpp
  medToolBoxContainer.cpp
  medToolBoxLayout.cpp
  medToolBoxPacsHost.cpp
  medToolBoxPacsNodes.cpp
  medToolBoxPacsSearch.cpp
  medToolBoxPatient.cpp
  medToolBoxRegistration.cpp
  medToolBoxSource.cpp
  medToolBoxView.cpp
  medViewPool.cpp
  medViewContainer.cpp
  medViewContainerCustom.cpp
  medViewContainerMulti.cpp
  medViewContainerSingle.cpp
  medWorkspaceShifter.cpp)

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
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  medCore
  medPacs
  medSql)

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
