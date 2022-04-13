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

set(PYTHON_VERSION_MAJOR 3)

if (UNIX AND NOT APPLE)
    set(PYTHON_VERSION_MINOR 8)
else()
    set(PYTHON_VERSION_MINOR 9)
endif()

function(embed_python target)

    get_target_property(binary_dir ${target} BINARY_DIR)
    set(working_dir "${binary_dir}/python")
    file(MAKE_DIRECTORY "${working_dir}")

## #############################################################################
## Download embeddable Python
## #############################################################################

    set(python_zip "${working_dir}/python.zip")

    add_custom_command(OUTPUT ${python_zip}
        COMMAND ${CMAKE_COMMAND} ARGS
        -D PYTHON_ZIP=${python_zip}
        -D PYTHON_VERSION_MINOR=${PYTHON_VERSION_MINOR}
        -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/download_python.cmake"
        VERBATIM
        )

## #############################################################################
## List core Python libraries
## #############################################################################

   if (UNIX)
       set(python_vname python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
       if (APPLE)
           set(libraries lib${python_vname}.dylib)
       else()
           set(libraries lib${python_vname}.so)
       endif()
   elseif (WIN32)
       set(python_vname python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
       set(libraries
           ${python_vname}.dll
           ${python_vname}_d.dll
           ${python_vname}.lib
           ${python_vname}_d.lib
           )
   endif()

   set(core_python_libraries)
   foreach (library ${libraries})
       list(APPEND core_python_libraries "${working_dir}/lib/${library}")
   endforeach()

## #############################################################################
## Extract Python files
## #############################################################################

    add_custom_command(OUTPUT ${core_python_libraries}
        COMMAND ${CMAKE_COMMAND} ARGS
        -D PYTHON_ZIP=${python_zip}
        -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/extract_python.cmake"
        WORKING_DIRECTORY "${working_dir}"
        DEPENDS ${python_zip}
        VERBATIM
        )

## #############################################################################
## Import core Python libraries
## #############################################################################

    foreach (library ${core_python_libraries})
        get_filename_component(library_name "${library}" NAME)
        set(copied_library "${CMAKE_BINARY_DIR}/lib/${library_name}")

        if (APPLE)
            add_custom_command(OUTPUT ${copied_library}
              COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${library}" "${copied_library}"
              COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id "${copied_library}" "${copied_library}"
              DEPENDS "${library}"
              )
        else()
            add_custom_command(OUTPUT ${imported_core_libraries}
              COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${library}" "${copied_library}"
              DEPENDS "${library}"
              )
        endif()

        install(FILES "${copied_library}" TYPE LIB)
        target_sources(${target} PRIVATE ${copied_library})

        if (NOT WIN32)
            target_link_libraries(${target} PUBLIC "${copied_library}")
        endif()
    endforeach()

    if (WIN32)
        target_link_directories(${target} PUBLIC "${working_dir}")
    endif()

## #############################################################################
## Embed Python as external resource
## #############################################################################

    set(vname python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})

    set_external_resources(${target} APPEND DESTINATION ${vname} FILES "${working_dir}/LICENSE.txt")
    set_external_resources(${target} APPEND DESTINATION ${vname}/lib/${vname} DIRECTORIES "${working_dir}/lib/${vname}")

## #############################################################################
## Include directories
## #############################################################################

    target_include_directories(${target} PUBLIC "${working_dir}/include/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")

endfunction()
