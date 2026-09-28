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

set(PYTHON_VERSION_MAJOR 3  CACHE STRING "Python major version")
set(PYTHON_VERSION_MINOR 12 CACHE STRING "Python minor version")
set(PYTHON_VERSION_PATCH 14 CACHE STRING "Python patch version")

function(pyncpp_project)

    set(ep pyncpp)

    EP_Initialisation(${ep}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS ON
        )

    if(NOT USE_SYSTEM_${ep})

        epComputPath(${ep})

        set(git_url ${GITHUB_PREFIX}LIRYC-IHU/pyncpp.git)
        set(git_tag working)

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

            GIT_REPOSITORY ${git_url}
            GIT_TAG ${git_tag}
            GIT_SHALLOW True
            GIT_PROGRESS True

            DEPENDS ${${ep}_dependencies}
            CMAKE_ARGS ${cmake_args}
            INSTALL_COMMAND ""
            )

        ExternalProject_Get_Property(${ep} BINARY_DIR)
        if(WIN32)
            set(PYTHON_EXE "${BINARY_DIR}/python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}/python.exe")

            ExternalProject_Add_Step(${ep} install_dependencies_windows
                # setuptools needed for Python > 3.12 on Windows
                COMMAND ${PYTHON_EXE} -m pip install numpy vtk SimpleITK scipy setuptools
                DEPENDEES install
                COMMENT "Installing Python dependencies on Windows"
                WORKING_DIRECTORY ${BINARY_DIR}
            )
        else()
            set(PYTHON_EXE "${BINARY_DIR}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/bin/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")

            ExternalProject_Add_Step(${ep} install_dependencies_unix
                COMMAND ${PYTHON_EXE} -m pip install numpy vtk SimpleITK scipy
                DEPENDEES install
                COMMENT "Installing Python dependencies on Unix"
                WORKING_DIRECTORY ${BINARY_DIR}
            )
        endif()

        ## #####################################################################
        ## Export variables
        ## #####################################################################

        set(${ep}_ROOT "${build_path}" PARENT_SCOPE)

    endif()

endfunction()
