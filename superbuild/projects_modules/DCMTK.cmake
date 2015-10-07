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

function(DCMTK_project)
set(ep DCMTK)


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
  REQUIRED_FOR_PLUGINS OFF
  )

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Set up versioning control.
## #############################################################################

set(git_url git://git.dcmtk.org/dcmtk.git)
set(git_tag DCMTK-3.6.1_20150924)


## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

if (WIN32)
  set(BUILD_SHARED_LIBS_${ep} OFF)
endif()


# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -w")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -w")
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX:=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DDCMTK_WITH_DOXYGEN=OFF
  -DDCMTK_WITH_ZLIB=OFF    
  -DDCMTK_WITH_OPENSSL=OFF 
  -DDCMTK_WITH_PNG=OFF     
  -DDCMTK_WITH_TIFF=OFF    
  -DDCMTK_WITH_XML=OFF
  -DDCMTK_WITH_WRAP=OFF
  -DDCMTK_WITH_ICONV=OFF
  -DBUILD_APPS=OFF
  )

## #############################################################################
## Add external-project
## #############################################################################

ExternalProject_Add(${ep}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  BUILD_ALWAYS
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} install_dir)
set(${ep}_DIR ${install_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
