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

include_guard(GLOBAL)
if(${USE_FFmpeg})

function(ffmpeg_project)
set(ep ffmpeg)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies)

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
## Set up versioning control
## #############################################################################

if (NOT DEFINED ${ep}_SOURCE_DIR)
    if(UNIX) # is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
        set(tag "n3.4.8") # FFMPEG
        set(location GIT_REPOSITORY "${GITHUB_PREFIX}FFmpeg/FFmpeg.git" GIT_TAG ${tag})
    endif()
endif()

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

set(cmake_args
    ${ep_common_cache_args}
    -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
    -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
    )

## #############################################################################
## Add external-project
## #############################################################################

if (UNIX)
    ExternalProject_Add(${ep}
        PREFIX ${EP_PATH_SOURCE}
        SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
        BINARY_DIR ${build_path}
        TMP_DIR ${tmp_path}
        STAMP_DIR ${stamp_path}

        ${location}
        CONFIGURE_COMMAND ${EP_PATH_SOURCE}/${ep}/configure
            --prefix=${EP_PATH_BUILD}/${ep}
            --disable-static --enable-shared
            --disable-yasm
        BUILD_COMMAND make install
        )

    install(DIRECTORY ${EP_PATH_BUILD}/${ep}/lib/
        DESTINATION lib
        FILES_MATCHING PATTERN "lib*"
        )
endif()

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################

ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif() #NOT USE_SYSTEM_ep

endfunction()
endif()
