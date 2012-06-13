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
  medLayoutChooser.h
  medPacsSelector.h
  medToolBoxContainer.h

  # viewer stuff
  medPacsSelector.h
  medViewPool.h
  medTabbedViewContainers.h
  medViewerConfiguration.h
  medImageFileLoader.h

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
  commonWidgets/medStatusBar.h
  commonWidgets/medQuickAccessMenu.h
  commonWidgets/medGroupBox.h

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
  settingsWidgets/medInteractionSettingsWidget.h

  # general toolbox classes
  toolboxes/medToolBox.h
  toolboxes/medToolBoxBody.h
  toolboxes/medToolBoxHeader.h
  toolboxes/medToolBoxTab.h

  # specific toolbox classes
  toolboxes/medToolBoxDiffusion.h
  toolboxes/medToolBoxDiffusionFiberView.h
  toolboxes/medToolBoxDiffusionFiberBundling.h
  toolboxes/medToolBoxRegistration.h
  toolboxes/medToolBoxRegistrationCustom.h
  toolboxes/medToolBoxSegmentation.h
  toolboxes/medToolBoxSegmentationCustom.h
  toolboxes/medToolBoxDiffusionTensorView.h
  toolboxes/medToolBoxFiltering.h
  toolboxes/medToolBoxFilteringCustom.h
  toolboxes/medBrowserToolBoxJobs.h
  toolboxes/medViewerToolBoxLayout.h
  toolboxes/medViewerToolBoxTime.h
  toolboxes/medViewerToolBoxViewProperties.h
  toolboxes/medBrowserToolBoxPacsHost.h
  toolboxes/medBrowserToolBoxPacsNodes.h
  toolboxes/medBrowserToolBoxPacsSearch.h
  toolboxes/medViewerToolBoxPatient.h
  toolboxes/medToolBoxCompositeDataSetImporter.h
  toolboxes/medToolBoxCompositeDataSetImporterCustom.h
  toolboxes/medToolBoxActions.h


  # viewContainers:
  viewContainers/medViewContainerCompare.h
  viewContainers/medViewContainerMulti.h
  viewContainers/medViewContainerCustom.h
  viewContainers/medViewContainerSingle.h
  viewContainers/medViewContainerFiltering.h
  viewContainers/medViewContainer.h
)

set(${PROJECT_NAME}_HEADERS
  #LUT
  LUT/medLUTToXMLWriter.h
  LUT/medXMLToLUTReader.h

  medGuiExport.h
  database/medDatabaseNavigatorController.h
  database/medDatabasePreviewController.h
  ${${PROJECT_NAME}_HEADERS_MOC}
  )

set(${PROJECT_NAME}_SOURCES
  medLayoutChooser.cpp
  medPacsSelector.cpp
  medToolBoxContainer.cpp
  medTabbedViewContainers.cpp
  medViewerConfiguration.cpp

  medPacsSelector.cpp
  medViewPool.cpp
  medImageFileLoader.cpp

  # commonWidgets:
  commonWidgets/medDropSite.cpp
  commonWidgets/medWorkspaceShifter.cpp
  commonWidgets/medButton.cpp
  commonWidgets/medProgressionStack.cpp
  commonWidgets/medFlicker.cpp
  commonWidgets/medStatusBar.cpp
  commonWidgets/medQuickAccessMenu.cpp
  commonWidgets/medGroupBox.cpp

  # db related items
  database/medDatabaseSearchPanel.cpp
  database/medDatabaseView.cpp
  database/medDatabaseNavigator.cpp
  database/medDatabaseNavigatorController.cpp
  database/medDatabaseNavigatorItem.cpp
  database/medDatabaseNavigatorItemGroup.cpp
  database/medDatabaseNavigatorScene.cpp
  database/medDatabaseNavigatorView.cpp
  database/medDatabasePreview.cpp
  database/medDatabasePreviewArrow.cpp
  database/medDatabasePreviewController.cpp
  database/medDatabasePreviewItem.cpp
  database/medDatabasePreviewItemGroup.cpp
  database/medDatabasePreviewScene.cpp
  database/medDatabasePreviewSelector.cpp
  database/medDatabasePreviewTooltip.cpp
  database/medDatabasePreviewView.cpp

  # factories
  factories/medSettingsWidgetFactory.cpp
  factories/medToolBoxFactory.cpp
  factories/medViewerConfigurationFactory.cpp

  # LUT
  LUT/medClutEditor.cpp
  LUT/medLoadLUTDialog.cpp
  LUT/medSaveLUTDialog.cpp
  LUT/medLUTToXMLWriter.cpp
  LUT/medXMLToLUTReader.cpp

  # settings classes
  settingsWidgets/medSettingsWidget.cpp
  settingsWidgets/medDatabaseSettingsWidget.cpp
  settingsWidgets/medSettingsEditor.cpp
  settingsWidgets/medSystemSettingsWidget.cpp
  settingsWidgets/medStartupSettingsWidget.cpp
  settingsWidgets/medInteractionSettingsWidget.cpp

  # general toolbox classes
  toolboxes/medToolBox.cpp
  toolboxes/medToolBoxBody.cpp
  toolboxes/medToolBoxHeader.cpp
  toolboxes/medToolBoxTab.cpp

  # specific toolbox classes
  toolboxes/medToolBoxDiffusion.cpp
  toolboxes/medToolBoxDiffusionCustom.cpp
  toolboxes/medToolBoxDiffusionFiberView.cpp
  toolboxes/medToolBoxDiffusionFiberBundling.cpp
  toolboxes/medToolBoxRegistration.cpp
  toolboxes/medToolBoxRegistrationCustom.cpp
  toolboxes/medToolBoxSegmentation.cpp
  toolboxes/medToolBoxSegmentationCustom.cpp
  toolboxes/medToolBoxDiffusionTensorView.cpp
  toolboxes/medToolBoxFiltering.cpp
  toolboxes/medToolBoxFilteringCustom.cpp
  toolboxes/medBrowserToolBoxJobs.cpp
  toolboxes/medViewerToolBoxLayout.cpp
  toolboxes/medViewerToolBoxTime.cpp
  toolboxes/medViewerToolBoxViewProperties.cpp
  toolboxes/medBrowserToolBoxPacsHost.cpp
  toolboxes/medBrowserToolBoxPacsNodes.cpp
  toolboxes/medBrowserToolBoxPacsSearch.cpp
  toolboxes/medViewerToolBoxPatient.cpp
  toolboxes/medToolBoxCompositeDataSetImporter.cpp
  toolboxes/medToolBoxCompositeDataSetImporterCustom.cpp
  toolboxes/medToolBoxActions.cpp


  # viewContainers:
  viewContainers/medViewContainerCompare.cpp
  viewContainers/medViewContainerMulti.cpp
  viewContainers/medViewContainerCustom.cpp
  viewContainers/medViewContainerSingle.cpp
  viewContainers/medViewContainerFiltering.cpp
  viewContainers/medViewContainer.cpp
)

set(${PROJECT_NAME}_RESOURCES medGui.qrc)

## #################################################################
## Build rules
## #################################################################
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPch.h" "../medPch/medPch.cpp"
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES_WRAP})
endif(MEDINRIA_USE_PRECOMPILED_HEADERS)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_QRC}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkGui
  dtkLog
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
