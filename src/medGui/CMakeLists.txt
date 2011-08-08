### CMakeLists.txt ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed May  4 14:43:36 2011 (+0200)
##           By: Julien Wintz
##     Update #: 168
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
  medPacsSelector.h
  medButton.h  
  
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
  medToolBoxFilteringCustom.h
  medToolBoxFiltering.h
  medToolBoxDiffusionTensorView.h
  
  # viewer stuff
  medViewPool.h
  medViewContainer.h
  medViewContainerCustom.h
  medViewContainerMulti.h
  medViewContainerSingle.h
  medViewContainerCompare.h
  medStackedViewContainers.h
  medViewContainerFiltering.h
  
  medWorkspaceShifter.h 
  medSaveLUTDialog.h
  medLoadLUTDialog.h
  
  # general configuration classes
  medViewerConfiguration.h  
  medViewerConfigurationFactory.h
  medViewerToolBoxLayout.h
  medViewerToolBoxView.h
  medViewerToolBoxTime.h
  medViewerToolBoxViewProperties.h
  medViewerToolBoxView.h
  
  # pacs toolboxes
  medBrowserToolBoxPacsHost.h
  medBrowserToolBoxPacsNodes.h
  medBrowserToolBoxPacsSearch.h    
  
  medDatabaseSearchPanel.h

  # db related items
  database/medDatabaseSearchPanel.h  
  database/medDatabaseView.h
  database/medDatabaseNavigator.h  
  database/medDatabaseNavigatorItem.h
  database/medDatabaseNavigatorItemGroup.h
  database/medDatabaseNavigatorView.h
  database/medDatabasePreview.h
  database/medDatabasePreviewArrow.h
  database/medDatabasePreviewItem.h
  database/medDatabasePreviewItemGroup.h
  database/medDatabasePreviewSelector.h
  database/medDatabasePreviewTooltip.h
  database/medDatabasePreviewView.h

  # commonWidgets:
  commonWidgets/medDropSite.h
  commonWidgets/medWorkspaceShifter.h
  commonWidgets/medButton.h
  commonWidgets/medProgressionStack.h
  commonWidgets/medFlicker.h

  # factories
  factories/medSettingsWidgetFactory.h
  factories/medToolBoxFactory.h
  factories/medViewerConfigurationFactory.h

  # LUT
  LUT/medClutEditor.h
  LUT/medLoadLUTDialog.h
  LUT/medSaveLUTDialog.h

  # settings classes
  settingsWidgets/medSettingsWidget.h
  settingsWidgets/medDatabaseSettingsWidget.h
  settingsWidgets/medSettingsEditor.h
  settingsWidgets/medSystemSettingsWidget.h
  settingsWidgets/medStartupSettingsWidget.h
>>>>>>> master-visages


  # general toolbox classes
  toolboxes/medToolBox.h
  toolboxes/medToolBoxBody.h
  toolboxes/medToolBoxHeader.h
  toolboxes/medToolBoxTab.h

  # specific toolbox classes
  toolboxes/medToolBoxDiffusion.h
  toolboxes/medToolBoxDiffusionCustom.h
  toolboxes/medToolBoxDiffusionFiberView.h
  toolboxes/medToolBoxDiffusionFiberBundling.h
  toolboxes/medToolBoxRegistration.h
  toolboxes/medToolBoxRegistrationCustom.h
  toolboxes/medToolBoxDiffusionTensorView.h
  toolboxes/medBrowserToolBoxJobs.h
  toolboxes/medViewerToolBoxLayout.h
  toolboxes/medViewerToolBoxView.h
  toolboxes/medViewerToolBoxTime.h
  toolboxes/medViewerToolBoxViewProperties.h
  toolboxes/medBrowserToolBoxPacsHost.h
  toolboxes/medBrowserToolBoxPacsNodes.h
  toolboxes/medBrowserToolBoxPacsSearch.h
  toolboxes/medViewerToolBoxPatient.h

  # viewContainers:
  viewContainers/medViewContainerCompare.h
  viewContainers/medViewContainerMulti.h
  viewContainers/medViewContainerCustom.h
  viewContainers/medViewContainerSingle.h
  viewContainers/medViewContainer.h
)

set(${PROJECT_NAME}_HEADERS
  medLUTtoXMLWriter.h
  medXMLtoLUTReader.h
  medViewContainer_p.h
  medGuiExport.h
  database/medDatabaseNavigatorController.h
  database/medDatabasePreviewController.h
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
  medPacsSelector.cpp
  medButton.cpp
  
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
  medToolBoxFilteringCustom.cpp
  medToolBoxFiltering.cpp
  medToolBoxDiffusionTensorView.cpp
  
  medViewPool.cpp
  medViewContainer.cpp
  medViewContainerCustom.cpp
  medViewContainerMulti.cpp
  medViewContainerSingle.cpp
  medViewContainerCompare.cpp
  medViewContainerFiltering.cpp
  medStackedViewContainers.cpp
  medWorkspaceShifter.cpp
  medLUTToXMLWriter.cpp
  medXMLToLUTReader.cpp
  medSaveLUTDialog.cpp
  medLoadLUTDialog.cpp
  
  medViewerConfiguration.cpp
  medViewerConfigurationFactory.cpp
  medViewerToolBoxLayout.cpp
  medViewerToolBoxTime.cpp
  medViewerToolBoxViewProperties.cpp
  medViewerToolBoxView.cpp
  
  medBrowserToolBoxPacsHost.cpp
  medBrowserToolBoxPacsNodes.cpp
  medBrowserToolBoxPacsSearch.cpp

  medDatabaseSearchPanel.cpp 
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
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_QRC}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkGui
  medCore)

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
