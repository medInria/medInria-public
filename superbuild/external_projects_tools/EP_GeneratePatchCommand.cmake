## #############################################################################
## Check which patch has to be applied
## #############################################################################
macro(directory_count_elements ep_path elements_count_res)
    file(GLOB RESULT ${ep_path}/*)
    list(LENGTH RESULT ${elements_count_res})
endmacro()


function(ep_GeneratePatchCommand ep OutVar)
    find_program(GIT_BIN NAMES git)
    foreach (patch ${ARGN})
        set(PATCHES_TO_APPLY_CUR ${CMAKE_SOURCE_DIR}/superbuild/patches/${patch})
		directory_count_elements(${EP_PATH_SOURCE}/${ep} count)
		
        if(NOT ${count} EQUAL 0) #Check if external_project is already cloned
            execute_process(COMMAND ${GIT_BIN} apply --reverse --ignore-whitespace --check ${CMAKE_SOURCE_DIR}/superbuild/patches/${patch}
                            WORKING_DIRECTORY ${EP_PATH_SOURCE}/${ep}
                            RESULT_VARIABLE PATCH_ALREADY_APPLIED
                            OUTPUT_QUIET
                            ERROR_QUIET)
            				
            if (PATCH_ALREADY_APPLIED EQUAL 0) #Like all OS system command 0 is Ok, not 0 is bad.
            	# The patch is already applied
            	message("The patch ${patch} for the external project ${ep} is already applied")
            	set (PATCHES_TO_APPLY_CUR "") #Then erase PATCHES_TO_APPLY_CUR
            else()
                execute_process(COMMAND ${GIT_BIN} apply --ignore-whitespace --check ${CMAKE_SOURCE_DIR}/superbuild/patches/${patch}
                                WORKING_DIRECTORY ${EP_PATH_SOURCE}/${ep}
                                RESULT_VARIABLE PATCH_APPLICABLE
                                OUTPUT_QUIET
                                ERROR_QUIET)
            	if(PATCH_APPLICABLE EQUAL 0) #Like all OS system command 0 is Ok, not 0 is bad.
        		    #Do nothing the value is already set
        		    message("The patch ${patch} will be applied on pre-existing ${EP_PATH_SOURCE}/${ep}")
        		else()
            		message(FATAL_ERROR "The patch ${patch} for the external project ${ep} is NOT APPLICABLE")
            	endif()
            endif()
        endif()
		set(PATCHES_TO_APPLY ${PATCHES_TO_APPLY} ${PATCHES_TO_APPLY_CUR})
    endforeach()

    set(PATCH_COMMAND "")
    if (NOT "${PATCHES_TO_APPLY}" STREQUAL "")
        set(PATCH_COMMAND ${GIT_BIN} apply --ignore-whitespace ${PATCHES_TO_APPLY})
    endif()

    set(${OutVar} ${PATCH_COMMAND} PARENT_SCOPE)
endfunction()