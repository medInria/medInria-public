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

function(medInria_project)

set(ep medInria)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  dtk 
  DCMTK 
  ITK 
  VTK 
  TTK 
  QtDCM
  RPI
  )
  
## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}  
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS ON
  REQUIRED_FOR_PLUGINS ON
  )


if (NOT USE_SYSTEM_${ep})
## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -Wall")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall")
endif()

set(cmake_args
   ${ep_common_cache_args}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DDCMTK_DIR=${DCMTK_DIR}
  -Ddtk_DIR=${dtk_DIR}
  -DITK_DIR=${ITK_DIR}
  -DQTDCM_DIR=${QtDCM_DIR}
  -DRPI_DIR=${RPI_DIR}
  -DTTK_DIR=${TTK_DIR}
  -DVTK_DIR=${VTK_DIR}
  -DQt5_DIR=${Qt5_DIR}
  -DBOOST_ROOT=${BOOST_ROOT}
  -DMEDINRIA_VERSION:STRING=${${PROJECT_NAME}_VERSION}
  -DBUILD_ALL_PLUGINS=OFF
  -DBUILD_COMPOSITEDATASET_PLUGIN=OFF
  -DBUILD_EXAMPLE_PLUGINS=OFF
  -DBUILD_LEGACY/ITKDATADIFFUSIONGRADIENTLIST_PLUGIN=OFF
  -DBUILD_LEGACY/ITKDATAIMAGE_PLUGIN=ON
  -DBUILD_LEGACY/ITKDATASHIMAGE_PLUGIN=OFF
  -DBUILD_LEGACY/ITKDATATENSORIMAGE_PLUGIN=OFF
  -DBUILD_LEGACY/ITKFILTERS_PLUGIN=OFF
  -DBUILD_LEGACY/ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONS_PLUGIN=OFF
  -DBUILD_LEGACY/MEDSEGMENTATION_PLUGIN=OFF
  -DBUILD_LEGACY/MEDVTKFIBERSDATA_PLUGIN=OFF
  -DBUILD_LEGACY/MEDVTKVIEW_PLUGIN=ON
  -DBUILD_LEGACY/QTDCMDATASOURCE_PLUGIN=OFF
  -DBUILD_LEGACY/UNDOREDOREGISTRATION_PLUGIN=OFF
  -DBUILD_LEGACY/VTKDATAMESH_PLUGIN=OFF
  -DBUILD_PROCESS/ARITHMETIC_OPERATION/MEDITKADDIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/ARITHMETIC_OPERATION/MEDITKDIVIDEIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/ARITHMETIC_OPERATION/MEDITKMULTIPLYIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/ARITHMETIC_OPERATION/MEDITKSUBTRACTIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/MORPHOMATH_OPERATION/MEDITKCLOSINGIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/MORPHOMATH_OPERATION/MEDITKDILATEIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/MORPHOMATH_OPERATION/MEDITKERODEIMAGEPROCESS_PLUGIN=ON
  -DBUILD_PROCESS/MORPHOMATH_OPERATION/MEDITKOPENINGIMAGEPROCESS_PLUGIN=ON
  )
  
  
## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
  BINARY_DIR ${CMAKE_BINARY_DIR}/medInria-build
  STAMP_DIR ${CMAKE_BINARY_DIR}/medInria-stamp
  UPDATE_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1
  )

# ## #############################################################################
# ## Add step to get revisions
# ## #############################################################################

# # Create a revisions directory in the binary dir.
# ExternalProject_Get_Property(${ep} binary_dir)
# set(revisions_dir ${binary_dir}/revisions)
# file(MAKE_DIRECTORY ${revisions_dir})

# # Write out values into cmake module that will be loaded during configuration
# #TODO : Would be a good idea to find a better way.
# set(revisions_args ${revisions_dir}/revisions_args.cmake)
# file(WRITE ${revisions_args}
#   "set(projects ${external_projects})\n"
#   )
# file(APPEND ${revisions_args}
#   "set(sp_source_dir ${CMAKE_SOURCE_DIR})\n"
#   )
# file(APPEND ${revisions_args}
#   "set(GET_REVISIONS_MODULE_PATH ${GET_REVISIONS_MODULE_PATH})\n"
#   )

# ExternalProject_Add_Step(${ep} get-revisions
#     COMMAND cmake ${GET_REVISIONS_MODULE_PATH}
#     COMMENT "Get projects revisions"
#     DEPENDEES download    
#     ALWAYS 1
#     WORKING_DIRECTORY ${revisions_dir}
#     )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

ExternalProject_Get_Property(${ep} source_dir)
set(${ep}_SOURCE_DIR ${source_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()     























