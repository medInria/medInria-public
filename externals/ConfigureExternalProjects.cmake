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
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
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

file(GLOB projects_modules 
  "${CMAKE_SOURCE_DIR}/externals/projects_modules/*.cmake"
  )

foreach(module ${projects_modules})
    include(${module})
endforeach()

## #############################################################################
## Call specific module of each project
## #############################################################################

macro(call func_name)
    set(tmp_function_file ${CMAKE_BINARY_DIR}/externals/tmp/tmp_call_ep.cmake)
    string(REPLACE "-" "_" func ${func_name})
    file(WRITE ${tmp_function_file} "${func}()")
    include(${tmp_function_file})
    file(REMOVE ${tmp_function_file})
endmacro()

foreach (external_project ${external_projects})
    call(${external_project}_project)
    
    
# ## #############################################################################
# ## Add custom targets update, and build to explicitly update and rebuild all.
# ## #############################################################################

#     if(update-${external_project})
#       set(update_dependencies ${update_dependencies} update-${external_project})
#     endif()
#     if(build-${external_project})
#       set(build_dependencies ${build_dependencies} build-${external_project})
#     endif()
endforeach()


# add_custom_target(update
#   DEPENDS ${update_dependencies}
#   COMMAND echo All project have been updated. 
#   && echo To build them, run the build target: 'cmake --build . --target build'
#   )
  
# add_custom_target(build
#   DEPENDS ${build_dependencies}
#   )
