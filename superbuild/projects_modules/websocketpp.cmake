function(websocketpp_project)

set(ep websocketpp)

## #############################################################################
## List the dependencies of the project
## #############################################################################
list(APPEND ${ep}_dependencies "")  
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

set(git_url ${GITHUB_PREFIX}zaphoyd/websocketpp.git)
set(git_tag 0.8.2)

## #############################################################################
## Add external-project
## #############################################################################
epComputPath(${ep})

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_MACOSX_RPATH:BOOL=OFF
  -DCMAKE_INSTALL_PREFIX=${build_path}
)

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  BINARY_DIR ${build_path}
  TMP_DIR ${tmp_path}
  STAMP_DIR ${stamp_path}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
set(${ep}_DIR ${build_path} PARENT_SCOPE)

## #############################################################################
## Add custom targets
## #############################################################################

endif() #NOT USE_SYSTEM_ep

endfunction()
