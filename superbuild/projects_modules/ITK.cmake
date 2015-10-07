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

function(ITK_project)
set(ep ITK)


## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  VTK
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
## Set up versioning control.
## #############################################################################

set(git_url git://itk.org/ITK.git)
set(git_tag release)


## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -w")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -w -fpermissive")
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DBUILD_EXAMPLES=OFF
  -DBUILD_TESTING=OFF
  -DModule_ITKIOPhilipsREC=ON
  -DModule_ITKReview=ON
  -DModule_ITKVtkGlue=ON
  -DVTK_DIR=${VTK_DIR}
  )

## #############################################################################
## Check if patch has to be applied
## #############################################################################

ep_GeneratePatchCommand(ITK ITK_PATCH_COMMAND ITK_Mac_Rpath.patch)

## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  ${ITK_PATCH_COMMAND}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  BUILD_ALWAYS
  )


## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(ITK binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)



endif() #NOT USE_SYSTEM_ep

endfunction()
