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

set(PYTHON_VERSION_MAJOR 3 CACHE STRING "Python major version")
set(PYTHON_VERSION_MINOR 10 CACHE STRING "Python minor version")
set(PYTHON_VERSION_PATCH 10 CACHE STRING "Python patch version")

function(pyncpp_project)

    set(ep pyncpp)

    EP_Initialisation(${ep}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS ON
        )

    if(NOT USE_SYSTEM_${ep})

        epComputPath(${ep})

        set(project_args
            GIT_REPOSITORY ${GITHUB_PREFIX}LIRYC-IHU/pyncpp.git
            GIT_TAG working
            GIT_SHALLOW True
            GIT_PROGRESS True
            )

        set(cmake_args
            ${ep_common_cache_args}
            -D "PYNCPP_PYTHON_VERSION_MAJOR:STRING=${PYTHON_VERSION_MAJOR}"
            -D "PYNCPP_PYTHON_VERSION_MINOR:STRING=${PYTHON_VERSION_MINOR}"
            -D "PYNCPP_PYTHON_VERSION_PATCH:STRING=${PYTHON_VERSION_PATCH}"
            -D "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}"
            )

        if(UNIX)
            list(APPEND cmake_args
                -D Qt5_DIR:PATH=${Qt5_DIR}
                -D OPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}
                )
        endif()

        ## #####################################################################
        ## Add external project
        ## #####################################################################

        ExternalProject_Add(${ep}
            PREFIX ${EP_PATH_SOURCE}
            SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
            BINARY_DIR ${build_path}
            TMP_DIR ${tmp_path}
            STAMP_DIR ${stamp_path}
            DEPENDS ${${ep}_dependencies}
            CMAKE_ARGS ${cmake_args}
            INSTALL_COMMAND ""
            "${project_args}"
            )

        ## #####################################################################
        ## Export variables
        ## #####################################################################

        set(${ep}_ROOT "${build_path}" PARENT_SCOPE)

    endif()

endfunction()
