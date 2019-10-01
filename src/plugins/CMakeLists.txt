###############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################


## #############################################################################
## Options
## #############################################################################

option(BUILD_ALL_PLUGINS
  "Build all Medinria plugins (overrides individual selections)"
  ON
  )

option(BUILD_EXAMPLE_PLUGINS
  "Enable this if you want to build the examples"
  OFF
  )

option(BUILD_COMPOSITEDATASET_PLUGIN
  "Enable this if you want to build compositeDataSet plugin"
  OFF
  )


## #############################################################################
## list plugins
## #############################################################################

## ADD here any new plugin with a default compilation value,
## you want to include in the build,
set(PLUGIN_LIST
  legacy/medVtkFibersData                                                ON
  legacy/itkDataImage                                                    ON
  legacy/vtkDataMesh                                                     ON
  legacy/itkDataDiffusionGradientList                                    ON
  legacy/itkDataTensorImage                                              ON
  legacy/medSegmentation                                                 ON
  legacy/qtdcmDataSource                                                 ON
  legacy/itkDataSHImage                                                  ON
  legacy/undoRedoRegistration                                            ON
  legacy/itkProcessRegistrationDiffeomorphicDemons                       ON
  legacy/medVtkView                                                      ON
  legacy/medMaskApplication                                              ON
  legacy/medBinaryOperation                                              ON
  legacy/manualRegistration                                              ON
  legacy/polygonRoi                                                      ON
  legacy/mscAlgorithmPaint                                               ON
  legacy/variationalSegmentation                                         ON
  process/arithmetic_operation                                           ON
  process/bias_correction                                                ON
  process/dwi_basic_thresholding                                         ON
  process/morphomath_operation                                           ON
  process/mask_image                                                     ON
  process/single_filter                                                  ON
  )

  if(USE_DTKIMAGING)
    set(PLUGIN_LIST
      ${PLUGIN_LIST}
      medITKImageDTKImageConverter ON
      medImagingCompatibility ON
      )    
  endif()

## #############################################################################
## process plugins
## #############################################################################

## see cmake/module/add_plugin.cmake
add_plugins(${PLUGIN_LIST}
  )

if(BUILD_COMPOSITEDATASET_PLUGIN)
  add_subdirectory(medCompositeDataSets)
endif()

if (BUILD_EXAMPLE_PLUGINS)
  add_subdirectory(legacy/examples)
endif()
