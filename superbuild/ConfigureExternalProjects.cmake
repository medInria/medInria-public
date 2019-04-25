##############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2019. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

## #############################################################################
## Add common prefix for all external-projects
## #############################################################################
include(CMakeDependentOption)
option(EP_CHECKBOX_CUSTOM_DIRS  "titi" OFF)
CMAKE_DEPENDENT_OPTION(EP_CHECKBOX_SIDE_BY_SIDE "toto" OFF "NOT EP_CHECKBOX_CUSTOM_DIRS" OFF)
CMAKE_DEPENDENT_OPTION(EP_CHECKBOX_ON_TOP_LEVEL "tata" ON "NOT EP_CHECKBOX_CUSTOM_DIRS" OFF)

macro(ep_change_garde var )
    if(NOT "${${var}_PEVIOUS}" STREQUAL "${${var}}")
        set(${var}_PEVIOUS "${${var}}" CACHE INTERNAL "" FORCE)
        set(${var}_change ON CACHE INTERNAL "" FORCE)
    else()
        set(${var}_change OFF CACHE INTERNAL "" FORCE)
    endif()
endmacro()
    
function(setOnTop)
	set(EP_PREFIX "${EP_PATH_BASE}" CACHE INTERNAL "" FORCE)
    if(EP_CHECKBOX_SIDE_BY_SIDE)
        set(EP_PATH_SOURCE "${EP_PATH_BASE}"   CACHE FILEPATH "" FORCE)
        unset(EP_PATH_BUILD CACHE)
    else()
        set(EP_PATH_SOURCE "${EP_PATH_BASE}/src"   CACHE FILEPATH "" FORCE)
        set(EP_PATH_BUILD  "${EP_PATH_BASE}/build" CACHE FILEPATH "" FORCE)
    endif()
endfunction()

function(setOnSrcDir)
	set(EP_PREFIX "${CMAKE_SOURCE_DIR}/${EP_DIR_NAME}" CACHE INTERNAL "" FORCE)
    set(EP_PATH_SOURCE "${CMAKE_SOURCE_DIR}/${EP_DIR_NAME}" CACHE FILEPATH "" FORCE)
    if(NOT EP_CHECKBOX_SIDE_BY_SIDE)
        set(EP_PATH_BUILD  "${CMAKE_BINARY_DIR}/${EP_DIR_NAME}" CACHE FILEPATH "" FORCE)
    endif()
endfunction()


set(EP_CHECKBOX_ON_TOP_LEVEL_change OFF CACHE INTERNAL "" FORCE)
set(EP_CHECKBOX_SIDE_BY_SIDE_change OFF CACHE INTERNAL "" FORCE)
set(EP_PATH_BASE_change    OFF CACHE INTERNAL "" FORCE)
set(EP_DIR_NAME_change     OFF CACHE INTERNAL "" FORCE)

ep_change_garde(EP_CHECKBOX_ON_TOP_LEVEL)
ep_change_garde(EP_CHECKBOX_SIDE_BY_SIDE)
if(EP_CHECKBOX_CUSTOM_DIRS)
    if(EP_CHECKBOX_ON_TOP_LEVEL_change)
        unset(EP_PATH_BASE CACHE)
        unset(EP_DIR_NAME  CACHE)
		set(EP_PREFIX "${CMAKE_SOURCE_DIR}/ExtProjs" CACHE FILEPATH "" FORCE)
        set(EP_PATH_SOURCE "${CMAKE_SOURCE_DIR}/ExtProjs" CACHE FILEPATH "" FORCE)
		set(EP_PATH_BUILD  "${CMAKE_BINARY_DIR}/ExtProjs" CACHE FILEPATH "" FORCE)
    endif()
else()	
    if(EP_CHECKBOX_ON_TOP_LEVEL)
        if(EP_CHECKBOX_ON_TOP_LEVEL_change)
            unset(EP_DIR_NAME CACHE)
            set(EP_PATH_BASE "${CMAKE_SOURCE_DIR}_ExtProjs" CACHE FILEPATH "Base path of external-projects" FORCE)
        endif()
        ep_change_garde(EP_PATH_BASE)
        if(EP_PATH_BASE_change OR EP_CHECKBOX_SIDE_BY_SIDE_change)
            setOnTop()
        endif()
    else()
        if(EP_CHECKBOX_ON_TOP_LEVEL_change)
            unset(EP_PATH_BASE CACHE)
            set(EP_DIR_NAME "ExtProjs" CACHE FILEPATH "Directory name of external-projects" FORCE)
        endif()
        ep_change_garde(EP_DIR_NAME)
        if(EP_DIR_NAME_change OR EP_CHECKBOX_SIDE_BY_SIDE_change)
            setOnSrcDir()
        endif()
    endif()
endif()

## #############################################################################
## Add common variables for all external-projects
## #############################################################################

set(CMAKE_INSTALL_PREFIX "" )  
mark_as_advanced(CMAKE_INSTALL_PREFIX)

set(ep_common_c_flags 
  "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}"
  )

set(ep_common_cxx_flags 
  "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}"
  )

set(ep_common_shared_linker_flags
  "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_INIT} ${ADDITIONAL_SHARED_LINKER_FLAGS}"
  )

set(ep_common_cache_args
  -DCMAKE_C_COMPILER:=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER:=${CMAKE_CXX_COMPILER}
  -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
  -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
  )

if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} -G ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

if(${USE_GITHUB_SSH})
  set(GITHUB_PREFIX git@github.com:)
else()
  set(GITHUB_PREFIX https://github.com/)
endif()

if(${USE_GITLAB_INRIA_SSH})
  set(GITLAB_INRIA_PREFIX git@gitlab.inria.fr:)
else()
  set(GITLAB_INRIA_PREFIX https://gitlab.inria.fr/)
endif()

## #############################################################################
## Include cmake modules of external-project
## #############################################################################

include(ExternalProject) 

## #############################################################################
## Include common configuration steps
## #############################################################################

include(EP_Initialisation)
include(EP_GeneratePatchCommand) 

## #############################################################################
## Include specific module of each project
## #############################################################################

file(GLOB projects_modules ${CMAKE_SOURCE_DIR}/superbuild/projects_modules/*.cmake)

foreach(module ${projects_modules})
    include(${module})
endforeach()

## #############################################################################
## Call specific module of each project
## #############################################################################

macro(call func_name)
    set(tmp_function_file ${CMAKE_CURRENT_BINARY_DIR}/tmp/tmp_call_ep.cmake)
    string(REPLACE "-" "_" func ${func_name})
    file(WRITE ${tmp_function_file} "${func}()")
    include(${tmp_function_file})
    file(REMOVE ${tmp_function_file})
endmacro()

foreach (external_project ${external_projects})
    call(${external_project}_project)
endforeach()
