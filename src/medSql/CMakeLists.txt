### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Fri Sep 25 11:51:59 2009 (+0200)
## Version: $Id$
## Last-Updated: Tue Jun 29 15:56:20 2010 (+0200)
##           By: Julien Wintz
##     Update #: 45
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
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medDatabaseController.h
  medDatabaseControllerImpl.h
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
  medDatabaseNonPersistentController.h
  medDatabaseNonPersistentControllerImpl.h
  medDatabaseNonPersistentItem.h
  medDatabaseNonPersistentReader.h
  medDatabaseNonPersistentImporter.h
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
  medDatabaseReader.h
  medDatabaseWriter.h
  )

set(${PROJECT_NAME}_HEADERS_MOC
  medDatabaseController.h
  medDatabaseControllerImpl.h
  medDatabaseExporter.h
  medDatabaseImporter.h
  medDatabaseModel.h
  medDatabaseView.h
  medDatabaseNavigator.h
  medDatabaseNavigatorItem.h
  medDatabaseNavigatorItemGroup.h
  medDatabaseNavigatorItemLoader.h
  medDatabaseNavigatorView.h
  medDatabaseNonPersistentController.h
  medDatabaseNonPersistentControllerImpl.h
  medDatabaseNonPersistentItem.h
  medDatabaseNonPersistentReader.h
  medDatabaseNonPersistentImporter.h
  medDatabasePreview.h
  medDatabasePreviewArrow.h
  medDatabasePreviewItem.h
  medDatabasePreviewItemGroup.h
  medDatabasePreviewItemLoader.h
  medDatabasePreviewSelector.h
  medDatabasePreviewTooltip.h
  medDatabasePreviewView.h
  medDatabaseReader.h
  medDatabaseWriter.h
  )

set(${PROJECT_NAME}_SOURCES
  medDatabaseController.cpp
  medDatabaseControllerImpl.cpp
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
  medDatabaseNonPersistentController.cpp
  medDatabaseNonPersistentControllerImpl.cpp
  medDatabaseNonPersistentItem.cpp
  medDatabaseNonPersistentReader.cpp
  medDatabaseNonPersistentImporter.cpp
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
  medDatabaseReader.cpp
  medDatabaseWriter.cpp
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
  dtkCore
  medCore)

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
