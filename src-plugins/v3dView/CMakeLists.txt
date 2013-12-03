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

if(${PROJECT_NAME_UP}_USE_TENSORS)

    add_subdirectory(tensorInteractor)
    set(${PROJECT_NAME}_TTK_DEPENDENT_LIBRARIES
       ttkAlgorithms
       ITKTensor
       )
else()
    add_definitions(-DDISABLE_TTK_DEPENDENT_CODE)
endif()

add_subdirectory(annotations)

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  )

file(GLOB CPP
  *.cpp
  )
set(${PROJECT_NAME}_CPP
  ${CPP}
  ${${PROJECT_NAME}_CPP}
  )


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

if (MEDINRIA-PLUGINS_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPLUGINSPCH_H}
    ${MEDPLUGINSPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## add library
## #############################################################################


qt4_add_resources(${PROJECT_NAME}_QRC ${${PROJECT_NAME}_QRC})


add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_PCH}
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_CPP}
  ${${PROJECT_NAME}_QRC}
  )


## #############################################################################
## Link
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  ${${PROJECT_NAME}_TTK_DEPENDENT_LINK_LIBRARIES}
  dtkCore
  dtkLog
  medCore
  vtkCommon
  vtkRendering
  vtkImageView
  vtkWidgetsAddOn
  vtkVisuManagement
  vtkDataManagement
  vtkVolumeRendering
  QVTK
  vtkHWShading
  vtkHelpers
  )




## #############################################################################
## Install rules
## #############################################################################

## see cmake/module/set_plugin_install_rules.cmake
set_plugin_install_rules(${PROJECT_NAME})

