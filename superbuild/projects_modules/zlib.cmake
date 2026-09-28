function(ZLIB_project)

    set(ep ZLIB)

    EP_Initialisation(${ep}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS OFF
        )

    if (NOT USE_SYSTEM_${ep})

        set(git_url ${GITHUB_PREFIX}madler/zlib.git)
        set(git_tag v1.3.1.2)

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

        if (UNIX)
            set(${ep}_cxx_flags "${${ep}_cxx_flags} -w") # remove warnings
        endif()

        if (MSVC)
            set(${ep}_cxx_flags "${${ep}_cxx_flags} /Zc:__cplusplus")
        endif()

        set(cmake_args
            ${ep_common_cache_args}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_externals_projects}
            -DCMAKE_C_FLAGS=${${ep}_c_flags}
            -DCMAKE_CXX_FLAGS=${${ep}_cxx_flags}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS_${ep}}
            )

## #############################################################################
## Add external-project
## #############################################################################

        epComputPath(${ep})

        ExternalProject_Add(${ep}
            PREFIX ${EP_PATH_SOURCE}
            SOURCE_DIR ${EP_PATH_SOURCE}/${ep}
            BINARY_DIR ${build_path}
            INSTALL_DIR ${build_path}
            TMP_DIR ${tmp_path}
            STAMP_DIR ${stamp_path}

            GIT_REPOSITORY ${git_url}
            GIT_TAG ${git_tag}
            CMAKE_GENERATOR ${gen}
            CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
            CMAKE_ARGS ${cmake_args}
            DEPENDS ${${ep}_dependencies}
            UPDATE_COMMAND ""
            )

## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
        set(${ep}_ROOT ${build_path} PARENT_SCOPE)

    endif()

endfunction()
