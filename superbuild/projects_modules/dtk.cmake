##############################################################################
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
###############################################################################

function(dtk_project)
set(ep dtk)


## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  ""
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

set(git_url ${GITLAB_INRIA_PREFIX}dtk/dtk.git)
set(git_tag 1.7.1)


## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
 if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -Wall")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wno-inconsistent-missing-override")
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}   
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS_${ep}}
  -DDTK_BUILD_COMPOSER=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_DISTRIBUTED=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_SCRIPT=OFF                                                                                                                                                                                                                                                                                       
  -DDTK_BUILD_SUPPORT_COMPOSER=OFF                                                                                                                                                                                                                                                                                       
  -DDTK_BUILD_SUPPORT_CONTAINER=OFF                                                                                                                                                                                                                                                                                       
  -DDTK_BUILD_SUPPORT_CORE=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_SUPPORT_DISTRIBUTED=OFF                                                                                                                                                                                                                                                                                       
  -DDTK_BUILD_SUPPORT_GUI=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_SUPPORT_MATH=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_SUPPORT_PLOT=OFF                                                                                                                                                                                                                                                                                       
  -DDTK_BUILD_SUPPORT_VR=ON                                                                                                                                                                                                                                                                                        
  -DDTK_BUILD_WRAPPERS=OFF
  -DQt5_DIR=${Qt5_DIR}
  )


## #############################################################################
## Add external-project
## #############################################################################

ep_GeneratePatchCommand(${ep} DTK_PATCH_COMMAND dtk-1.7.1.patch)

epComputPath(${ep})

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  BINARY_DIR ${build_path}
  TMP_DIR ${tmp_path}
  STAMP_DIR ${stamp_path}
  
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  PATCH_COMMAND ${DTK_PATCH_COMMAND}
  INSTALL_COMMAND ""
  )


## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)


endif() #NOT USE_SYSTEM_ep

endfunction()
