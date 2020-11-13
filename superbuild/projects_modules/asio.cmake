function(asio_project)

set(ep asio)

## #############################################################################
## List the dependencies of the project
## #############################################################################

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

###############################################################################
## Define repository for sources
###############################################################################

set(git_url ${GITHUB_PREFIX}chriskohlhoff/asio.git)
set(git_tag asio-1-18-0)

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
  UPDATE_COMMAND ""
  DEPENDS ${${ep}_dependencies}
  # Nothing to configure or build
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
set(${ep}_DIR ${EP_PATH_SOURCE}/${ep}/${ep} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
