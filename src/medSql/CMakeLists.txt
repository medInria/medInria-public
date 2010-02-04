### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Fri Sep 25 11:51:59 2009 (+0200)
## Version: $Id$
## Last-Updated: Tue Jan 19 13:43:46 2010 (+0100)
##           By: Julien Wintz
##     Update #: 27
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medSql)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medDatabaseController.h
  medDatabaseExporter.h
  medDatabaseImporter.h
  medDatabaseItem.h
  medDatabaseModel.h
  medDatabaseNavigator.h
  medDatabaseNavigatorController.h
  medDatabaseNavigatorItem.h
  medDatabaseNavigatorItemGroup.h
  medDatabaseNavigatorItemLoader.h
  medDatabaseNavigatorScene.h
  medDatabaseNavigatorView.h
  medDatabasePreview.h
  medDatabasePreviewArrow.h
  medDatabasePreviewController.h
  medDatabasePreviewItem.h
  medDatabasePreviewItemGroup.h
  medDatabasePreviewItemLoader.h
  medDatabasePreviewScene.h
  medDatabasePreviewSelector.h
  medDatabasePreviewTooltip.h
  medDatabasePreviewView.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medDatabaseController.h
  medDatabaseExporter.h
  medDatabaseImporter.h
  medDatabaseModel.h
  medDatabaseView.h
  medDatabaseNavigator.h
  medDatabaseNavigatorController.h
  medDatabaseNavigatorItem.h
  medDatabaseNavigatorItemGroup.h
  medDatabaseNavigatorItemLoader.h
  medDatabaseNavigatorView.h
  medDatabasePreview.h
  medDatabasePreviewArrow.h
  medDatabasePreviewController.h
  medDatabasePreviewItem.h
  medDatabasePreviewItemGroup.h
  medDatabasePreviewItemLoader.h
  medDatabasePreviewSelector.h
  medDatabasePreviewTooltip.h
  medDatabasePreviewView.h
)

set(${PROJECT_NAME}_SOURCES
  medDatabaseController.cpp
  medDatabaseExporter.cpp
  medDatabaseImporter.cpp
  medDatabaseItem.cpp
  medDatabaseModel.cpp
  medDatabaseView.cpp
  medDatabaseNavigator.cpp
  medDatabaseNavigatorController.cpp
  medDatabaseNavigatorItem.cpp
  medDatabaseNavigatorItemGroup.cpp
  medDatabaseNavigatorItemLoader.cpp
  medDatabaseNavigatorScene.cpp
  medDatabaseNavigatorView.cpp
  medDatabasePreview.cpp
  medDatabasePreviewArrow.cpp
  medDatabasePreviewController.cpp
  medDatabasePreviewItem.cpp
  medDatabasePreviewItemGroup.cpp
  medDatabasePreviewItemLoader.cpp
  medDatabasePreviewScene.cpp
  medDatabasePreviewSelector.cpp
  medDatabasePreviewTooltip.cpp
  medDatabasePreviewView.cpp
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
  dtkCore
  medCore
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
