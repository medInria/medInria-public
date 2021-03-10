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
# Usage: add_external_resources(target [RCC name] [PREFIX prefix] resources...)
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
#   The following options can be used:
#
#   RCC: When this option is used, the input resources must be directories, and
#        their contents will be compiled into a single Qt binary resource file
#        called "name.rcc". This file can then be registered using code like:

#        QResource::registerResource(med::getExternalResourcePath("name.rcc"...)

#        This allows the resources to be accessed in the same way as compiled-in
#        resources i.e. QFile(:/prefix/my_resource.foo)
#
#   PREFIX: Add a resource prefix to the qrc used to generate the rcc file (to
#           use with the RCC option).
#
################################################################################

  cmake_parse_arguments(resources "" "RCC;PREFIX" "" ${ARGN})
  set(resources ${resources_UNPARSED_ARGUMENTS})

  get_target_property(target_type ${target} TYPE)

  if ((NOT target_type STREQUAL "EXECUTABLE")
      AND
      (NOT target_type STREQUAL "SHARED_LIBRARY"))
    message(FATAL_ERROR "Target must be an executable or a shared library")
  endif()

  if (resources)

    # make the rcc file if requested
    if (resources_RCC)
      set(qrc_file "${CMAKE_CURRENT_BINARY_DIR}/resources/${resources_RCC}/${resources_RCC}.qrc")
      set(rcc_file "${CMAKE_CURRENT_BINARY_DIR}/resources/${resources_RCC}/${resources_RCC}.rcc")
      set(dependency_files)

      file(WRITE "${qrc_file}" "<RCC><qresource")

      if (resources_PREFIX)
        file(APPEND "${qrc_file}" " prefix=\"/" ${resources_PREFIX} "\"")
      endif()

      file(APPEND "${qrc_file}" ">")

      foreach (resource_dir ${resources})
        file(GLOB_RECURSE resource_files "${resource_dir}"
                "${resource_dir}/*"
                )
        foreach(resource_file ${resource_files})
          file(RELATIVE_PATH relative_resource_file "${resource_dir}" "${resource_file}")
          file(APPEND "${qrc_file}" "<file>${relative_resource_file}</file>")

          # create a target to copy the resource file to a location relative to the qrc file
          set(output_file "${CMAKE_CURRENT_BINARY_DIR}/resources/${resources_RCC}/${relative_resource_file}")
          add_custom_command(
            OUTPUT "${output_file}"
            COMMAND ${CMAKE_COMMAND} ARGS -E copy "${resource_file}" "${output_file}"
            )
          list(APPEND dependency_files "${resource_file}" "${output_file}")
        endforeach()
      endforeach()

      file(APPEND "${qrc_file}" "</qresource></RCC>")

      qt5_add_binary_resources(${target}_${resources_RCC}_RCC ${qrc_file}
        DESTINATION ${rcc_file}
        )

      # add dependencies to regenerate the rcc file if the resources change
      add_custom_target(${target}_${resources_RCC}_FILES ALL
        DEPENDS ${dependency_files})
      add_dependencies(${target}_${resources_RCC}_RCC ${target}_${resources_RCC}_FILES)

      # now the only resource will be the rcc file
      set(resources ${rcc_file})
    endif()

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
  endif()

endmacro()
