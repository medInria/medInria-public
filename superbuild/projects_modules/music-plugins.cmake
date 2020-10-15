function(music_plugins_project)

    set(external_project music-plugins)

    list(APPEND ${external_project}_dependencies
        dtk
        ITK
        VTK
        DCMTK
        medInria
        jsoncons
        asio
        websocketpp
        openssl
        mmg
        tetgen
        eigen
        qwt
        quazip
        )

    EP_Initialisation(${external_project}
        USE_SYSTEM OFF
        BUILD_SHARED_LIBS ON
        REQUIRED_FOR_PLUGINS ON
        )

    if (NOT USE_SYSTEM_${external_project})

        set(git_url ${GITHUB_PREFIX}Inria-Asclepios/music.git)
        set(git_tag music3)

        set(${external_project}_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type for MUSICardio plugins: None Debug Release RelWithDebInfo MinSizeRel")

        set(cmake_args
            ${ep_common_cache_args}
            -DCMAKE_BUILD_TYPE=${${external_project}_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS_${external_project}}
            -DQt5_DIR=${Qt5_DIR}
            -Ddtk_DIR:FILEPATH=${dtk_DIR}
            -DITK_DIR:FILEPATH=${ITK_DIR}
            -DVTK_DIR:FILEPATH=${VTK_DIR}
            -DDCMTK_DIR:FILEPATH=${DCMTK_DIR}
            -DmedInria_DIR:FILEPATH=${medInria_DIR}
            -DBoost_INCLUDE_DIR=${Boost_INCLUDE_DIR}
            -Djsoncons_DIR:FILEPATH=${jsoncons_DIR}
            -Dasio_DIR:FILEPATH=${asio_DIR}
            -Dwebsocketpp_DIR:FILEPATH=${websocketpp_DIR}
            -Dopenssl_DIR:FILEPATH=${openssl_DIR}
            -DEIGEN_INCLUDE_DIR:FILEPATH=${eigen_INCLUDE_DIR}
            -DQWT_INCLUDE_DIR:FILEPATH=${qwt_INCLUDE_DIR}
            -DQWT_DIR:FILEPATH=${qwt_DIR}
            -DMMG_INCDIR:FILEPATH=${mmg_INCDIR}
            -DMMG_LIBDIR:FILEPATH=${mmg_LIBDIR}
            -DMMG_SRC_DIR:FILEPATH=${mmg_SRC_DIR}
            -DTETGEN_INCLUDE_DIR:FILEPATH=${tetgen_INCLUDE_DIR}
            -DTETGEN_DIR:FILEPATH=${tetgen_DIR}
            -DQUAZIP_DIR:FILEPATH=${quazip_DIR}
            -DQUAZIP_INCLUDE_DIR:FILEPATH=${quazip_INCLUDE_DIR}
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
            INSTALL_COMMAND ""
            UPDATE_COMMAND ""
            BUILD_ALWAYS 1
            )

        set(EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS ${EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS} "${build_path}" PARENT_SCOPE)

        ExternalProject_Get_Property(${external_project} binary_dir)
        set(${external_project}_DIR ${binary_dir} PARENT_SCOPE)

        ExternalProject_Get_Property(${external_project} source_dir)
        set(${external_project}_SOURCE_DIR ${source_dir} PARENT_SCOPE)

    endif()

endfunction()
