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

function(embed_python target)

    include(python_files_info)

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
## List Python files
## #############################################################################

    foreach (file ${PYTHON_HEADERS})
        list(APPEND python_headers "${working_dir}/${PYTHON_HEADERS_DIR}/${file}")
    endforeach()

    foreach (file ${PYTHON_LIBRARIES})
        if (NOT file STREQUAL ${PYTHON_MAIN_LIBRARY})
            list(APPEND python_libraries "${working_dir}/${PYTHON_LIBRARIES_DIR}/${file}")
        endif()
    endforeach()

    set(python_main_library "${working_dir}/${PYTHON_LIBRARIES_DIR}/${PYTHON_MAIN_LIBRARY}")

    foreach (file ${PYTHON_MODULES})
        list(APPEND python_modules "${working_dir}/${PYTHON_MODULES_DIR}/${file}")
    endforeach()

    set(python_license_file "${working_dir}/${PYTHON_LICENSE_FILE}")

## #############################################################################
## Extract Python files
## #############################################################################

    add_custom_command(OUTPUT ${python_headers} ${python_libraries} ${python_main_library} ${python_modules} ${python_license_file}
        COMMAND ${CMAKE_COMMAND} ARGS
        -D PYTHON_ZIP=${python_zip}
        -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/extract_python.cmake"
        WORKING_DIRECTORY "${working_dir}"
        DEPENDS ${python_zip}
        VERBATIM
        )

## #############################################################################
## Import Python library
## #############################################################################

    set(copied_library "${CMAKE_BINARY_DIR}/lib/${PYTHON_MAIN_LIBRARY}")

    if (APPLE)
        add_custom_command(OUTPUT ${copied_library}
          COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${python_main_library}" "${CMAKE_BINARY_DIR}/lib/"
          COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id "${copied_library}" "${copied_library}"
          DEPENDS "${python_main_library}"
          )
    else()
        add_custom_command(OUTPUT ${copied_library}
          COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different "${python_main_library}" "${CMAKE_BINARY_DIR}/lib/"
          DEPENDS "${python_main_library}"
          )
    endif()

    INSTALL(FILES "${copied_library}" TYPE LIB)

## #############################################################################
## Apply to target
## #############################################################################

    target_include_directories(${target} PUBLIC "${working_dir}/${PYTHON_HEADERS_DIR}")
    target_sources(${target} PRIVATE ${python_headers} ${copied_library})
	IF(NOT WIN32)
        target_link_libraries(${TARGET_NAME} PUBLIC "${copied_library}")
	ELSE()
        #target_link_libraries(${TARGET_NAME} PUBLIC "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:debug>:PYTHON_STUB_LIBRARY_DEBUG>$<$<CONFIG:release>:PYTHON_STUB_LIBRARY>$<$<CONFIG:MinSizeRel>:PYTHON_STUB_LIBRARY>$<$<CONFIG:RelWithDebInfo>:PYTHON_STUB_LIBRARY>)
	    target_link_directories(${TARGET_NAME} PUBLIC "${working_dir}/${PYTHON_LIBRARIES_DIR}")
	ENDIF()

    target_compile_definitions(${target} PUBLIC PYTHON_VERSION_MINOR=${PYTHON_VERSION_MINOR})

    add_external_resources(${target} ${python_libraries} ${python_modules} ${python_license_file})

endfunction()
