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

function(generate_python_resources target)

    get_target_property(package_name ${target} PYTHON_PACKAGE_NAME)
    if (NOT package_name)
        set(package_name ${target})
    endif()

    set(modules_archive "${CMAKE_CURRENT_BINARY_DIR}/python/modules/${target}.zip")
    set(modules_dir "${CMAKE_CURRENT_BINARY_DIR}/python/modules/${package_name}")
    set(libraries_dir "${CMAKE_CURRENT_BINARY_DIR}/python/libraries/${package_name}")

    get_target_property(modules ${target} PYTHON_MODULES)
    get_target_property(bindings ${target} PYTHON_BINDINGS)

    if (NOT modules)
        set(modules "")
    endif()

    if (NOT "__init__" IN_LIST modules)
        # Create default __init__ file
        set(init_file "${modules_dir}/__init__.py")
        set(init_content "# Generated\n")
        file(WRITE ${init_file} ${init_content})
        list(APPEND modules "__init__")
    endif()

    foreach (module ${modules})
        list(APPEND module_paths "${modules_dir}/${module}.py")
        list(APPEND depends "${modules_dir}/${module}.py")
    endforeach()

    if (bindings)
        foreach(bindings_target ${bindings})
            get_target_property(module ${bindings_target} OUTPUT_NAME)
            get_target_property(library_prefix ${bindings_target} PREFIX)

            if (WIN32)
                get_target_property(library_suffix ${bindings_target} SUFFIX)
            else()
                set(library_suffix ".so")
            endif()

            set(library_name ${library_prefix}${module})
            set(library ${libraries_dir}/${library_name}${library_suffix})
            set(forward_module "${modules_dir}/${library_name}.py")
            file(WRITE ${forward_module} "from ${library_name} import *")

            list(APPEND module_paths
                "${modules_dir}/${module}.py"
                ${forward_module}
                )

            list(APPEND depends ${bindings_target})

            add_external_resources(${target} ${library})
        endforeach()
    endif()

    add_custom_command(OUTPUT ${modules_archive}
        COMMAND ${CMAKE_COMMAND} -E tar c ${modules_archive} --format=zip -- ${init_file} ${module_paths}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python/modules
        DEPENDS ${depends}
        VERBATIM
        )

    add_external_resources(${target} ${modules_archive})

endfunction()
