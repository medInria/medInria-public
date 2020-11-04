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

if(${USE_FFmpeg})
  list(APPEND ${ep}_dependencies ffmpeg)
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

if (NOT DEFINED ${ep}_SOURCE_DIR)
    set(git_url ${GITHUB_PREFIX}Kitware/VTK.git)
    set(git_tag v8.1.2)
endif()


## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX)
  set(${ep}_c_flags "${${ep}_c_flags} -w")
  set(${ep}_cxx_flags "${${ep}_cxx_flags} -w")
endif()

# library extension
if (UNIX AND NOT APPLE)
    set(extention so)
elseif(APPLE)
    set(extention dylib)
endif() # no WIN32 use of FFmpeg

set(cmake_args
  ${ep_common_cache_args}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
  -DCMAKE_C_FLAGS=${${ep}_c_flags}
  -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
  -DCMAKE_MACOSX_RPATH:BOOL=OFF
  -DCMAKE_SHARED_LINKER_FLAGS=${${ep}_shared_linker_flags}  
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>  
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS_${ep}}
  -DBUILD_TESTING=OFF
  -DBUILD_DOCUMENTATION=OFF
  -DBUILD_EXAMPLES=OFF
  -DVTK_RENDERING_BACKEND=OpenGL2
  -DVTK_Group_Qt=ON
  -DModule_vtkGUISupportQtOpenGL=ON
  -DModule_vtkRenderingOSPRay:BOOL=${USE_OSPRay}
  -DVTK_QT_VERSION=5
  -DQt5_DIR=${Qt5_DIR}
  -DVTK_USE_OGGTHEORA_ENCODER:BOOL=ON # OGV Export
  )

if(USE_OSPRay)
    list(APPEND cmake_args
        -Dospray_DIR=${ospray_DIR}
        -DOSPRAY_INSTALL_DIR=${OSPRAY_INSTALL_DIR}
    )
endif()

# Video Export
if(${USE_FFmpeg})
    list(APPEND cmake_args
        # FFMPEG
        -DModule_vtkIOFFMPEG:BOOL=ON
        -DFFMPEG_ROOT:STRING=${EP_PATH_BUILD}/ffmpeg
        -DFFMPEG_INCLUDE_DIR:STRING=${EP_PATH_BUILD}/ffmpeg/include/

        -DFFMPEG_LIBAVCODEC_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include
        -DFFMPEG_LIBAVDEVICE_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include
        -DFFMPEG_LIBAVFORMAT_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include
        -DFFMPEG_LIBAVUTIL_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include
        -DFFMPEG_LIBSWRESAMPLE_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include
        -DFFMPEG_LIBSWSCALE_INCLUDE_DIRS:STRING=${EP_PATH_BUILD}/ffmpeg/include

        -DFFMPEG_LIBAVDEVICE_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libavdevice.${extention}
        -DFFMPEG_LIBAVCODEC_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libavcodec.${extention}
        -DFFMPEG_LIBAVFORMAT_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libavformat.${extention}
        -DFFMPEG_LIBAVUTIL_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libavutil.${extention}
        -DFFMPEG_LIBSWRESAMPLE_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libswresample.${extention}
        -DFFMPEG_LIBSWSCALE_LIBRARIES:STRING=${EP_PATH_BUILD}/ffmpeg/lib/libswscale.${extention}
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
  PATCH_COMMAND ${${ep}_PATCH_COMMAND}
  CMAKE_GENERATOR ${gen}
  CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
  CMAKE_ARGS ${cmake_args}
  DEPENDS ${${ep}_dependencies}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 0
  )
  
## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
