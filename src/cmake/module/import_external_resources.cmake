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

function(import_external_resources target)

################################################################################
#
# Usage: import_external_resources(target [APPEND]
#                                  [DESTINATION destination]
#                                  [FROM_TARGETS [targets...]]
#                                  [FILES [files...]]
#                                  [DIRECTORIES [directories...]])
#
# Creates post-build commands of 'target' to import resources in the appropriate
# folder for the target.
#
# The options are:
#
#
# DESTINATION
#     Specify the root directory (relative to the resource directory) into which
#     the resources will be imported (note that if the DESTINATION option was
#     used in `set_external_resources`, the two subdirectories are combined).
#
# FROM_TARGETS
#     Specify a list of targets that have associated resources (declared using
#     'set_external_resources') to import.
#
# FILES, DIRECTORIES
#     Specify resource files or directories to add directly. This is the
#     equivalent of the following lines:
#
#         set_external_resources(target FILES ... DIRECTORIES ...)
#         import_external_resources(target FROM_TARGETS target)
#
#     (in other words, when the target providing the resources is the same as
#     the one importing them)
#
################################################################################

    cmake_parse_arguments(PARSE_ARGV 1 "ARG"
        ""
        "DESTINATION"
        "FROM_TARGETS;FILES;DIRECTORIES"
        )

    get_target_property(target_type ${target} TYPE)

    if ((NOT target_type STREQUAL "EXECUTABLE")
        AND
        (NOT target_type STREQUAL "SHARED_LIBRARY"))
        message(FATAL_ERROR "Target must be an executable or a shared library.")
    endif()

    if (ARG_DESTINATION)
        set(base_destination "${ARG_DESTINATION}")
    else()
        set(base_destination ".")
    endif()

    _get_resources_parent_directory(target_dir ${target})
    _get_resources_directory(resources_base_dir ${target} ${target_type})
    set(resources_base_dir ${resources_base_dir}$<$<NOT:$<STREQUAL:${base_destination},".">>:/${base_destination}>)
    _convert_to_bundle_if_needed(${target} ${target_type})

    if (ARG_FILES OR ARG_DIRECTORIES)
        set_external_resources(${target} FILES ${ARG_FILES} DIRECTORIES ${ARG_DIRECTORIES})
        _import_resources(${target} ${target_dir} ${resources_base_dir} FILES ${ARG_FILES} DIRECTORIES ${ARG_DIRECTORIES})
    endif()

    if (ARG_FROM_TARGETS)
        foreach(source_target ${ARG_FROM_TARGETS})
            get_target_property(resources ${source_target} ${PROJECT_NAME}_RESOURCES)
            while (resources)
                _pop_resource_group(resources destination files directories)
                set(resources_dir ${resources_base_dir}$<$<NOT:$<STREQUAL:${destination},".">>:/${destination}>)
                _import_resources(${target} ${target_dir} ${resources_dir} FILES ${files} DIRECTORIES ${directories})
            endwhile()
        endforeach()

        add_dependencies(${target} ${ARG_FROM_TARGETS})
    endif()

endfunction()

################################################################################
# Internal
################################################################################

function(_import_resources target base_dir sub_dir)

    cmake_parse_arguments(PARSE_ARGV 3 "ARG"
        ""
        ""
        "FILES;DIRECTORIES"
        )

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${sub_dir}"
        WORKING_DIRECTORY "${base_dir}"
        )

    if (ARG_FILES)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARG_FILES} "${sub_dir}"
            WORKING_DIRECTORY "${base_dir}"
            )
        if (NOT APPLE)
            install(FILES ${ARG_FILES} DESTINATION "${sub_dir}")
        endif()
    endif()

    if (ARG_DIRECTORIES)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARG_DIRECTORIES} "${sub_dir}"
            WORKING_DIRECTORY "${base_dir}"
            )
        if (NOT APPLE)
            set(adjusted_directories)
            foreach (directory ${ARG_DIRECTORIES})
                list(APPEND adjusted_directories ${directory}/)
            endforeach()
            install(DIRECTORY ${adjusted_directories} DESTINATION "${sub_dir}")
        endif()
    endif()

endfunction()

# Extract data constructed using the '_build_resource_group' macro in 'set_external_resources.cmake'
macro(_pop_resource_group resources_var destination_var files_var directories_var)

    list(POP_FRONT ${resources_var} ${destination_var} item)
    set(${files_var})
    set(${directories_var})
    set(current_var)

    while (NOT item STREQUAL "_END_GROUP_")
        if (item STREQUAL _FILES_)
            set(current_var ${files_var})
        elseif (item STREQUAL _DIRECTORIES_)
            set(current_var ${directories_var})
        else()
            list(APPEND ${current_var} ${item})
        endif()
        list(POP_FRONT ${resources_var} item)
    endwhile()

endmacro()

macro(_get_resources_parent_directory parent_dir_var target)

    if (APPLE)
        set(${parent_dir_var} $<TARGET_BUNDLE_CONTENT_DIR:${target}>)
    else()
        set(${parent_dir_var} ${CMAKE_BINARY_DIR}>)
        get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

        if (${is_multi_config})
            string(APPEND ${parent_dir_var} /$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:MinSizeRel>:MinSizeRel>)
        endif()
    endif()

endmacro()

macro(_get_resources_directory resources_dir_var target target_type)

    if (APPLE)
        set(${resources_dir_var} Resources)
    else()
        if (${target_type} STREQUAL "SHARED_LIBRARY")
            set(${resources_dir_var} resources/${target})
        else()
            set(${resources_dir_var} resources)
        endif()
    endif()

endmacro()

function(_convert_to_bundle_if_needed target target_type)

    if (APPLE)
        if (target_type STREQUAL "SHARED_LIBRARY")
            get_target_property(is_framework ${target} FRAMEWORK)
            if (NOT is_framework)
                set_target_properties(${target} PROPERTIES
                    FRAMEWORK TRUE
                    MACOSX_BUNDLE_BUNDLE_NAME ${target}
                    MACOSX_FRAMEWORK_IDENTIFIER ${${PROJECT_NAME}_IDENTIFIER}.${target}
                    )
            endif()
        endif()
    endif()

endfunction()
