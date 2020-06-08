function(gvirtualXRay_project) 

set(ep gvirtualXRay)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies)
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

epComputPath(${ep})

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DCMAKE_INSTALL_PREFIX:PATH=${build_path}
  -DBUILD_TUTORIALS:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  -DUSE_SYSTEM_ZLIB:BOOL=ON
  -DUSE_SYSTEM_LIBTIFF:BOOL=OFF
  -DUSE_SYSTEM_GLEW=OFF
  -DUSE_LIBTIFF:BOOL=OFF
  -DUSE_FFTW3:BOOL=OFF
  -DUSE_FREETYPE:BOOL=OFF
  -DUSE_GDCM:BOOL=OFF
  -DUSE_ITK:BOOL=OFF
  -DUSE_assimp:BOOL=OFF
)
 
## #############################################################################
## Add external-project
## ############################################################################# 
if(APPLE)
  set (cmake_args
  ${cmake_args}
  -DCMAKE_MACOSX_RPATH=OFF)
endif(APPLE)

ExternalProject_Add(${ep}
  PREFIX ${EP_PATH_SOURCE}
  SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
  BINARY_DIR ${build_path}
  TMP_DIR ${tmp_path}
  STAMP_DIR ${stamp_path}
  CMAKE_ARGS ${cmake_args}

  URL https://sourceforge.net/projects/gvirtualxray/files/1.1/gVirtualXRay-1.1.0-Source.zip/download
  DEPENDS ${${ep}_dependencies}
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_SUBDIR gvxr
  BUILD_COMMAND  ${CMAKE_MAKE_PROGRAM} install
  INSTALL_COMMAND ""
)
## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()


