################################################################################
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
################################################################################

project(v3dViewPlugin)

## #############################################################################
## Setup version numbering
## #############################################################################

set(${PROJECT_NAME}_VERSION ${MEDINRIA_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")



## #############################################################################
## Resolve dependencies
## #############################################################################

option(${PROJECT_NAME_UP}_USE_TENSORS
  "Allow tensor interactions in ${PROJECT_NAME}"
  ON
  )

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


if(${PROJECT_NAME_UP}_USE_TENSORS)
    # the Tensor ToolKit is required by the tensor interactor: v3dViewTensorInteractor
    find_package(TTK REQUIRED)
    include(${TTK_USE_FILE})
endif()

if (NOT VTK_USE_QT)
  message(WARNING
  "VTK has to be compiled with the option VTK_USE_QT:ON in order to compile this project.
  Please consider setting correctly your VTK."
  )
endif()

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  annotations
  )

if(${PROJECT_NAME_UP}_USE_TENSORS)

    list_source_files(${PROJECT_NAME}
      tensorInteractor
      )
    set(${PROJECT_NAME}_TTK_DEPENDENT_LIBRARIES
       ttkAlgorithms
       ITKTensor
       )
else()
    add_definitions(-DDISABLE_TTK_DEPENDENT_CODE)
endif()

## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME}
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
  ${MEDINRIA_INCLUDE_DIRS}
  )



## #############################################################################
## Precompile headers
## #############################################################################

if(MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${PROJECT_NAME}_SOURCES}
    )
endif()


## #############################################################################
## add library
## #############################################################################


qt4_add_resources(${PROJECT_NAME}_QRC ${${PROJECT_NAME}_QRC})


add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${${PROJECT_NAME}_TTK_DEPENDENT_LIBRARIES}
  dtkCore
  dtkLog
  vtkCommon
  vtkRendering
  vtkVolumeRendering
  QVTK
  medCore
  medVtkInria
  )




## #############################################################################
## Install rules
## #############################################################################

## see cmake/module/set_plugin_install_rules.cmake
set_plugin_install_rules(${PROJECT_NAME})

