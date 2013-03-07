#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

project(medGui)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_DEVEL_HEADERS_MOC
  commonWidgets/medProgressionStack.h
  factories/medToolBoxFactory.h
  toolboxes/medFilteringAbstractToolBox.h
  toolboxes/medFilteringSelectorToolBox.h
  toolboxes/medRegistrationAbstractToolBox.h
  toolboxes/medRegistrationSelectorToolBox.h
  toolboxes/medSegmentationAbstractToolBox.h
  toolboxes/medSegmentationSelectorToolBox.h
  toolboxes/medToolBox.h
)

set(${PROJECT_NAME}_DEVEL_HEADERS
  medGuiExport.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  ${${PROJECT_NAME}_DEVEL_HEADERS_MOC}
  medLayoutChooser.h
  medPacsSelector.h
  medToolBoxContainer.h

  # viewer stuff
  medPacsSelector.h
  medViewPool.h
  medTabbedViewContainers.h
  medWorkspace.h
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
  database/medDatabaseEditItemDialog.h

  # commonWidgets:
  commonWidgets/medDropSite.h
  commonWidgets/medWorkspaceShifter.h
  commonWidgets/medButton.h
  commonWidgets/medFlicker.h
  commonWidgets/medStatusBar.h
  commonWidgets/medQuickAccessMenu.h
  commonWidgets/medGroupBox.h
  commonWidgets/medMultipleImageSelectionWidget.h
  commonWidgets/medThumbnailContainer.h
  commonWidgets/medHomepageButton.h

  # factories
  factories/medSettingsWidgetFactory.h
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
  toolboxes/medToolBoxBody.h
  toolboxes/medToolBoxHeader.h
  toolboxes/medToolBoxTab.h

  # specific toolbox classes
  toolboxes/medDiffusionSelectorToolBox.h
  toolboxes/medDiffusionAbstractToolBox.h
  toolboxes/medFiberViewToolBox.h
  toolboxes/medFiberBundlingToolBox.h
  toolboxes/medTensorViewToolBox.h

  toolboxes/medToolBoxDiffusionShView.h
  toolboxes/medFilteringSelectorToolBox.h
  toolboxes/medFilteringAbstractToolBox.h
  toolboxes/medBrowserJobsToolBox.h
  toolboxes/medVisualizationLayoutToolBox.h
  toolboxes/medTimeLineToolBox.h
  toolboxes/medViewPropertiesToolBox.h
  toolboxes/medBrowserPacsHostToolBox.h
  toolboxes/medBrowserPacsNodesToolBox.h
  toolboxes/medBrowserPacsSearchToolBox.h
  toolboxes/medPatientSelectorToolBox.h
  toolboxes/medCompositeDataSetImporterSelectorToolBox.h
  toolboxes/medCompositeDataSetImporterAbstractToolBox.h
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
  ${${PROJECT_NAME}_DEVEL_HEADERS}
  #LUT
  LUT/medLUTToXMLWriter.h
  LUT/medXMLToLUTReader.h

  database/medDatabaseNavigatorController.h
  database/medDatabasePreviewController.h
  ${${PROJECT_NAME}_HEADERS_MOC}
  )

set(${PROJECT_NAME}_SOURCES
  medLayoutChooser.cpp
  medPacsSelector.cpp
  medToolBoxContainer.cpp
  medTabbedViewContainers.cpp
  medWorkspace.cpp

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
  commonWidgets/medHomepageButton.cpp

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
  database/medDatabaseEditItemDialog.cpp

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
  toolboxes/medDiffusionSelectorToolBox.cpp
  toolboxes/medDiffusionAbstractToolBox.cpp
  toolboxes/medFiberViewToolBox.cpp
  toolboxes/medFiberBundlingToolBox.cpp
  toolboxes/medRegistrationSelectorToolBox.cpp
  toolboxes/medRegistrationAbstractToolBox.cpp
  toolboxes/medSegmentationSelectorToolBox.cpp
  toolboxes/medSegmentationAbstractToolBox.cpp
  toolboxes/medTensorViewToolBox.cpp
  toolboxes/medToolBoxDiffusionShView.cpp
  toolboxes/medFilteringSelectorToolBox.cpp
  toolboxes/medFilteringAbstractToolBox.cpp
  toolboxes/medBrowserJobsToolBox.cpp
  toolboxes/medVisualizationLayoutToolBox.cpp
  toolboxes/medTimeLineToolBox.cpp
  toolboxes/medViewPropertiesToolBox.cpp
  toolboxes/medBrowserPacsHostToolBox.cpp
  toolboxes/medBrowserPacsNodesToolBox.cpp
  toolboxes/medBrowserPacsSearchToolBox.cpp
  toolboxes/medPatientSelectorToolBox.cpp
  toolboxes/medCompositeDataSetImporterSelectorToolBox.cpp
  toolboxes/medCompositeDataSetImporterAbstractToolBox.cpp
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

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPch.h" "../medPch/medPch.cpp"
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES_WRAP})
endif()

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
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

if (MEDINRIA_BUILD_TESTING)
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
