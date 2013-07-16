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

cmake_minimum_required(VERSION 2.8.1)

project(itkDataImageReaderPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION ${medInria_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (ITK_USE_SYSTEM_GDCM)
  add_definitions(-DITK_USE_SYSTEM_GDCM)
endif()

#else()
#  message(WARNING "The DICOM GDCM reader plugin needs ITK to compile with SYSTEM_GDCM. If you don't want to use GDCM, discard this warning. Otherwise, #please set the option ITK_USE_SYSTEM_GDCM:ON in ITK and re-compile")
#endif()


## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkDataImageReaderPlugin.h
)

if (ITK_USE_SYSTEM_GDCM)
  set(${PROJECT_NAME}_HEADERS_MOC
    ${${PROJECT_NAME}_HEADERS_MOC}
    itkGDCMDataImageReader.h
    )
endif()

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkDataImageReaderPlugin.cpp
  itkDataImageReaderCommand.cxx
  itkMetaDataImageReader.cpp
  itkAnalyzeDataImageReader.cpp
  itkPNGDataImageReader.cpp
  itkBMPDataImageReader.cpp
  itkJPEGDataImageReader.cpp
  itkTIFFDataImageReader.cpp
  itkNiftiDataImageReader.cpp
  itkNrrdDataImageReader.cpp
  itkGiplDataImageReader.cpp
  itkVTKDataImageReader.cpp
  itkPhilipsRECDataImageReader.cpp
  itkGISDataImageReader.cpp
  )
if (ITK_USE_SYSTEM_GDCM)
  set(${PROJECT_NAME}_SOURCES
    ${${PROJECT_NAME}_SOURCES}
    itkGDCMDataImageReader.cpp
    )
endif()


## #################################################################
## Build rules
## #################################################################

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH
    "medPluginsPch.h" "../libs/medPluginsPch/medPluginsItkPch.cpp"
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_SOURCES_MOC})
endif()

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkItkLogForwarder
  dtkCore dtkLog
  medCore
  itkDataImageBase
  ITKIOReview
  itkIOAddOn
  ${ITK_LIBRARIES})

if (MEDINRIA-PLUGINS_BUILD_TESTING)
  add_subdirectory(Testing)
endif()

if (MEDINRIA-PLUGINS_BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

# Group common files together in Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION plugins
    LIBRARY DESTINATION plugins
    ARCHIVE DESTINATION lib)


