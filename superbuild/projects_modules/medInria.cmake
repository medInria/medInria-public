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
  set(${ep}_c_flags "${${ep}_c_flags} -Wall -Wno-inconsistent-missing-override")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall -Wno-inconsistent-missing-override")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -fpermissive")
endif()

set(${ep}_BUILD_TYPE Debug CACHE STRING "Build type configuration specific to medInria.")

set(cmake_args
   ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${${ep}_BUILD_TYPE}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DDCMTK_DIR=${DCMTK_DIR}
  -Ddtk_DIR=${dtk_DIR}
  -DITK_DIR=${ITK_DIR}
  -DQtDCM_DIR=${QtDCM_DIR}
  -DRPI_DIR=${RPI_DIR}
  -DTTK_DIR=${TTK_DIR}
  -DVTK_DIR=${VTK_DIR}
  -DQt5_DIR=${Qt5_DIR}
  -DBoost_INCLUDE_DIR=${Boost_INCLUDE_DIR}
  -DmedInria_VERSION:STRING=${${PROJECT_NAME}_VERSION}
  -DBUILD_ALL_PLUGINS=OFF
  -DBUILD_COMPOSITEDATASET_PLUGIN=OFF
  -DBUILD_EXAMPLE_PLUGINS=OFF
  -DBUILD_LEGACY/ITKDATADIFFUSIONGRADIENTLIST_PLUGIN=ON
  -DBUILD_LEGACY/ITKDATAIMAGE_PLUGIN=ON
  -DBUILD_LEGACY/ITKDATASHIMAGE_PLUGIN=ON
  -DBUILD_LEGACY/ITKDATATENSORIMAGE_PLUGIN=ON
  -DBUILD_LEGACY/ITKFILTERS_PLUGIN=ON
  -DBUILD_LEGACY/ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONS_PLUGIN=ON
  -DBUILD_LEGACY/MEDSEGMENTATION_PLUGIN=ON
  -DBUILD_LEGACY/MEDVTKFIBERSDATA_PLUGIN=ON
  -DBUILD_LEGACY/MEDVTKVIEW_PLUGIN=ON
  -DBUILD_LEGACY/QTDCMDATASOURCE_PLUGIN=ON
  -DBUILD_LEGACY/UNDOREDOREGISTRATION_PLUGIN=ON
  -DBUILD_LEGACY/VTKDATAMESH_PLUGIN=ON
  -DBUILD_PROCESS/ARITHMETIC_OPERATION=ON
  -DBUILD_PROCESS/MORPHOMATH_OPERATION=ON
  )
  
  
## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  SOURCE_DIR ${medInria_SOURCE_DIR}
  BINARY_DIR ${medInria_BINARY_DIR}
  STAMP_DIR ${medinria_Stamp_DIR}
  UPDATE_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

ExternalProject_Get_Property(${ep} source_dir)
set(${ep}_SOURCE_DIR ${source_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()     























