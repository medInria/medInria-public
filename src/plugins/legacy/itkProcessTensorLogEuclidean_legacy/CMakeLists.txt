cmake_minimum_required(VERSION 2.8.9)

project(itkProcessTensorLogEuclideanPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION_MAJOR 3 CACHE STRING "${PROJECT_NAME} major version number." FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 0 CACHE STRING "${PROJECT_NAME} minor version number." FORCE)
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build version number." FORCE)
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

## #################################################################
## Resolve dependencies
## #################################################################

find_package(medInria REQUIRED)
#message("${medInria_USE_FILE}")
#include(${medInria_USE_FILE})

find_package(TTK_Private REQUIRED)
include(${TTK_Private_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkProcessTensorLogEuclideanPlugin.h
  itkProcessTensorEstimationToolBox.h
  itkProcessTensorScalarMapsToolBox.h
  itkProcessTensorTractographyToolBox.h
  itkProcessTensorEstimation.h
  itkProcessTensorScalarMaps.h
  itkProcessTensorTractography.h

  itkQtSignalAdaptor.h
  itkProcessTensorLogEuclideanEstimation.h
  itkProcessTensorLogEuclideanRemoveNonPositive.h
  itkProcessTensorDTIBrainMaskCalculator.h
  itkProcessTensorLogEuclideanAnisotropicSmoothing.h
  itkProcessTensorLogEuclideanTractography.h
)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkProcessTensorLogEuclideanPlugin.cpp
  itkProcessTensorEstimationToolBox.cpp
  itkProcessTensorScalarMapsToolBox.cpp
  itkProcessTensorTractographyToolBox.cpp
  itkProcessTensorEstimation.cpp
  itkProcessTensorScalarMaps.cpp
  itkProcessTensorTractography.cpp

  itkQtSignalAdaptor.cxx
  itkProcessTensorLogEuclideanEstimation.cpp
  itkProcessTensorLogEuclideanRemoveNonPositive.cpp
  itkProcessTensorDTIBrainMaskCalculator.cpp
  itkProcessTensorLogEuclideanAnisotropicSmoothing.cpp
  itkProcessTensorLogEuclideanTractography.cpp
)

set(${PROJECT_NAME}_RESOURCES dtitrack.qrc)

## #################################################################
## Build rules
## #################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkLog
  dtkCore
  medCore
  ITKCommon
  ITKTensor
)

if (PLUGINS_PATH)
  set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_PATH})
  set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_PATH})
endif()

## #################################################################
## Source file layout in development environments like Visual Studio
## #################################################################

SOURCE_GROUP("Header Files" REGULAR_EXPRESSION .*\\.h\$)
SOURCE_GROUP("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
