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

## #############################################################################
## Embed medPch
## #############################################################################

file(GLOB MEDPLUGINSPCH_H
  medPluginsPch/*.h
  )
  
file(GLOB MEDPLUGINSPCH_CPP
  medPluginsPch/*.cpp
  )


## #############################################################################
## options
## #############################################################################

option(BUILD_ALL_PLUGINS 
  "Build all Medinria plugins (overrides individual selections)" 
  ON
  )

option(PLUGINS_EXAMPLES 
  "Enable this if you want to build the examples" 
  OFF
  )

option(BUILD_COMPOSITE_DATASET 
  "Enable this if you want to build compositeDataSet plugin"   
  OFF
  )


## #############################################################################
## list plugins
## #############################################################################

## ADD here any new plugin you want to include in the build, 
## except for registration plugins, see below.
set(PLUGIN_LIST
  v3dData                                   ON
  v3dView                                   ON
  itkDataImage                              ON
  itkDataImageReader                        ON
  itkDataImageWriter                        ON
  itkDCMTKDataImageReader                   ON
  vtkDataMesh                               ON
  vtkDataMeshReader                         ON
  vtkDataMeshWriter                         ON
  itkDataDiffusionGradientList              ON
  itkDataTensorImage                        ON
  itkDataTensorImageWriter                  ON
  itkDataTensorImageReader                  ON
  medSegmentation                           ON
  itkFilters                                ON
  qtdcmDataSource                           ON
  itkDataSHImage                            ON
  itkDataSHImageWriter                      ON
  itkDataSHImageReader                      ON
  undoRedoRegistration                      ON
	itkProcessRegistrationDiffeomorphicDemons ON
  )

if (MEDINRIA-PLUGINS_BUILD_EXAMPLES)
  set(PLUGIN_LIST
    ${PLUGIN_LIST}
    exampleProcessImageFilter               ON
    ITKProcessExample                       ON
    )
endif()

if(MEDINRIA-PLUGINS_BUILD_COMPOSITEDATASET)
  set (PLUGIN_LIST
    ${PLUGIN_LIST}
    medCompositeDataSets                    ON
    )
endif()    


## #############################################################################
## process plugins
## #############################################################################

## see cmake/module/add_plugin.cmake
add_plugins(${PLUGIN_LIST})

