################################################################################
#
# medInria
#
# Copyright (c) INRIA 2022. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

function(PYNCPP_project)

    set(ep PYNCPP)

    EP_Initialisation(${ep}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS OFF
        )

    if(USE_SYSTEM_${ep})
        find_package(PYNCPP ${version_major}.${version_minor}.${version_patch}
            REQUIRED COMPONENTS Qt5
            )
    else()
        epComputPath(${ep})

        set(source_dir ${EP_PATH_SOURCE}/${ep})
        set(binary_dir ${build_path})

        set(project_args
            GIT_REPOSITORY ${GITHUB_PREFIX}LIRYC-IHU/pyncpp.git
            GIT_TAG origin/working
            GIT_SHALLOW True
            GIT_PROGRESS True
            )

        ## #####################################################################
        ## Add external project
        ## #####################################################################

        ExternalProject_Add(${ep}
            PREFIX ${EP_PATH_SOURCE}
            SOURCE_DIR ${source_dir}
            BINARY_DIR ${binary_dir}
            TMP_DIR ${tmp_path}
            STAMP_DIR ${stamp_path}
            DEPENDS ${${ep}_dependencies}
            INSTALL_COMMAND ""
            "${project_args}"
            )

        ExternalProject_Get_Property(${ep} binary_dir)

        ## #####################################################################
        ## Export variables
        ## #####################################################################

        set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

    endif()

endfunction()
