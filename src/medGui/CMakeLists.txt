### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Fri Oct 16 11:43:58 2009 (+0200)
##           By: Julien Wintz
##     Update #: 64
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
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImageList.h
  medImagePreview.h
  medImageStack.h
  medImageReflector.h
  medPatientPreview.h
  medSeriesPreview.h
  medStatusPanel.h
  medStudyPreview.h
  medStyle.h
  medToolBox.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medClutEditor.h
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImageList.h
  medImagePreview.h
  medImageReflector.h
  medImageStack.h
  medPatientPreview.h
  medSeriesPreview.h
  medStatusPanel.h
  medStudyPreview.h
  medStyle.h
  medToolBox.h
)

set(${PROJECT_NAME}_SOURCES
  medClutEditor.cpp
  medLayoutChooser.cpp
  medLoginWidget.cpp
  medImageFlow.cpp
  medImageList.cpp
  medImagePreview.cpp
  medImageReflector.cpp
  medImageStack.cpp
  medPatientPreview.cpp
  medSeriesPreview.cpp
  medStatusPanel.cpp
  medStudyPreview.cpp
  medStyle.cpp
  medToolBox.cpp
)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME}
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
