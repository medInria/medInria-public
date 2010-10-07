### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Sat Oct  3 18:22:21 2009 (+0200)
## Version: $Id$
## Last-Updated: Tue Oct  5 18:03:15 2010 (+0200)
##           By: Julien Wintz
##     Update #: 41
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medPacs)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  medAbstractPacsBaseScu.h
  medAbstractPacsFactory.h
  medAbstractPacsEchoScu.h
  medAbstractPacsFindScu.h
  medAbstractPacsMoveScu.h	
  medAbstractPacsStoreScu.h
  medAbstractPacsStoreScp.h
  medAbstractPacsNode.h
  medAbstractPacsResultDataset.h
  medAbstractPacsKey.h
  medPacsExport.h
  medPacsWidget.h)

set(${PROJECT_NAME}_HEADERS_MOC
  medAbstractPacsBaseScu.h
  medAbstractPacsFactory.h
  medAbstractPacsEchoScu.h
  medAbstractPacsFindScu.h
  medAbstractPacsMoveScu.h
  medAbstractPacsStoreScu.h
  medAbstractPacsStoreScp.h
  medAbstractPacsNode.h
  medAbstractPacsResultDataset.h
  medAbstractPacsKey.h	
  medPacsWidget.h)

set(${PROJECT_NAME}_SOURCES
  medAbstractPacsBaseScu.cpp	
  medAbstractPacsFactory.cpp
  medAbstractPacsEchoScu.cpp
  medAbstractPacsFindScu.cpp
  medAbstractPacsMoveScu.cpp
  medAbstractPacsStoreScu.cpp
  medAbstractPacsStoreScp.cpp
  medAbstractPacsNode.cpp
  medAbstractPacsResultDataset.cpp
  medAbstractPacsKey.cpp
  medPacsWidget.cpp)

## #################################################################
## Build rules
## #################################################################

if(NOT MSVC)
add_definitions(-Wno-write-strings)
add_definitions(-Wformat=0)
endif(NOT MSVC)

add_definitions(-DQT_SHARED)
add_definitions(${QT_DEFINITIONS})
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
