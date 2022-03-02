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
################################################################################

function(QtDcm_project)
set(ep QtDCM)


## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies 
  ITK 
  DCMTK
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

set(git_url ${GITHUB_PREFIX}medInria/qtdcm.git)
set(git_tag APHP) 

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (WIN32)
  set(BUILD_SHARED_LIBS_${ep} OFF)
  set(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS ON)
else()
  set(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS OFF)
endif()

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
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DQt5_DIR=${Qt5_DIR}
  -DITK_DIR:FILEPATH=${ITK_DIR}
  -DDCMTK_DIR:FILEPATH=${DCMTK_DIR}
  -DDCMTK_FIND_PACKAGE_USE_CONFIG_ONLY=ON
  -DDCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS:BOOL=${DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS}
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