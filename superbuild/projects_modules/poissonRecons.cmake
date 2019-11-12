function(poisson_project)

set(ep poisson)

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
  BUILD_SHARED_LIBS OFF
  REQUIRED_FOR_PLUGINS ON
  )

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository for sources
## #############################################################################

set(git_url ${GITHUB_PREFIX}mkazhdan/PoissonRecon.git )

set(git_tag 5ca45fbd82b0b79ea0bba1caa65abd8f6faae557) 

## #############################################################################
## Add external-project
## #############################################################################
set(patch_dir ${EP_PATH_SOURCE}/../../medInria-public/superbuild/patches)

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  DEPENDS ${${ep}_dependencies}
  PATCH_COMMAND  patch -i ${patch_dir}/PoissonReconPNGMakefilePatch.txt ${EP_PATH_SOURCE}/${ep}/PNG/Makefile && 
                 patch -i ${patch_dir}/PoissonReconMakefilePatch.txt ${EP_PATH_SOURCE}/${ep}/Makefile && 
		 cp ${EP_PATH_SOURCE}/${ep}/Src/PoissonRecon.cpp ${EP_PATH_SOURCE}/${ep}/Src/PoissonReconLib.h && 
		 patch -i ${patch_dir}/PoissonReconLibPatch.h ${EP_PATH_SOURCE}/${ep}/Src/PoissonReconLib.h && 
		 patch -i ${patch_dir}/PoissonReconPatch.txt ${EP_PATH_SOURCE}/${ep}/Src/PoissonRecon.cpp    
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Configure step not required by PoissonRecon."
  # no update, build or install command
  UPDATE_COMMAND ""
  BUILD_COMMAND cd ${EP_PATH_SOURCE}/${ep}/poisson/PNG   &&  make  
  INSTALL_COMMAND ""
)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
