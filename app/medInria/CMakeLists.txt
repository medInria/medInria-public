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

project(medInria)

## #############################################################################
## List Sources
## #############################################################################

add_subdirectory(QSingleApplication)
add_subdirectory(ressources)

file(GLOB HEADER
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADER}
  ${${PROJECT_NAME}_H}
  )

file(GLOB CPP
  *.cpp
  )
set(${PROJECT_NAME}_CPP
  ${CPP}
  ${${PROJECT_NAME}_CPP}
  )

## #############################################################################
## include directorie.
## #############################################################################

##  see cmake/module/list_header_directories_to_include.cmake
list_header_directories_to_include(${PROJECT_NAME} 
  ${${PROJECT_NAME}_H}
  )

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS} 
  ${MEDINRIA_INCLUDE_DIRS}
  )  

## #############################################################################
## Add preproc if we have revisions from the super project
## #############################################################################

if (EXISTS ${CMAKE_BINARY_DIR}/revisions/medRevisions.h)
  include_directories(${CMAKE_BINARY_DIR}/revisions)
  set(${PROJECT_NAME}_HEADERS
  medRevisions.h
  ${${PROJECT_NAME}_HEADERS}
  )
  add_definitions(-DMEDINRIA_HAS_REVISIONS)
endif()


## #############################################################################
## Add preproc if we want immersive room stuff
## #############################################################################

option(ACTIVATE_WALL_OPTION 
  "Activate the command line option --wall (INRIA Sophia, immersive room)"
  OFF
  )
if (ACTIVATE_WALL_OPTION)
  add_definitions(-DACTIVATE_WALL_OPTION)
endif()


## #############################################################################
## OS specificites
## #############################################################################

if(WIN32)
  option(MED_SHOW_CONSOLE 
    "Show console at runtime (mainly for debugging)"
    OFF
    )
endif()


if (APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()
if (WIN32)
  
  if(NOT ${MED_SHOW_CONSOLE} )
    set(DEPLOYMENT_SYSTEM WIN32)
  endif()
  
  set(${PROJECT_NAME}_RC ressources/medInria.rc)
endif()


## #############################################################################
## Precompile headers
## #############################################################################

if (MEDINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_PCH
    ${MEDPCH_H}
    ${MEDPCH_CPP}
    ${${PROJECT_NAME}_CPP}
    )
endif()


## #############################################################################
## Add Exe
## #############################################################################

qt4_add_resources(${PROJECT_NAME}_QRC 
  ${${PROJECT_NAME}_QRC}
  )

add_executable(${PROJECT_NAME} ${DEPLOYMENT_SYSTEM} # Empty for Linux
  ${${PROJECT_NAME}_CPP}
  ${${PROJECT_NAME}_H}   # Just to have it in EDI
  ${${PROJECT_NAME}_QRC}
  ${${PROJECT_NAME}_QSS} # Just to have it in EDI
  ${${PROJECT_NAME}_RC}  # Empty for Unix 
  )


## #############################################################################
## Links.
## #############################################################################

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES}
  dtkCore
  dtkScript
  dtkGui
  dtkVr
  medCore
  medGui
  medCore
  medSql
  medPacs
  )


## #############################################################################
## Bundle setup
## #############################################################################

#TODO : Not sure if it is still usefull, if it is, it should be mooved 
#       to the superproject IMO.

if (APPLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME 
    ${PROJECT_NAME}
    )
  set(MACOSX_BUNDLE_ICON_FILE 
    medInria.icns
    )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING 
    ${${PROJECT_NAME}_VERSION}
    )
  set(MACOSX_BUNDLE_BUNDLE_VERSION 
    ${${PROJECT_NAME}_VERSION}
    )
  set(MACOSX_BUNDLE_LONG_VERSION_STRING 
    "Version ${${PROJECT_NAME}_VERSION}"
    )
  set(${PROJECT_NAME}_RESOURCE_DIR 
    ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/Resources
    )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${PROJECT_NAME}_RESOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ressources/medInria.icns ${${PROJECT_NAME}_RESOURCE_DIR}
    )
endif()

## #############################################################################
## install
## #############################################################################

## see cmake/module/set_lib_install_rules.Cmake
set_lib_install_rules(${PROJECT_NAME})


#TODO : Not sure if it is still usefull, if it is, it should be mooved 
#       to the superproject IMO.

if (APPLE)
  install(CODE "
  execute_process(COMMAND 
    \${QT_BINARY_DIR}/macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app
    )
	execute_process(COMMAND 
      \${dtk_DIR}/bin/dtkDeploy \${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app -inject-dir=\${CMAKE_CURRENT_BINARY_DIR}/plugins
	  )
  ")
endif()





