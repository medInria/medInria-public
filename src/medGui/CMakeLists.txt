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

set(${PROJECT_NAME}_HEADERS_MOC
  
  # general setting classes  
  medSettingsWidget.h
  medSettingsWidgetFactory.h
  
  # specific setting classes
  medDatabaseSettingsWidget.h
  medSettingsEditor.h
  medSystemSettingsWidget.h
  medStartupSettingsWidget.h
 
  medBrowserToolBoxJobs.h
  medClutEditor.h
  medDropSite.h
  medFlicker.h
  medLayoutChooser.h
  medProgressionStack.h
  medStatusQuitButton.h
  medSettingsButton.h
  medPacsSelector.h
  
  # general toolbox classes
  medToolBox.h
  medToolBoxBody.h
  medToolBoxHeader.h
  medToolBoxTab.h
  medToolBoxFactory.h
  medToolBoxContainer.h
  
  # specific toolbox classes
  medToolBoxDiffusion.h
  medToolBoxDiffusionCustom.h
  medToolBoxDiffusionFiberView.h   
  medToolBoxDiffusionFiberBundling.h
  medToolBoxRegistration.h
  medToolBoxRegistrationCustom.h
  medToolBoxDiffusionTensorView.h
  
  # viewer stuff
  medViewPool.h
  medViewContainer.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medViewContainerCompare.h
  medStackedViewContainers.h
  
  medWorkspaceShifter.h 
  medSaveLUTDialog.h
  medLoadLUTDialog.h
  
  # general configuration classes
  medViewerConfiguration.h  
  medViewerConfigurationFactory.h
  medViewerToolBoxLayout.h
  medViewerToolBoxViewProperties.h
  medViewerToolBoxView.h
  
  # pacs toolboxes
  medBrowserToolBoxPacsHost.h
  medBrowserToolBoxPacsNodes.h
  medBrowserToolBoxPacsSearch.h
 
  )
set(${PROJECT_NAME}_HEADERS
  medLUTtoXMLWriter.h
  medXMLtoLUTReader.h
  medViewContainer_p.h
  ${${PROJECT_NAME}_HEADERS_MOC}
  )


set(${PROJECT_NAME}_SOURCES
  medSettingsWidget.cpp
  medSettingsWidgetFactory.cpp 
  
  medSettingsEditor.cpp
  medDatabaseSettingsWidget.cpp
  medStartupSettingsWidget.cpp
  medSystemSettingsWidget.cpp

  medBrowserToolBoxJobs.cpp
  medClutEditor.cpp
  medDropSite.cpp
  medFlicker.cpp
  medLayoutChooser.cpp
  medProgressionStack.cpp
  medStatusQuitButton.cpp
  medSettingsButton.cpp
  medPacsSelector.cpp
  
  medToolBox.cpp
  medToolBoxBody.cpp
  medToolBoxHeader.cpp
  medToolBoxTab.cpp
  medToolBoxFactory.cpp
  medToolBoxContainer.cpp 
  
  medToolBoxDiffusion.cpp
  medToolBoxDiffusionCustom.cpp
  medToolBoxDiffusionFiberView.cpp   
  medToolBoxDiffusionFiberBundling.cpp
  medToolBoxRegistration.cpp
  medToolBoxRegistrationCustom.cpp
  medToolBoxDiffusionTensorView.cpp
  
  medViewPool.cpp
  medViewContainer.cpp
  medViewContainerCustom.cpp
  medViewContainerMulti.cpp
  medViewContainerSingle.cpp
  medViewContainerCompare.cpp
  medStackedViewContainers.cpp
  medWorkspaceShifter.cpp
  medLUTToXMLWriter.cpp
  medXMLToLUTReader.cpp
  medSaveLUTDialog.cpp
  medLoadLUTDialog.cpp
  
  medViewerConfigurationFactory.cpp
  medViewerConfiguration.cpp
  medViewerToolBoxLayout.cpp
  medViewerToolBoxViewProperties.cpp
  medViewerToolBoxView.cpp
  
  medBrowserToolBoxPacsHost.cpp
  medBrowserToolBoxPacsNodes.cpp
  medBrowserToolBoxPacsSearch.cpp    
  )

# Include header files for Visual Studio.
FILE(GLOB ${PROJECT_NAME}_HEADERS_ALL *.h )
set(${PROJECT_NAME}_SOURCES
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS_MOC}
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
