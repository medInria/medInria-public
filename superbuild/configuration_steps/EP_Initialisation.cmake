################################################################################
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

macro(ep_Initialisation ep
  USE_SYSTEM use_system_def 
  BUILD_SHARED_LIBS build_shared_libs_def
  REQUIRED_FOR_PLUGINS required_for_plugins
  )
  
## #############################################################################
## Add variable : do we want use the system version ?
## #############################################################################

option(USE_SYSTEM_${ep} 
  "Use system installed version of ${ep}" 
  ${use_system_def}
  )

if (USE_SYSTEM_${ep})
  find_package(${ep} REQUIRED)


## #############################################################################
## Complete superProjectConfig.cmake
## #############################################################################

  if(${required_for_plugins})  
  #  provide path of project needeed for Asclepios and visages plugins 
  file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
    "find_package(${ep} REQUIRED
      PATHS \"${${ep}_DIR}\"
      NO_CMAKE_BUILDS_PATH
      )\n"
    )
  endif()


else()
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##      This part will be run only if USED_SYSTEM_ep is set to OFF
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  if (${required_for_plugins})
    if (DEFINED ${ep}_BINARY_DIR)
      file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
      "find_package(${ep} REQUIRED
        PATHS \"${${ep}_BINARY_DIR}\" 
        PATH_SUFFIXES install build
        NO_CMAKE_BUILDS_PATH
        )\n"
      )
    else()
      file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
        "find_package(${ep} REQUIRED
          PATHS \"${EP_PREFIX_thirdparts}/src/${ep}-build\" 
          PATH_SUFFIXES install build
          NO_CMAKE_BUILDS_PATH
          )\n"
        )
    endif()
  endif()


## #############################################################################
## Add Variable : do we want a static or a dynamic build ?
## #############################################################################
  
  option(BUILD_SHARED_LIBS_${ep} 
    "Build shared libs for ${ep}" 
    ${build_shared_libs_def}
    )
  mark_as_advanced(BUILD_SHARED_LIBS_${ep})
  
  
## #############################################################################
## Set compilation flags
## #############################################################################
  
  set(${ep}_c_flags ${ep_common_c_flags})
  set(${ep}_cxx_flags ${ep_common_cxx_flags})
  set(${ep}_shared_linker_flags ${ep_common_shared_linker_flags})
  
  # Add PIC flag if Static build on UNIX with amd64 arch
  if (UNIX)
    if (NOT BUILD_SHARED_LIBS_${ep} AND 
        "${CMAKE_SYSTEM_PROCESSOR}" MATCHES 64)
        
      set(${ep}_c_flags "${${ep}_c_flags} -fPIC")
      set(${ep}_cxx_flags "${${ep}_cxx_flags} -fPIC")
    endif()
  endif()  

  if (APPLE)
    if (BUILD_SHARED_LIBS_${ep})
      set(${ep}_shared_linker_flags "${${ep}_shared_linker_flags} -headerpad_max_install_names")
    endif()
  endif()

## #############################################################################
## Remove dependencies with other external-project if a system version is used.
## #############################################################################

  foreach(dependence ${${ep}_dependencies})
   if(USE_SYSTEM_${dependence})
    list(REMOVE_ITEM ${ep}_dependencies ${dependence})
   endif()
  endforeach()


## #############################################################################
## Add target dependencies 
## #############################################################################

  # Add dependencies between the target of this project 
  # and the global target from the superproject
  # foreach (target ${global_targets})
  #   add_dependencies(${target} ${ep}-${target})
  # endforeach() 

endif()

endmacro()
