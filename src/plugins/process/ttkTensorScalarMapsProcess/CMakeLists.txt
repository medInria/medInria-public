set(TARGET_NAME ttkTensorScalarMapsProcessPlugin)

## #############################################################################
## Resolve dependencies
## #############################################################################

find_package(TTK REQUIRED COMPONENTS ITKTensor ttkAlgorithms)

find_package(VTK REQUIRED COMPONENTS vtkRenderingCore)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS ITKCommon)
include(${ITK_USE_FILE})

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_QRC}
  )

## #############################################################################
## include directories.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PRIVATE ${${TARGET_NAME}_INCLUDE_DIRS}
  )
# PRIVATE because plugins are not supposed to be used somewhere else

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ITKCommon
  Qt5::Core
  dtkCore
  dtkCoreSupport
  dtkLog
  TTK::ITKTensor
  TTK::ttkAlgorithms
  medCore
  medCoreLegacy
  medVtkInria
  medWidgets
  vtkRenderingCore
  )

## #################################################################
## Install rules
## #################################################################

set_plugin_install_rules(${TARGET_NAME})
