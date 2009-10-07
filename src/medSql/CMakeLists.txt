### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Fri Sep 25 11:51:59 2009 (+0200)
## Version: $Id$
## Last-Updated: Fri Sep 25 12:21:53 2009 (+0200)
##           By: Julien Wintz
##     Update #: 15
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
  medDatabaseItem.h
  medDatabaseModel.h
)

set(${PROJECT_NAME}_HEADERS_MOC
  medDatabaseController.h
  medDatabaseModel.h
  medDatabaseView.h
)

set(${PROJECT_NAME}_SOURCES
  medDatabaseController.cpp
  medDatabaseItem.cpp
  medDatabaseModel.cpp
  medDatabaseView.cpp
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
  dtkCore
  ${QT_LIBRARIES}
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
