## #############################################################################
## Generate patch command for an external project. To use with ExternalProject_Add
## #############################################################################
function(ep_GeneratePatchCommand ep OutVar patch)
    set(PATCH_COMMAND ${CMAKE_COMMAND}
                      -Dep:STRING=${ep}
                      -DMEDINRIA_SOURCE_DIR:STRING=${CMAKE_SOURCE_DIR}
                      -DEP_PATH_SOURCE:STRING=${EP_PATH_SOURCE}
                      -Dpatch:STRING=${patch} 
                      -P
                      ${CMAKE_SOURCE_DIR}/superbuild/external_projects_tools/EP_PatcherScript.cmake)

    set(${OutVar} ${PATCH_COMMAND} PARENT_SCOPE)
endfunction()
