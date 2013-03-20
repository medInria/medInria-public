#   System identification

if (UNIX AND NOT APPLE)

    get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

    if ("${LIB64}" STREQUAL "TRUE")
        set(LIBSUFFIX 64)
    else()
        set(LIBSUFFIX "")
    endif()

    set(INSTALL_LIB_DIR lib${LIBSUFFIX}/medInria CACHE PATH "Installation directory for libraries")
    mark_as_advanced(INSTALL_LIB_DIR)

    find_program(LSB_RELEASE_EXECUTABLE lsb_release)
    if (LSB_RELEASE_EXECUTABLE)
        execute_process(COMMAND ${LSB_RELEASE_EXECUTABLE} -is
                        OUTPUT_VARIABLE DISTRIB
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(ERROR "Cannot determine distribution.")
    endif()
    mark_as_advanced(LSB_RELEASE_EXECUTABLE)

    find_program(ARCH_EXECUTABLE arch)
    if (ARCH_EXECUTABLE)
        execute_process(COMMAND ${ARCH_EXECUTABLE}
                        OUTPUT_VARIABLE ARCH
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(ERROR "Cannot determine architecture.")
    endif()

    #   This is ugly but for the time being the only way to
    #   decide the native packaging system. The list has
    #   to be revisited for any new type of packaging system,
    #   which defaults to DEB for the time being.

    find_file(PACKAGE_TYPE_RPM Packages PATHS /var/lib/rpm/)
    mark_as_advanced(PACKAGE_TYPE_RPM)

    if (PACKAGE_TYPE_RPM)
        set(CPACK_PACKAGE_TYPE RPM)
    else()
        set(CPACK_PACKAGE_TYPE DEB)
    endif()

    set(PACKAGE_ARCH ${ARCH})
    if ("${ARCH} STREQUAL 'x86_64" AND NOT PACKAGE_TYPE_RPM)
        set(PACKAGE_ARCH amd64)
    endif()

    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

elseif (APPLE)
    set(DISTRIB Apple)
    set(CPACK_PACKAGING_INSTALL_PREFIX "TODO")
else()
    set(DISTRIB windows)
    set(CPACK_PACKAGE_TYPE NSIS)
    set(CPACK_PACKAGING_INSTALL_PREFIX "TODO")
endif()
