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
## Add external-project
## #############################################################################
ExternalProject_Add(xcom
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  DOWNLOAD_COMMAND curl -Lo xcom.tar.gz https://physics.nist.gov/PhysRefData/Xcom/XCOM.tar.gz
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  DEPENDS ${${ep}_dependencies}
  PATCH_COMMAND   tar xvzf ${EP_PATH_SOURCE}/src/xcom.tar.gz -C ${EP_PATH_SOURCE}/../build/  
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Configure step not required by Jsoncons."
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


