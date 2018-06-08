##############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

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
