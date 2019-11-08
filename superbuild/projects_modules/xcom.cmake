function(xcom_project) 

set(ep xcom)

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
  REQUIRED_FOR_PLUGINS OFF
  )
if (NOT USE_SYSTEM_${ep})
## #############################################################################
## Define repository where get the sources
## #############################################################################
set(git_url ${GITHUB_PREFIX}bruceravel/demeter)
## set(git_tag master)
set(git_tag 0.9.26)
## #############################################################################
## Add external-project
## #############################################################################
ExternalProject_Add(xcom
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  DEPENDS ${${ep}_dependencies}
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Configure step not required by Jsoncons."
# no update, build or install command
  UPDATE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )
## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)
## #############################################################################
## Add custom targets
## #############################################################################

endif() #NOT USE_SYSTEM_ep

endfunction()


