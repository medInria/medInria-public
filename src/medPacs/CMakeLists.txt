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
  medPacsMover.h
  medPacsExport.h
  medPacsWidget.h
  medPacsNode.h
  medMoveCommandItem.h)

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
  medPacsMover.h
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
  medPacsMover.cpp
  medPacsWidget.cpp)

## #################################################################
## Build rules
## #################################################################

if (NOT MSVC)
add_definitions(-Wno-write-strings)
add_definitions(-Wformat=0)
endif()

add_definitions(-DQT_SHARED)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "medPch.h" "../medPch/medPch.cpp" 
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES_WRAP})
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_WRAP})

target_link_libraries(${PROJECT_NAME} 
  ${QT_LIBRARIES}
  dtkCore
  medCore)

# Include header files for Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Installation
## #################################################################

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
