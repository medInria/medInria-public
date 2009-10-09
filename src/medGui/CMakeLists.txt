### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Sep 16 17:20:13 2009 (+0200)
## Version: $Id$
## Last-Updated: Fri Oct  9 09:28:00 2009 (+0200)
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

project(medGui)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImagePreview.h
  medImageStack.h
  medPatientPreview.h
  medSeriesPreview.h
  medStudyPreview.h
  medStyle.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medLayoutChooser.h
  medLoginWidget.h
  medImageFlow.h
  medImagePreview.h
  medImageStack.h
  medPatientPreview.h
  medSeriesPreview.h
  medStudyPreview.h
  medStyle.h
)

set(${PROJECT_NAME}_SOURCES
  medLayoutChooser.cpp
  medLoginWidget.cpp
  medImageFlow.cpp
  medImagePreview.cpp
  medImageStack.cpp
  medPatientPreview.cpp
  medSeriesPreview.cpp
  medStudyPreview.cpp
  medStyle.cpp
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
