################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

function(VTK_project)
set(ep VTK)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies ZLIB)

if(USE_Python)
  list(APPEND ${ep}_dependencies pyncpp)
endif()
  
## #############################################################################
## Prepare the project
## #############################################################################

EP_Initialisation(${ep} 
  USE_SYSTEM OFF 
  BUILD_SHARED_LIBS ON
  REQUIRED_FOR_PLUGINS ON
  )

if (NOT USE_SYSTEM_${ep})

## #############################################################################
## Set up versioning control
## #############################################################################

set(git_url ${GITHUB_PREFIX}Kitware/VTK.git)
set(git_tag v9.0.3)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

if (UNIX)
    set(${ep}_cxx_flags "${${ep}_cxx_flags} -w") # remove warnings
endif()

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_MACOSX_RPATH:BOOL=OFF
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>  
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DVTK_BUILD_TESTING=OFF
  -DVTK_BUILD_DOCUMENTATION=OFF
  -DVTK_BUILD_EXAMPLES=OFF
  -DVTK_RENDERING_BACKEND=OpenGL2
  -DVTK_QT_VERSION=5
  -DVTK_MODULE_ENABLE_VTK_GUISupportQt=YES
  -DVTK_MODULE_ENABLE_VTK_RenderingQt=YES
  -DVTK_MODULE_ENABLE_VTK_IOOggTheora:BOOL=YES
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  )
  
set(cmake_cache_args
  -DQt5_DIR:FILEPATH=${Qt5_DIR}
  )

# VTK (v9.3.1 at least) internal zlib is outdated to compile with modern clang on some macos
if(NOT USE_SYSTEM_ZLIB AND APPLE)
    list(APPEND cmake_args
        -DVTK_MODULE_USE_EXTERNAL_VTK_zlib:BOOL=ON
        -DZLIB_INCLUDE_DIR:FILEPATH=${ZLIB_ROOT}/include
        -DZLIB_LIBRARY_RELEASE:FILEPATH=${ZLIB_ROOT}/lib/libz.dylib
        -DZLIB_LIBRARY_DEBUG:FILEPATH=${ZLIB_ROOT}/lib/libz.dylib
    )
endif()

if (APPLE) # Error on recent macs with VTK 9.0.3
    list(APPEND cmake_args
        -DVTK_MODULE_ENABLE_VTK_hdf5=NO
    )
endif()

if(USE_OSPRay)
    list(APPEND cmake_args
        -DVTK_MODULE_ENABLE_VTK_RenderingOSPRay=YES
        -Dospray_DIR=${ospray_DIR}
        -DOSPRAY_INSTALL_DIR=${OSPRAY_INSTALL_DIR}
    )
endif()

if(USE_Python)
    if(UNIX)
        set(python_version    "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
        set(python_root       "${pyncpp_ROOT}/lib/python${python_version}")
        if(APPLE)
            set(python_executable "${pyncpp_ROOT}/lib/python${python_version}/bin/python${python_version}")
        else()
            set(python_executable "${pyncpp_ROOT}/lib/python${python_version}/bin/python${python_version}_bin")
        endif()
        set(python_include    "${pyncpp_ROOT}/lib/python${python_version}/include/python${python_version}")
        set(python_library    "${pyncpp_ROOT}/lib/python${python_version}/lib/libpython${python_version}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    else()
        set(python_version    "${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}")
        set(python_root       "${pyncpp_ROOT}/python${python_version}")
        set(python_executable "${pyncpp_ROOT}/python${python_version}/pythonw$<$<CONFIG:Debug>:_d>.exe")
        set(python_include    "${pyncpp_ROOT}/python${python_version}/include")
        set(python_library    "${pyncpp_ROOT}/python${python_version}/libs/python${python_version}$<$<CONFIG:Debug>:_d>.lib")
    endif()
    list(APPEND cmake_args
        -DVTK_WRAP_PYTHON:BOOL=ON
        -DPython3_EXECUTABLE:PATH=${python_executable}
        -DPython3_INCLUDE_DIR:PATH=${python_include}
        -DPython3_LIBRARY:PATH=${python_library}
        -DPython3_ROOT_DIR:PATH=${python_root}
        -DVTK_PYTHON_VERSION:STRING=${PYTHON_VERSION_MAJOR}
        )
endif()

## #############################################################################
## Check if patch has to be applied
## #############################################################################

ep_GeneratePatchCommand(${ep} ${ep}_PATCH_COMMAND VTK.patch)

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
  GIT_SHALLOW True
  GIT_PROGRESS True
  PATCH_COMMAND ${${ep}_PATCH_COMMAND}

  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  CMAKE_CACHE_ARGS ${cmake_cache_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  )
  
## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_ROOT ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
