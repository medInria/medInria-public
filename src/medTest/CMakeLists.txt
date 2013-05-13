#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

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
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
