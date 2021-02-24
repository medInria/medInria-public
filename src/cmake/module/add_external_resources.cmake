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

macro(add_external_resources target resource1)

################################################################################
#
# Usage: add_external_resources(target resource1 resource2 ...)
#
# External resources will he handled as follows:
#
#   macOS:    The files will be placed in the resource folder of the bundle
#             (for the exectuble) or the framework (for shared libraries).
#
#   other OS: At build time the files will be copied to
#             ${PROJECT_NAME}_RESOURCES_DIR (and inside a subfolder if target is
#             a libray). When making the package, a 'resources' folder with
#             the same structure will be created in the package root.
#
################################################################################

  get_target_property(target_type ${target} TYPE)

  if ((NOT target_type STREQUAL "EXECUTABLE")
      AND
      (NOT target_type STREQUAL "SHARED_LIBRARY"))
    message(FATAL_ERROR "Target must be an executable or a shared library")
  endif()

  set(resources "${resource1}" ${ARGN})
  target_sources(${target} PRIVATE ${resources})
  set_target_properties(${target} PROPERTIES RESOURCE "${resources}")

  if (APPLE)
### MACOS RULES

    # if the target is a library, we must build it as a framework to contain
    # the resources.
    if (target_type STREQUAL "SHARED_LIBRARY")
      set_target_properties(${target} PROPERTIES
        FRAMEWORK TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${target}
        MACOSX_FRAMEWORK_IDENTIFIER ${${PROJECT_NAME}_IDENTIFIER}.${target}
        )
    endif()

  else()
### LINUX AND WINDOWS RULES

    set(build_dir "${${PROJECT_NAME}_RESOURCES_DIR}")

    # use subfolders for library-specific resources to avoid name collisions
    if (target_type STREQUAL "SHARED_LIBRARY")
      set(build_dir "${build_dir}/${target}")
    endif()

    foreach(resource ${resources})
      get_filename_component(filename "${resource}" NAME)
      set(output_file "${build_dir}/${filename}")
      add_custom_command(
        OUTPUT "${output_file}"
        COMMAND ${CMAKE_COMMAND} ARGS -E copy "${resource}" "${output_file}"
        )
      string(REPLACE " " "_" target_suffix "${filename}")
      add_custom_target(${target}_resource_${target_suffix} ALL
        DEPENDS "${resource}" "${output_file}")
    endforeach()

  endif()

endmacro()
