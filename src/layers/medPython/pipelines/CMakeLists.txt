################################################################################
#
# medInria
#
# Copyright (c) INRIA 2021. All rights reserved.
#
# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(TARGET_NAME medPipeline)
set(PYTHON_PACKAGE_NAME ${TARGET_NAME})

## #############################################################################
## Find dependencies
## #############################################################################

find_package(ITK REQUIRED ITKCommon ITKImageFilterBase)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED
    vtkCommonCore
    vtkCommonDataModel
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkIOLegacy
    vtkRenderingCore
    vtkRenderingVolumeOpenGL2
    )
include(${VTK_USE_FILE})

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    class
    gui
    instance
    )

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

list_source_files(${TARGET_NAME}Bindings
    bindings
    )

list_header_directories_to_include(${TARGET_NAME}Bindings
    ${${TARGET_NAME}Bindings_HEADERS}
    )

set(bindings_interfaces
    header.i
    constants.i
    pipeline_instance.i
    pipeline_workspace.i
    pipeline_working_state.i
    pipeline_delegate.i
    pipeline_step_properties.i
    filtering_delegate.i
    reformat_delegate.i
    registration_delegate.i
    roi_toolbox_delegate.i
    segmentation_delegate.i
    python_pipeline.i
    )

## #############################################################################
## Add targets
## #############################################################################

add_library(${TARGET_NAME} SHARED
    ${${TARGET_NAME}_CFILES}
    )

add_library(${TARGET_NAME}Bindings SHARED
    ${${TARGET_NAME}Bindings_CFILES}
    )

target_compile_definitions(${TARGET_NAME} PRIVATE
    TARGET_NAME="${TARGET_NAME}"
    )

target_compile_definitions(${TARGET_NAME}Bindings PRIVATE
    TARGET_NAME="${TARGET_NAME}Bindings"
    PYTHON_PACKAGE_NAME="${PYTHON_PACKAGE_NAME}"
    )

## #############################################################################
## Add Python modules
## #############################################################################

set_property(TARGET ${TARGET_NAME}Bindings PROPERTY
    PYTHON_PACKAGE_NAME ${PYTHON_PACKAGE_NAME})

add_python_bindings(${TARGET_NAME}Bindings ${TARGET_NAME}Bindings_modules
    IMPORT
    medInria_bindings
    INCLUDE
    ${bindings_interfaces}
    )

add_python_modules(${TARGET_NAME}Bindings
    SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    __init__.py
    )

generate_python_resources(${TARGET_NAME}Bindings)

## #############################################################################
## Include directories
## #############################################################################

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    )

target_include_directories(${TARGET_NAME}Bindings
    PUBLIC
    ${${TARGET_NAME}Bindings_INCLUDE_DIRS}
    )

target_include_directories(${TARGET_NAME}Bindings_modules
    PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings
    )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    PUBLIC
    ${ITK_LIBRARIES}
    ${VTK_LIBRARIES}
    dtkCore
    dtkCoreSupport
    medCoreLegacy
    medUtilities
    medVtkInria
    medVtkDataMeshBase
    ${PYTHON_PROJECT_NAME}Base
    )

target_link_libraries(${TARGET_NAME}Bindings
    PUBLIC
    medCoreLegacy
    ${PYTHON_PROJECT_NAME}Base
    )

target_link_libraries(${TARGET_NAME}Bindings_modules
    PUBLIC
    ${TARGET_NAME}
    ${PYTHON_PROJECT_NAME}Tools
    )

## #############################################################################
## Install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

set_lib_install_rules(${TARGET_NAME}Bindings
    ${${TARGET_NAME}Bindings_HEADERS}
    )
