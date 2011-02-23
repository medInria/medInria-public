### CMakeLists.txt ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Mon Dec 20 13:33:18 2010 (+0100)
##           By: Julien Wintz
##     Update #: 167
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
  medDatabaseSettingsWidget.h
  medSettingsEditor.h
  medSystemSettingsWidget.h
  medStartupSettingsWidget.h
  
  medSettingsWidget.h
  medSettingsWidgetFactory.h
  
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medProgressionStack.h
  medStatusQuitButton.h
  medPacsSelector.h
  
  medToolBox.h
  medToolBoxBody.h
  medToolBoxHeader.h
  medToolBoxTab.h
  medToolBoxFactory.h
  medToolBoxContainer.h
  
  medViewPool.h
  medViewContainer.h
  medViewContainer_p.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medViewContainerStack.h
  medWorkspaceShifter.h
  medLUTtoXMLWriter.h
  medXMLtoLUTReader.h
  medSaveLUTDialog.h
  medLoadLUTDialog.h
  medViewerConfiguration.h  
  medViewerConfigurationFactory.h
  medViewerToolBoxLayout.h
  )

set(${PROJECT_NAME}_HEADERS_MOC
  medSettingsEditor.h
  medDatabaseSettingsWidget.h
  medSystemSettingsWidget.h
  medStartupSettingsWidget.h
  
  medSettingsWidget.h
  medSettingsWidgetFactory.h
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medProgressionStack.h
  medStatusQuitButton.h
  medPacsSelector.h
  
  medToolBox.h
  medToolBoxBody.h
  medToolBoxHeader.h
  medToolBoxTab.h
  medToolBoxFactory.h
  medToolBoxContainer.h
  
  medViewPool.h
  medViewContainer.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medViewContainerStack.h
  medWorkspaceShifter.h
  medSaveLUTDialog.h
  medLoadLUTDialog.h
  medViewerConfigurationFactory.h
  medViewerConfiguration.h
  medViewerToolBoxLayout.h  
  )

set(${PROJECT_NAME}_SOURCES
  medSettingsEditor.cpp
  medDatabaseSettingsWidget.cpp
  medStartupSettingsWidget.cpp
  medSystemSettingsWidget.cpp
  medSettingsWidget.cpp
  medSettingsWidgetFactory.cpp
  medClutEditor.cpp
  medDropSite.cpp
  medFlicker.cpp
  medLayoutChooser.cpp
  medProgressionStack.cpp
  medStatusQuitButton.cpp
  medPacsSelector.cpp
  
  medToolBox.cpp
  medToolBoxBody.cpp
  medToolBoxHeader.cpp
  medToolBoxTab.cpp
  medToolBoxFactory.cpp
  medToolBoxContainer.cpp 
  
  medViewPool.cpp
  medViewContainer.cpp
  medViewContainerCustom.cpp
  medViewContainerMulti.cpp
  medViewContainerSingle.cpp
  medViewContainerStack.cpp
  medWorkspaceShifter.cpp
  medLUTToXMLWriter.cpp
  medXMLToLUTReader.cpp
  medSaveLUTDialog.cpp
  medLoadLUTDialog.cpp
  medViewerConfigurationFactory.cpp
  medViewerConfiguration.cpp
  medViewerToolBoxLayout.cpp
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
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkGui
  medCore)

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
