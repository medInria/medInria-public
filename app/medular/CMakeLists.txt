### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu Sep 17 07:53:29 2009 (+0200)
## Version: $Id$
## Last-Updated: Fri Sep 25 12:50:17 2009 (+0200)
##           By: Julien Wintz
##     Update #: 30
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
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS

)

set(${PROJECT_NAME}_HEADERS_MOC
# medBrowserArea.h
  medMainWindow.h
  medViewerArea.h
  medWelcomeArea.h
)

set(${PROJECT_NAME}_SOURCES
  medBrowserArea.cpp
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

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
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
  medGui
  medSql
)

## #################################################################
## Handling of generated script modules
## #################################################################

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/medular.i
#  COMMAND ${CMAKE_COMMAND} ARGS -E  make_directory ${CMAKE_BINARY_DIR}/modules
#  COMMAND ${CMAKE_COMMAND} ARGS -E  copy ${${PROJECT_NAME}_BINARY_DIR}/medular.py ${CMAKE_BINARY_DIR}/modules
#  COMMENT "-- Moving python modules to ${CMAKE_BINARY_DIR}/modules"
#)

## #################################################################
## Bundle setup
## #################################################################

#if(APPLE)
#  set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
#  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1.0)
#  SET(MACOSX_BUNDLE_VERSION 0.1.0)
#  SET(MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.1.0)
#  set(${PROJECT_NAME}_RESOURCE_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Resources)
## set(${PROJECT_NAME}_PLUGINS_DIR  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Plugins)
#  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${PROJECT_NAME}_RESOURCE_DIR}
##   COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${PROJECT_NAME}_PLUGINS_DIR}
#    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.icns ${${PROJECT_NAME}_RESOURCE_DIR}
#    COMMENT "-- Setting up plugins and resources")
#endif(APPLE)
