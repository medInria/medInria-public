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
## Set up versioning control
## #############################################################################

set(git_url ${GITHUB_PREFIX}DCMTK/dcmtk.git)
if(${USE_LASTEST_DCMTK})
  set(git_tag master)
else()
  set(git_tag DCMTK-3.6.7)
endif()



## #############################################################################
## Check if patch has to be applied
## #############################################################################
  
# ep_GeneratePatchCommand(${ep} ${ep}_PATCH_COMMAND DCMTK_STL_QUIET.patch)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

if (WIN32)
  set(BUILD_SHARED_LIBS_${ep} OFF)
  set(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS ON)
  set(DCMTK_WITH_ICU=OFF)
else()
  set(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS OFF)
  set(DCMTK_WITH_ICU=ON)
endif()

# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -w")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -w")
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_ExtProjs}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX:=<INSTALL_DIR>
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}

  )

set(cmake_cache_args
  -DDCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS:BOOL=${DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS}
  -DDCMTK_WITH_DOXYGEN:BOOL=OFF
  -DDCMTK_WITH_ZLIB:BOOL=OFF    
  -DDCMTK_WITH_OPENSSL:BOOL=OFF 
  -DDCMTK_WITH_ICU:BOOL=${DCMTK_WITH_ICU} 
  -DDCMTK_WITH_PNG:BOOL=OFF     
  -DDCMTK_WITH_TIFF:BOOL=OFF    
  -DDCMTK_WITH_XML:BOOL=OFF
  -DDCMTK_WITH_WRAP:BOOL=OFF
  -DDCMTK_WITH_ICONV:BOOL=OFF
  -DDCMTK_ENABLE_STL:BOOL=ON
  -DBUILD_APPS:BOOL=OFF
  -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
  -DDCMTK_ENABLE_BUILTIN_DICTIONARY:BOOL=ON
  -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=ON
  -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON  
  -DCMAKE_INSTALL_PREFIX:PATH=${EP_INSTALL_PREFIX}/${ep}
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
  INSTALL_DIR ${EP_INSTALL_PREFIX}/${ep}
  
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  PATCH_COMMAND ${${ep}_PATCH_COMMAND}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  CMAKE_CACHE_ARGS ${cmake_cache_args}
  DEPENDS ${${ep}_dependencies}
  BUILD_ALWAYS ${EP_BUILD_ALWAYS}
  ${EP_INSTAL_COMMAND}
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_ROOT ${binary_dir} PARENT_SCOPE)
set(${ep}_DIR  ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
