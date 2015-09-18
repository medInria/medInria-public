################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

if(${PROJECT_NAME}_USE_PRECOMPILED_HEADERS)
  file(GLOB MEDPLUGINSPCH_H
    medPluginsPch/*.h
    )
  
  file(GLOB MEDPLUGINSPCH_CPP
    medPluginsPch/*.cpp
    )
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/)

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
  legacy/medVtkFibersData                           ON
  legacy/itkDataImage                               ON
  legacy/vtkDataMesh                                ON
  legacy/itkDataDiffusionGradientList               ON
  legacy/itkDataTensorImage                         ON
  legacy/medSegmentation                            ON
  legacy/itkFilters                                 ON
  legacy/qtdcmDataSource                            ON
  legacy/itkDataSHImage                             ON
  legacy/undoRedoRegistration                       ON
  legacy/itkProcessRegistrationDiffeomorphicDemons  ON
  legacy/medVtkView                                 ON
  
  meditkGaussianFilterPlugin                        ON
  meditkAddFilterPlugin                             ON
  meditkCloseFilterPlugin                           ON
  meditkDilateFilterPlugin                          ON
  meditkDivideFilterPlugin                          ON
  meditkErodeFilterPlugin                           ON
  meditkInvertFilterPlugin                          ON
  meditkMedianFilterPlugin                          ON
  meditkMultiplyFilterPlugin                        ON
  meditkNormalizeFilterPlugin                       ON
  meditkOpenFilterPlugin                            ON
  meditkShrinkFilterPlugin                          ON
  meditkSubstractFilterPlugin                       ON
  meditkWindowingFilterPlugin                       ON
  meditkRigidRegistration                           ON
  meditkRegistrationDiffeomorphicDemonsProcess      ON
 
  )   


## #############################################################################
## process plugins
## #############################################################################

## see cmake/module/add_plugin.cmake
add_plugins(${PLUGIN_LIST})

if(BUILD_COMPOSITEDATASET_PLUGIN)
  add_subdirectory(medCompositeDataSets)          
endif() 

if (BUILD_EXAMPLE_PLUGINS)
  add_subdirectory(examples)
endif()
