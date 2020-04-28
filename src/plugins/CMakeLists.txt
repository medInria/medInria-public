###############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
  legacy/diffeomorphicDemons                                             ON  
  legacy/iterativeClosestPoint                                           ON
  legacy/itkDataDiffusionGradientList                                    ON
  legacy/itkDataImage                                                    ON
  legacy/itkDataSHImage                                                  ON
  legacy/itkDataTensorImage                                              ON
  legacy/itkFilters                                                      ON
  legacy/itkINRDataImageReader                                           ON
  legacy/itkINRDataImageWriter                                           ON
  legacy/LCCLogDemons                                                    ON  
  legacy/manualRegistration                                              ON
  legacy/medAlgorithmPaint                                               ON
  legacy/medBinaryOperation                                              ON
  legacy/medCreateMeshFromMask                                           ON
  legacy/medFilteringWorkspaceL                                          ON
  legacy/medMaskApplication                                              ON
  legacy/medMeshingWorkspace                                             ON
  legacy/medN4BiasCorrection                                             ON
  legacy/medRegistrationWorkspace                                        ON
  legacy/medRemeshing                                                    ON
  legacy/medSegmentation                                                 ON
  legacy/medSegmentationWorkspace                                        ON
  legacy/medVtkFibersData                                                ON
  legacy/medVtkView                                                      ON
  legacy/meshManipulation                                                ON
  legacy/meshMapping                                                     ON
  legacy/polygonRoi                                                      ON
  legacy/qtdcmDataSource                                                 ON
  legacy/reformat                                                        ON
  legacy/undoRedoRegistration                                            ON
  legacy/variationalSegmentation                                         ON
  legacy/voiCutter                                                       ON
  legacy/vtkDataMesh                                                     ON
  process/arithmetic_operation                                           ON
  process/bias_correction                                                ON
  process/dwi_basic_thresholding                                         ON
  process/ttkTensorEstimationProcess                                     ON
  process/ttkTensorScalarMapsProcess                                     ON
  process/ttkTensorTractographyProcess                                   ON
  process/mask_image                                                     ON
  process/morphomath_operation                                           ON
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
