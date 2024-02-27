################################################################################
#
# medInria
#
# Copyright (c) INRIA 2021. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

macro(add_external_resources target)

################################################################################
#
# Usage: add_external_resources(target resources...)
#
# External resources will he handled as follows:
#
#   macOS:    The files will be placed in the resource folder of the bundle
#             (for the exectuble) or the framework (for shared libraries).
#
#   other OS: At post build time the files will be copied to
#             ${PROJECT_NAME}_RESOURCES_DIR (and inside a subfolder if target is
#             a libray). When making the package, a 'resources' folder with
#             the same structure will be created in the package root.
#
################################################################################

  set(resources ${ARGN})

  get_target_property(target_type ${target} TYPE)

  if ((NOT target_type STREQUAL "EXECUTABLE")
      AND
      (NOT target_type STREQUAL "SHARED_LIBRARY"))
    message(FATAL_ERROR "Target must be an executable or a shared library")
  endif()

  if (resources)

    target_sources(${target} PRIVATE ${resources})

    if (APPLE)

      # if the target is a library, we must build it as a framework to contain
      # the resources.
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

    else() # NOT APPLE

      # at post build time we copy the resources to the build directory.

      set(build_dir "${${PROJECT_NAME}_RESOURCES_DIR}")

      # use subfolders for library-specific resources to avoid name collisions
      if (target_type STREQUAL "SHARED_LIBRARY")
        set(build_dir "${build_dir}/${target}")
      endif()

      foreach(resource ${resources})
        get_filename_component(filename "${resource}" NAME)
        set(output_file "${build_dir}/${filename}")
        add_custom_command(TARGET ${target} POST_BUILD
          BYPRODUCTS "${output_file}"
          COMMAND ${CMAKE_COMMAND} ARGS -E copy "${resource}" "${output_file}"
          )
        list(APPEND copied_resources "${output_file}")
      endforeach()

      set(resources "${copied_resources}")

    endif()

    # allows the resources to be handled by the install command during
    # packaging.
    set_property(TARGET ${target} APPEND PROPERTY RESOURCE "${resources}")

  endif()

endmacro()
