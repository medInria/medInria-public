## #############################################################################
## Check which patch has to be applied
## #############################################################################

function(ep_GeneratePatchCommand ep OutVar)
    find_program(GIT_BIN NAMES git)
    foreach (patch ${ARGN})
        execute_process(COMMAND ${GIT_BIN} apply --ignore-whitespace  --check ${CMAKE_SOURCE_DIR}/superbuild/patches/${patch}
                        WORKING_DIRECTORY ${EP_PATH_SOURCE}/${ep}
                        RESULT_VARIABLE PATCH_OK
                        OUTPUT_QUIET
                        ERROR_QUIET)
        if (PATCH_OK EQUAL 0 OR NOT EXISTS ${EP_PATH_SOURCE}/${ep})
            set(PATCHES_TO_APPLY ${PATCHES_TO_APPLY} ${CMAKE_SOURCE_DIR}/superbuild/patches/${patch})
        endif()
    endforeach()

    set(PATCH_COMMAND "")
    if (NOT "${PATCHES_TO_APPLY}" STREQUAL "")
        set(PATCH_COMMAND ${GIT_BIN} apply --ignore-whitespace ${PATCHES_TO_APPLY})
    endif()

    set(${OutVar} ${PATCH_COMMAND} PARENT_SCOPE)
endfunction()
