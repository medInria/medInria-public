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

macro(ep_Initialisation ep)

cmake_parse_arguments(ep_Initialisation
    "NO_CMAKE_PACKAGE"
    "USE_SYSTEM;BUILD_SHARED_LIBS;REQUIRED_FOR_PLUGINS;PACKAGE_NAME"
    ""
    ${ARGN}
    )

if (NOT ep_Initialisation_USE_SYSTEM)
    set(ep_Initialisation_USE_SYSTEM OFF)
endif()

if (NOT ep_Initialisation_BUILD_SHARED_LIBS)
    set(ep_Initialisation_BUILD_SHARED_LIBS ON)
endif()

if (NOT ep_Initialisation_REQUIRED_FOR_PLUGINS)
    set(ep_Initialisation_REQUIRED_FOR_PLUGINS OFF)
endif()

if (NOT ep_Initialisation_PACKAGE_NAME)
    set(ep_Initialisation_PACKAGE_NAME ${ep})
endif()
  
## #############################################################################
## Add variable : do we want use the system version ?
## #############################################################################

option(USE_SYSTEM_${ep} 
  "Use system installed version of ${ep}" 
  ${ep_Initialisation_USE_SYSTEM}
  )

if (USE_SYSTEM_${ep})

    if(NOT ep_Initialisation_NO_CMAKE_PACKAGE)
        find_package(${ep_Initialisation_PACKAGE_NAME} REQUIRED)

## #############################################################################
## Complete superProjectConfig.cmake
## #############################################################################

        if(ep_Initialisation_REQUIRED_FOR_PLUGINS)
            #  provide path of project needeed for Asclepios and visages plugins
            file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
                "find_package(${ep_Initialisation_PACKAGE_NAME} REQUIRED
                    PATHS \"${${ep_Initialisation_PACKAGE_NAME}_DIR}\"
                    )\n"
                )
        endif()
    endif()

else()
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##      This part will be run only if USED_SYSTEM_ep is set to OFF
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  if(NOT ep_Initialisation_NO_CMAKE_PACKAGE)
      if (ep_Initialisation_REQUIRED_FOR_PLUGINS)
          if (DEFINED ${ep}_BINARY_DIR)
              file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
                  "find_package(${ep_Initialisation_PACKAGE_NAME} REQUIRED
                      PATHS \"${${ep_Initialisation_PACKAGE_NAME}_BINARY_DIR}\"
                      PATH_SUFFIXES install build
                      )\n"
                  )
          else()
              if(DEFINED EP_PATH_BUILD)
                  set(build_dir ${EP_PATH_BUILD})
              else()
                  set(build_dir "${EP_PATH_SOURCE}/${ep}-build" )
              endif()

              file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
                  "find_package(${ep} REQUIRED
                      PATHS \"${build_dir}\"
                      PATH_SUFFIXES install build
                      )\n"
                  )
          endif()
      endif()
  endif()


## #############################################################################
## Add Variable : do we want a static or a dynamic build ?
## #############################################################################
  
  option(BUILD_SHARED_LIBS_${ep} 
    "Build shared libs for ${ep}" 
    ${ep_Initialisation_BUILD_SHARED_LIBS}
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
