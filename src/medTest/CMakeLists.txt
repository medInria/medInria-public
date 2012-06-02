### CMakeLists.txt --- 
## 
## Author: John Stark
## Copyright (C) 2011 - Thibaud Kloczko, Inria.
## Created: Wed May 25 16:00:00 2011 (+0100)
## Version: $Id$
## Last-Updated: Mon Jun 20 18:17:39 2011 (+0200)
##           By: Julien Wintz
##     Update #: 2
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medTest)

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  medQtView.h
  medQtDataImage.h
  medQtDataImageReader.h
  medQtDataImageWriter.h
  medTestApplication.h  
)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  medTestExport.h
  medTest.h
)

set(${PROJECT_NAME}_SOURCES
  medQtView.cpp
  medQtDataImage.cpp
  medQtDataImageReader.cpp
  medQtDataImageWriter.cpp
  medTestApplication.cpp
  medTest.cpp
)

## #################################################################
## Build rules
## #################################################################

if(MSVC)
  configure_file(${CMAKE_SOURCE_DIR}/m4s_dll.rc.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc" @ONLY)
  set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
endif()

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})
# qt4_add_RESOURCES(${PROJECT_NAME}_SOURCES_QRC ${${PROJECT_NAME}_RESOURCES})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_MOC} 
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  dtkCore
  medCore
  ${QT_QTTEST_LIBRARY}
  ${QT_LIBRARIES}  
)

# Group common files together in Visual Studio.
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
