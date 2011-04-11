### CMakeLists.txt ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu Sep 17 07:53:29 2009 (+0200)
## Version: $Id$
## Last-Updated: Wed Nov 10 11:12:54 2010 (+0100)
##           By: Julien Wintz
##     Update #: 86
######################################################################
##
### Commentary:
##
######################################################################
##
### Change log:
##
######################################################################

project(medinria)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  medBrowserArea.h  
  medBrowserToolBoxSource.h
  medBrowserToolBoxSettings.h  
  medMainWindow.h
  medViewerArea.h    
  medViewerConfigurationDiffusion.h
  medViewerConfigurationFunctional.h
  medViewerConfigurationVisualization.h
  medViewerConfigurationRegistration.h   
  medViewerToolBoxConfiguration.h  
  medViewerToolBoxPatient.h    
  medPacsDataSource.h 
  medFileSystemDataSource.h  
  medDatabaseDataSource.h
  )

set(${PROJECT_NAME}_HEADERS
  medViewerArea_p.h
  ${${PROJECT_NAME}_HEADERS_MOC}  
)

set(${PROJECT_NAME}_SOURCES
  main.cpp
  medBrowserArea.cpp  
  medBrowserToolBoxSource.cpp
  medBrowserToolBoxSettings.cpp    
  medMainWindow.cpp
  medViewerArea.cpp    
  medViewerToolBoxConfiguration.cpp
  medViewerToolBoxPatient.cpp  
  medViewerConfigurationDiffusion.cpp
  medViewerConfigurationFunctional.cpp
  medViewerConfigurationVisualization.cpp
  medViewerConfigurationRegistration.cpp   
  medPacsDataSource.cpp  
  medFileSystemDataSource.cpp
  medDatabaseDataSource.cpp
  )

set(${PROJECT_NAME}_RESOURCES medinria.qrc)

# Include header files for Visual Studio.
FILE(GLOB ${PROJECT_NAME}_HEADERS_ALL *.h )
set(${PROJECT_NAME}_SOURCES
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS_MOC}
	)

## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

set(DEPLOYMENT_SYSTEM)
IF(APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()
if(WIN32)
  if( NOT ${MED_SHOW_CONSOLE} )
    set(DEPLOYMENT_SYSTEM WIN32)
  endif()
  set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES} ${PROJECT_NAME}.rc)
endif()


add_executable(${PROJECT_NAME} ${DEPLOYMENT_SYSTEM}
  ${${PROJECT_NAME}_SOURCES_WRAP}
  ${${PROJECT_NAME}_SOURCES_QRC}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES}
  )

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkScript
  dtkGui
  dtkVr
  medGui
  medSql
  medPacs)

## #################################################################
## Bundle setup
## #################################################################

if(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1.0)
  SET(MACOSX_BUNDLE_VERSION 0.1.0)
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.1.0)
  set(${PROJECT_NAME}_RESOURCE_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Resources)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${PROJECT_NAME}_RESOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.icns ${${PROJECT_NAME}_RESOURCE_DIR})
endif(APPLE)

## #################################################################
## Installation
## #################################################################

install(TARGETS ${PROJECT_NAME}
   BUNDLE DESTINATION bin
  RUNTIME DESTINATION bin)

if(APPLE)
  install(CODE "execute_process(COMMAND macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app)")
endif(APPLE)
