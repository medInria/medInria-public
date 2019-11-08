function(tetgen_project)

set(ep tetgen)

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
  REQUIRED_FOR_PLUGINS OFF
  )


if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Define repository where get the sources
## #############################################################################

set(git_url ${GITHUB_PREFIX}ufz/tetgen.git)
set(git_tag master)



## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX OR APPLE)
  set(${ep}_c_flags "${${ep}_c_flags}")
  set(${ep}_cxx_flags "${${ep}_cxx_flags}")
endif()

set(cmake_args
	${ep_common_cache_args}
	-DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
	-DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  )


## #############################################################################
## Add external-project
## #############################################################################
set(source_dir ${EP_PATH_SOURCE}/tetgen)
set(build_dir ${EP_PATH_SOURCE}/../build/tetgen)

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  GIT_REPOSITORY ${git_url}
  GIT_TAG ${git_tag}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  CONFIGURE_COMMAND mkdir -p ${build_dir} &&
                          cd ${build_dir} &&
                          cmake  ${cmake_args} ${source_dir}
  BUILD_COMMAND cd ${build_dir} && make   ## might have to put 
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  )


## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
