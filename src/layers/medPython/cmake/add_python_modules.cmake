################################################################################
#
# medInria
#
# Copyright (c) INRIA 2021. All rights reserved.
#
# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

function(add_python_modules target)

    cmake_parse_arguments(args
        ""
        "SOURCE_DIRECTORY"
        ""
        ${ARGN}
        )

    if (DEFINED args_SOURCE_DIRECTORY)
        set(source_directory ${args_SOURCE_DIRECTORY})
    else()
        set(source_directory ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    get_target_property(package_name ${target} PYTHON_PACKAGE_NAME)
    if (NOT package_name)
        set(package_name ${target})
    endif()

    set(modules_dir ${CMAKE_CURRENT_BINARY_DIR}/python/modules/${package_name})

    foreach (module ${args_UNPARSED_ARGUMENTS})
        get_filename_component(module_name ${module} NAME_WE)
        # Copy modules to the build folder (tar needs them all in one place)
        set(module_copy "${modules_dir}/${module_name}.py")
        add_custom_command(OUTPUT ${module_copy}
          COMMAND ${CMAKE_COMMAND} ARGS -E copy "${module}" "${module_copy}"
          WORKING_DIRECTORY ${source_directory}
          DEPENDS ${source_directory}/${module}
          )
        set_property(TARGET ${target} APPEND PROPERTY PYTHON_MODULES ${module_name})
    endforeach()

endfunction()
