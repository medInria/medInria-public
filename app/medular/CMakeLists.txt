### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu Sep 17 07:53:29 2009 (+0200)
## Version: $Id$
## Last-Updated: Thu Feb 11 10:32:39 2010 (+0100)
##           By: Julien Wintz
##     Update #: 48
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medular)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "medular major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "medular minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "medular build version number.")
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS

)

set(${PROJECT_NAME}_HEADERS_MOC
  medAdminArea.h
  medBrowserArea.h
  medDocumentationArea.h
  medMainWindow.h
  medViewerArea.h
  medWelcomeArea.h
)

set(${PROJECT_NAME}_HEADERS_UIC
  medAdminArea.ui
)

set(${PROJECT_NAME}_SOURCES
  medAdminArea.cpp
  medBrowserArea.cpp
  medDocumentationArea.cpp
  medMainWindow.cpp
  medViewerArea.cpp
  medWelcomeArea.cpp
  main.cpp
)

set(${PROJECT_NAME}_RESOURCES
  medular.qrc
)

## #################################################################
## Wrapping
## #################################################################

#dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medular python ${CMAKE_CURRENT_SOURCE_DIR}/medular.i ${${PROJECT_NAME}_WRAP_DEPENDS})
#dtk_wrap(${PROJECT_NAME} ${PROJECT_NAME}_SOURCES_WRAP medular tcl    ${CMAKE_CURRENT_SOURCE_DIR}/medular.i ${${PROJECT_NAME}_WRAP_DEPENDS})

## #################################################################
## Build rules
## #################################################################

qt4_wrap_ui(${PROJECT_NAME}_SOURCES_UIC ${${PROJECT_NAME}_HEADERS_UIC})
qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
  ${${PROJECT_NAME}_SOURCES_WRAP}
  ${${PROJECT_NAME}_SOURCES_QRC}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_UIC}
  ${${PROJECT_NAME}_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkScript
  dtkGui
  dtkHelp
  medGui
  medSql
)
