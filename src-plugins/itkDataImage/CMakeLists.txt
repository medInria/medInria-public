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

project(itkDataImagePlugin)

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

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC itkDataImagePlugin.h)
set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC}
  itkDataImage.h
  itkDataImageChar3.h
  itkDataImageChar4.h
  itkDataImageUChar4.h
  itkDataImageUChar3.h
  itkBinaryMask.h
  itkDataImageShort3.h
  itkDataImageUShort3.h
  itkDataImageUShort4.h
  itkDataImageInt3.h
  itkDataImageInt4.h
  itkDataImageLong4.h
  itkDataImageUInt3.h
  itkDataImageUInt4.h
  itkDataImageULong4.h
  itkDataImageLong3.h
  itkDataImageULong3.h
  itkDataImageFloat3.h
  itkDataImageFloat4.h
  itkDataImageDouble4.h
  itkDataImageDouble3.h
  itkDataImageVectorUChar3.h
  itkDataImageVectorFloat3.h
  itkDataImageRGB3.h
  itkDataImageRGBA3.h
  itkDataImageShort4.h
  )

set(${PROJECT_NAME}_SOURCES
itkDataImagePlugin.cpp
itkBinaryMask.cpp)

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
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore dtkLog
  medCore
  dtkItkLogForwarder
  ITKStatistics
)

if (BUILD_TESTING)
  add_subdirectory(Testing)
endif()

# Include header files for Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

# Due to the large number of template functions, windows needs an extra compile flag.
if (MSVC)
    set(bigobjFile itkDataImagePlugin.cpp)
    get_source_file_property(bigobjFile_COMPILE_FLAGS ${bigobjFile} COMPILE_FLAGS)
    if (${bigobjFile_COMPILE_FLAGS} STREQUAL NOTFOUND)
        set_source_files_properties(${bigobjFile} PROPERTIES COMPILE_FLAGS "/bigobj")
    else()
        set_source_files_properties(${bigobjFile} PROPERTIES COMPILE_FLAGS "${bigobjFile_COMPILE_FLAGS} /bigobj")
    endif()
endif()

#Prevent compiler warnings about valid std::copy.
if (MSVC)
    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS
    )
endif()

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
