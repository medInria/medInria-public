function(eigen_project)

set(ep eigen)

###############################################################################
## List the dependencies of the project
###############################################################################

list(APPEND ${ep}_dependencies "")
  
###############################################################################
## Prepare the project
############################################################################### 

EP_Initialisation(${ep}
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS OFF
  REQUIRED_FOR_PLUGINS ON
  )


if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository for sources
## #############################################################################

set(git_url ${GITLAB_PREFIX}libeigen/eigen)
set(git_tag 3.3.8)

## #############################################################################
## Check if patch has to be applied
## #############################################################################

ep_GeneratePatchCommand(${ep} ${ep}_PATCH_COMMAND eigen-3.3.8.patch)

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
  PATCH_COMMAND ${${ep}_PATCH_COMMAND}
  DEPENDS ${${ep}_dependencies}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Eigen - no configure step required."
  # Nothing to build, as Eigen is fully templated
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)
set(${ep}_INCLUDE_DIR ${EP_PATH_SOURCE}/${ep} PARENT_SCOPE)

endif() 

endfunction()
