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
  medViewerWorkspace.h
  medImageFileLoader.h

  # db related items
  database/medDatabaseSearchPanel.h
  database/medDatabaseView.h
  database/medDatabaseNavigator.h
  database/medDatabaseNavigatorItem.h
  database/medDatabaseNavigatorItemOverlay.h
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
  commonWidgets/medMultipleImageSelectionWidget.h
  commonWidgets/medThumbnailContainer.h

  # factories
  factories/medSettingsWidgetFactory.h
  factories/medToolBoxFactory.h
  factories/medWorkspaceFactory.h

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
  toolboxes/medDiffusionAbstractToolBox.h
  toolboxes/medToolBoxDiffusionFiberView.h
  toolboxes/medFiberBundlingToolBox.h
  toolboxes/medRegistrationSelectorToolBox.h
  toolboxes/medRegistrationAbstractToolBox.h
  toolboxes/medSegmentationSelectorToolBox.h
  toolboxes/medSegmentationAbstractToolBox.h
  toolboxes/medToolBoxDiffusionTensorView.h
  toolboxes/medFilteringSelectorToolBox.h
  toolboxes/medFilteringAbstractToolBox.h
  toolboxes/medBrowserJobsToolBox.h
  toolboxes/medVisualizationLayoutToolBox.h
  toolboxes/medWorkspaceTimeToolBox.h
  toolboxes/medWorkspaceViewPropertiesToolBox.h
  toolboxes/medBrowserPacsHostToolBox.h
  toolboxes/medBrowserPacsNodesToolBox.h
  toolboxes/medBrowserPacsSearchToolBox.h
  toolboxes/medWorkspacePatientToolBox.h
  toolboxes/medToolBoxCompositeDataSetImporter.h
  toolboxes/medToolBoxCompositeDataSetImporterCustom.h
  toolboxes/medActionsToolBox.h


  # viewContainers:
  viewContainers/medCompareViewContainer.h
  viewContainers/medMultiViewContainer.h
  viewContainers/medCustomViewContainer.h
  viewContainers/medSingleViewContainer.h
  viewContainers/medFilteringViewContainer.h
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
  medViewerWorkspace.cpp

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
  commonWidgets/medMultipleImageSelectionWidget.cpp
  commonWidgets/medThumbnailContainer.cpp

  # db related items
  database/medDatabaseSearchPanel.cpp
  database/medDatabaseView.cpp
  database/medDatabaseNavigator.cpp
  database/medDatabaseNavigatorController.cpp
  database/medDatabaseNavigatorItem.cpp
  database/medDatabaseNavigatorItemOverlay.cpp
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
  factories/medWorkspaceFactory.cpp

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
  toolboxes/medDiffusionAbstractToolBox.cpp
  toolboxes/medToolBoxDiffusionFiberView.cpp
  toolboxes/medFiberBundlingToolBox.cpp
  toolboxes/medRegistrationSelectorToolBox.cpp
  toolboxes/medRegistrationAbstractToolBox.cpp
  toolboxes/medSegmentationSelectorToolBox.cpp
  toolboxes/medSegmentationAbstractToolBox.cpp
  toolboxes/medToolBoxDiffusionTensorView.cpp
  toolboxes/medFilteringSelectorToolBox.cpp
  toolboxes/medFilteringAbstractToolBox.cpp
  toolboxes/medBrowserJobsToolBox.cpp
  toolboxes/medVisualizationLayoutToolBox.cpp
  toolboxes/medWorkspaceTimeToolBox.cpp
  toolboxes/medWorkspaceViewPropertiesToolBox.cpp
  toolboxes/medBrowserPacsHostToolBox.cpp
  toolboxes/medBrowserPacsNodesToolBox.cpp
  toolboxes/medBrowserPacsSearchToolBox.cpp
  toolboxes/medWorkspacePatientToolBox.cpp
  toolboxes/medToolBoxCompositeDataSetImporter.cpp
  toolboxes/medToolBoxCompositeDataSetImporterCustom.cpp
  toolboxes/medActionsToolBox.cpp


  # viewContainers:
  viewContainers/medCompareViewContainer.cpp
  viewContainers/medMultiViewContainer.cpp
  viewContainers/medCustomViewContainer.cpp
  viewContainers/medSingleViewContainer.cpp
  viewContainers/medFilteringViewContainer.cpp
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
  medCore
)

# Include header files for Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

if (BUILD_TESTING)
  add_subdirectory (Testing)
endif()

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
