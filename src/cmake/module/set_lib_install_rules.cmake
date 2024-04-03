################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

function(set_lib_install_rules target)

################################################################################
#
# Usage: set_lib_install_rules(target [RESOURCE] [HEADERS [headers...]])
#
# Set rules for a library target.
#
# The options are:
#
# RESOURCE
#     Indicate that this is a resource library. Resource libraries are shared
#     librairies that are not dependencies (direct or indirect) of the
#     executable. An example of this are Python extension modules, which are not
#     linked into the executable but loaded by the embedded Python's import
#     mechanism.
#
# HEADERS
#     Add headers to '{CMAKE_PREFIX_INSTALL}/include' during the install step.
#
################################################################################

    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" "" "HEADERS")
    get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    if (medInria_ROOT)
        set(dest ${medInria_ROOT})
    else()
        set(dest ${CMAKE_BINARY_DIR})
    endif()

    if(${GENERATOR_MULTI_CONFIG})
        set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/bin)
        set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/bin)
        set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/bin)
        set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/bin)

        set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/lib)
        set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/lib)
        set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/lib)
        set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/lib)

        set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/lib)
        set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/lib)
        set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/lib)
        set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/lib)
    else()
        set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${dest}/bin)
        set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${dest}/lib)
        set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${dest}/lib)
    endif()

    install(TARGETS ${target}
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        FRAMEWORK DESTINATION lib
        RESOURCE DESTINATION resources/${target}
        )

## #############################################################################
## Add headers wich have to be exposed in the include dir of the install tree
## #############################################################################

    if(ARG_HEADERS)
        install(FILES ${ARG_HEADERS}
            DESTINATION include/${target}
            )
    endif()

endfunction()
