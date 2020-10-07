function(zlib_project)

    set(external_project zlib)

    list(APPEND ${external_project}_dependencies
        )

    EP_Initialisation(${external_project}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS OFF
        )

    if (NOT USE_SYSTEM_${external_project})

        set(git_url ${GITHUB_PREFIX}madler/zlib.git)
        set(git_tag v1.2.11)

        set(${external_project}_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type for zlib: None Debug Release RelWithDebInfo MinSizeRel")

        set(cmake_args
            ${ep_common_cache_args}
            -DCMAKE_BUILD_TYPE=${${external_project}_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS_${external_project}}
            )

        epComputPath(${external_project})

        ExternalProject_Add(${external_project}
            PREFIX ${EP_PATH_SOURCE}
            SOURCE_DIR ${EP_PATH_SOURCE}/${external_project}
            BINARY_DIR ${build_path}
            TMP_DIR ${tmp_path}
            STAMP_DIR ${stamp_path}

            GIT_REPOSITORY ${git_url}
            GIT_TAG ${git_tag}
            CMAKE_GENERATOR ${gen}
            CMAKE_ARGS ${cmake_args}
            DEPENDS ${${external_project}_dependencies}
            UPDATE_COMMAND ""
            )

        set(${external_project}_DIR ${build_path} PARENT_SCOPE)

    endif()

endfunction()
