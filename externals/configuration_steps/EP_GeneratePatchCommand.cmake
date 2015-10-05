## #############################################################################
## Check which patch has to be applied
## #############################################################################

function(ep_GeneratePatchCommand ep OutVar)
    foreach (patch ${ARGN})
        execute_process(COMMAND git apply --ignore-whitespace  --check ${CMAKE_SOURCE_DIR}/patches/${patch}
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${ep}
                        RESULT_VARIABLE   PATCH_OK
                        OUTPUT_QUIET
                        ERROR_QUIET)
        if (PATCH_OK EQUAL 0 OR NOT EXISTS ${CMAKE_SOURCE_DIR}/${ep})
            set(PATCHES_TO_APPLY ${PATCHES_TO_APPLY} ${CMAKE_SOURCE_DIR}/patches/${patch})
        endif()
    endforeach()

    set(PATCH_COMMAND)
    if (NOT "${PATCHES_TO_APPLY}" STREQUAL "")
        set(PATCH_COMMAND git apply --ignore-whitespace ${PATCHES_TO_APPLY})
    endif()

    set(${OutVar} ${PATCH_COMMAND} PARENT_SCOPE)
endfunction()
