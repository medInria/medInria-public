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
# Usage: add_external_resources(target
#                               [SUBDIRECTORY subdirectory]
#                               [FILES [files...]]
#                               [DIRECTORIES [directories...]])
#
# The options are:
#
# SUBDIRECTORY
#     The resources will be placed in the specified subdirectory of the target
#     resource folder.
#
# FILES
#     Specify resources files to add for the target.
#
# DIRECTORIES
#     Specify resource directories to add for the target. The contents of each
#     directory will be copied to the destination folder.
#
# External resources will he handled as follows:
#
#   macOS:    The files will be placed in the resource folder of the bundle
#             (for the exectuble) or the framework (for shared libraries).
#
#   other OS: At post build time the files will be copied to
#             ${CMAKE_BINARY_DIR}/resources (and inside a subfolder if target is
#             a libray). When making the package, a 'resources' folder with
#             the same structure will be created in the package root.
#
################################################################################

  cmake_parse_arguments(args
    ""
    "SUBDIRECTORY"
    "FILES;DIRECTORIES"
    ${ARGN}
    )

  get_target_property(target_type ${target} TYPE)

  if ((NOT target_type STREQUAL "EXECUTABLE")
      AND
      (NOT target_type STREQUAL "SHARED_LIBRARY"))
    message(FATAL_ERROR "Target must be an executable or a shared library")
  endif()

  if (args_FILES OR args_DIRECTORIES)
      if (NOT APPLE)
          # On Linux and Windows, use subfolders for library-specific resources
          # to avoid name collisions.
          if (target_type STREQUAL "SHARED_LIBRARY")
              set(resources_dir resources/${target})
          else()
              set(resources_dir resources)
          endif()
      else()
          # on macOS, if the target is a library we must build it as a framework
          # to contain the resources.
          if (target_type STREQUAL "SHARED_LIBRARY")
              get_target_property(is_framework ${target} FRAMEWORK)
              if (NOT is_framework)
                  set_target_properties(${target} PROPERTIES
                      FRAMEWORK TRUE
                      MACOSX_BUNDLE_BUNDLE_NAME ${target}
                      MACOSX_FRAMEWORK_IDENTIFIER ${${PROJECT_NAME}_IDENTIFIER}.${target}
                      )
              endif()
              set(resources_dir Resources)
          else()
              set(resources_dir Contents/Resources)
          endif()
      endif()

      if (args_SUBDIRECTORY)
          set(resources_dir ${resources_dir}/${args_SUBDIRECTORY})
      endif()

      if (NOT APPLE)
          add_custom_command(TARGET ${target} POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E make_directory ${resources_dir}
              WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
              )

          if (args_FILES)
              add_custom_command(TARGET ${target} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${args_FILES}" .
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${resources_dir}
                  )
              install(FILES ${args_FILES} DESTINATION ${resources_dir})
          endif()

          if (args_DIRECTORIES)
              add_custom_command(TARGET ${target} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory "${args_DIRECTORIES}" .
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${resources_dir}
                  )
              foreach(directory ${args_DIRECTORIES})
                  list(APPEND directories directory/)
              endforeach()
              install(DIRECTORY ${directories} DESTINATION ${resources_dir})
          endif()
      else()
          add_custom_command(TARGET ${target} POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E make_directory ${resources_dir}
              WORKING_DIRECTORY $<TARGET_BUNDLE_DIR:${target}>
              )

          if (args_FILES)
              add_custom_command(TARGET ${target} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${args_FILES}" .
                  WORKING_DIRECTORY $<TARGET_BUNDLE_DIR:${target}>/${resources_dir}
                  )
          endif()

          if (args_DIRECTORIES)
              add_custom_command(TARGET ${target} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory "${args_DIRECTORIES}" .
                  WORKING_DIRECTORY $<TARGET_BUNDLE_DIR:${target}>/${resources_dir}
                  )
          endif()
      endif()
  endif()

endmacro()
