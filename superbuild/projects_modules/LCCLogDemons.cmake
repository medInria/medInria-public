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
##############################################################################

function(LCCLogDemons_project)
set(ep LCCLogDemons)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  Qt5
  ITK
  RPI
  dtk
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

set(git_url ${GITHUB_PREFIX}Inria-Asclepios/LCC-LogDemons.git)
set(git_tag master)


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
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS_${ep}}
  -DQt5_DIR:FILEPATH=${Qt5_DIR}
  -Ddtk_DIR:FILEPATH=${dtk_DIR}
  -DITK_DIR:FILEPATH=${ITK_DIR}
  -DRPI_DIR:FILEPATH=${RPI_DIR}
  -DLOG_DEMONS_BUILD_PLUGIN:BOOL=ON
  )


## #############################################################################
## Add external-project
## #############################################################################

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
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1   
  )
  
  
## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

  
endif() #NOT USE_SYSTEM_ep

endfunction()