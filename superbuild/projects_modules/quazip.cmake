function(quazip_project)

set(ep quazip)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies
     zlib)
  
## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS OFF
  REQUIRED_FOR_PLUGINS ON
)

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository where get the sources
## #############################################################################

set(git_url ${GITHUB_PREFIX}stachenov/quazip.git)
set(git_tag v1.1)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
# Compile using c++11 standard
set(${ep}_cxx_flags "${${ep}_cxx_flags} -Wall -std=c++11")
set(${ep}_c_flags "${${ep}_c_flags} -Wall")

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  # to find zlib 
  -DCMAKE_PREFIX_PATH:FILEPATH=${zlib_DIR}
  # to find Qt5
  -DQt5_DIR=${Qt5_DIR}
)

## #############################################################################
## Add external-project
## #############################################################################
if(APPLE)
  set(SPEC -spec macx-clang)
endif()

find_package(Qt5 REQUIRED Core)

ep_GeneratePatchCommand(${ep} ${ep}_PATCH_COMMAND quazip.patch)

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
  PATCH_COMMAND ${${ep}_PATCH_COMMAND}
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

set(${ep}_DIR ${build_path} PARENT_SCOPE)
set(${ep}_INCLUDE_DIR ${EP_PATH_SOURCE}/${ep} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
