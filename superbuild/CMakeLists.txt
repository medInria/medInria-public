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

## #############################################################################
## Add packages
## #############################################################################

find_package(Qt5 REQUIRED COMPONENTS
  Core
  Designer
  Gui
  Network
  OpenGL
  Sql                                                                                                                                                                                                                                       
  Widgets                                                                                                                                                                                                                                       
  Xml
  Test
  Quick
  )

find_package(Boost REQUIRED)

## #############################################################################
## Add exteral projects
## #############################################################################
                                                                                                                                                                                                                                          
# There are three possibilities:
# - if the option USE_SYSTEM_${external_project} is set, then the system version  
#   will be used (not yet fully tested)
# - if the option USE_SYSTEM_${external_project} is not set:
#   * there is a subdir with the package name, in which case this subdir is used 
#     directly
#   * otherwise use download and compile locally the package as an external 
#     module.

option(USE_DTKIMAGING "Use DTK imaging layer" OFF )
  
option(USE_OSPRay "Use OSPRay for CPU VR" OFF )

if (NOT WIN32)
  option(USE_FFmpeg "Build with FFmpeg video export support" OFF)
  if (USE_FFmpeg)
      list(APPEND external_projects ffmpeg)
  endif()
endif()
  
list(APPEND external_projects
  VTK 
  ITK 
  RPI 
  TTK 
  DCMTK 
  QtDCM 
  dtk
  LogDemons
  )

if (USE_DTKIMAGING)
  list(APPEND external_projects
      dtkImaging
      medInria
      )
else()
  list(APPEND external_projects
      medInria
      )
endif()

if (USE_OSPRay)
  SET(ospray_DIR CACHE PATH "The directory containing a CMAKE configuration file for OSPRay")
  SET(OSPRAY_INSTALL_DIR CACHE PATH "Install location of OSPRay")
endif()



## #############################################################################
## Setup build configuration
## #############################################################################

unset(CMAKE_BUILD_TYPE CACHE)
set(CMAKE_BUILD_TYPE_externals_projects Release CACHE STRING "Build type configuration for externals projects libraries.")
set(CMAKE_BUILD_TYPE_medInria Debug CACHE STRING "Build type configuration specific to medInria.")


## #############################################################################
## medInria dirs.
## #############################################################################
set(medInria_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(medInria_BINARY_DIR ${CMAKE_BINARY_DIR}/medInria-build)
set(medInria_STAMP_DIR ${CMAKE_BINARY_DIR}/medInria-stamp)


## #############################################################################
## Add modules
## #############################################################################

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/external_projects_tools
  ${CMAKE_MODULE_PATH}
    )

option(USE_GITHUB_SSH 
  "Use by default Git SSH addresses, requires public key set on github" OFF
  )
  
option(USE_GITLAB_INRIA_SSH 
  "Use by default Git SSH addresses, requires public key set on Inria gitlab" OFF
  )

# Add CMAKE_MODULE_PATH to superProjectConfig.cmake, usefull for 
# Asclepios and visages plugins
set(${PROJECT_NAME}_CONFIG_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
file(WRITE ${${PROJECT_NAME}_CONFIG_FILE}
  "set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH}
    \${CMAKE_MODULE_PATH}
    )\n\n
    set(USE_GITHUB_SSH ${USE_GITHUB_SSH})\n"
  )

# Add CMAKE_MODULE_PATH to superProjectConfig.cmake, usefull for 
# Asclepios and visages plugins
file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
  "set(USE_GITLAB_INRIA_SSH ${USE_GITLAB_INRIA_SSH})\n"
  )
  
# Add Qt dir
file(APPEND ${${PROJECT_NAME}_CONFIG_FILE}
  "set(Qt5_DIR ${Qt5_DIR})\n\n"
  )

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/projects_modules
  ${CMAKE_SOURCE_DIR}/packaging
  ${CMAKE_MODULE_PATH}
  )

set(PRIVATE_PLUGINS_DIRS "" CACHE PATH "Folders containing private plugins, separated by ';'")
set(PRIVATE_PLUGINS_LEGACY_DIRS "" CACHE PATH "Folders containing legacy private plugins, separated by ';'")

set(EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS "")

## #############################################################################
## Add Targets
## #############################################################################

set(global_targets 
  configure  
  install
  )
  
# This adds targets that will be run in each external-projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS ${global_targets})


## #############################################################################
## Check build environment
## #############################################################################

include(EP_CheckEnvironment)

## #############################################################################
## Configure external-projects
## #############################################################################

include(ConfigureExternalProjects)

## #############################################################################
## Package
## #############################################################################

include(Packaging)
