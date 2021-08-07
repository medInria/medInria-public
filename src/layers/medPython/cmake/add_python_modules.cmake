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

function(add_python_modules package)

    set(modules_dir "${PYTHON_BUILD_DIR}/modules/${package}")

    foreach (module ${ARGN})
        get_filename_component(module_name ${module} NAME)
        # Copy modules to the build folder (tar needs them all in one place)
        set(module_copy "${modules_dir}/${module_name}")
        add_custom_command(OUTPUT ${module_copy}
          COMMAND ${CMAKE_COMMAND} ARGS -E copy "${module}" "${module_copy}"
          )
        list(APPEND ${package}_PYTHON_MODULES ${module_name})
    endforeach()

    set(${package}_PYTHON_MODULES ${${package}_PYTHON_MODULES} PARENT_SCOPE)

endfunction()
