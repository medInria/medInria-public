function(openssl_project)

set(ep openssl)

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
set(git_url ${GITHUB_PREFIX}openssl/openssl.git)
set(git_tag OpenSSL_1_1_1b)
## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX OR APPLE)
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_C_COMPILER_VERSION VERSION_LESS 5.4)
    set(${ep}_c_flags "${${ep}_c_flags} -std=c99")
  else()
    set(${ep}_c_flags "${${ep}_c_flags}")
  endif()
  set(${ep}_cxx_flags "${${ep}_cxx_flags}")
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
  )

if(APPLE)
  set (cmake_args 
  ${cmake_args} 
  -DCMAKE_MACOSX_RPATH=OFF)
endif(APPLE)


## #############################################################################
## Add external-project
## #############################################################################
epComputPath(${ep})

if (WIN32)
	if(CMAKE_SIZE_OF_VOID_P EQUAL 8) 
		set(OS_CRYPTO VC-WIN64A) 
	elseif(CMAKE_SIZE_OF_VOID_P EQUAL 4)
		set(OS_CRYPTO VC-WIN32) 
	endif()

ExternalProject_Add(${ep}
    PREFIX ${EP_PATH_SOURCE}
    SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
    BINARY_DIR ${build_path}/realBuild
    TMP_DIR ${tmp_path}
    STAMP_DIR ${stamp_path}

    GIT_REPOSITORY ${git_url}
    GIT_TAG ${git_tag}
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    DEPENDS ${${ep}_dependencies}
    CONFIGURE_COMMAND perl ${EP_PATH_SOURCE}/${ep}/Configure ${OS_CRYPTO} no-tests no-zlib no-shared --prefix=${build_path}  --openssldir=${build_path} 
    BUILD_COMMAND nmake install
    INSTALL_COMMAND ""
    )

else (WIN32)

  ExternalProject_Add(${ep}
    PREFIX ${EP_PATH_SOURCE}
    SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
    BINARY_DIR ${build_path}/realBuild
    TMP_DIR ${tmp_path}
    STAMP_DIR ${stamp_path}

    GIT_REPOSITORY ${git_url}
    GIT_TAG ${git_tag}
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    DEPENDS ${${ep}_dependencies}
    CONFIGURE_COMMAND ${EP_PATH_SOURCE}/${ep}/config no-zlib no-tests no-shared --prefix=${build_path} --openssldir=${build_path} 
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} install_sw
    INSTALL_COMMAND ""
  )

endif (WIN32)

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
# this temporary variable is necessary; passing directly ${build_path} fails 
set(build_dir ${build_path})
set(${ep}_DIR ${build_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
