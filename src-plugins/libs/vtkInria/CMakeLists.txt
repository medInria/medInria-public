#######################################################################
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
#######################################################################

cmake_minimum_required(VERSION 2.8)

project(vtkInria)

# -------------------------------------------------------------------------------------
# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings. Copied from ITK CMakeLists.
# -------------------------------------------------------------------------------------
if (WIN32)
  if (NOT BORLAND)
    if (NOT CYGWIN)
      if (NOT MINGW)
          add_definitions(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
      endif()
    endif()
  endif()
endif()

# -------------------------------------------------------------------------------------
# vtkInria version number.
# -------------------------------------------------------------------------------------
set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_BUILD 0)
set(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

# -------------------------------------------------------------------------------------
# Set the path to our FindXXX.cmake files
# -------------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")


# -----------------------------------------------------------------------------
# Include Dart/CTest for testing
# -----------------------------------------------------------------------------
include(Dart)
include(CTest)

# -----------------------------------------------------------------------------
# Add in the option for building examples, default to ON
# -----------------------------------------------------------------------------
option(VTKINRIA_BUILD_EXAMPLES "Build vtkInria examples." OFF)

# -----------------------------------------------------------------------------
# Add in the option for precompiled headers, default to ON
# -----------------------------------------------------------------------------
if (MSVC)
  option(VTKINRIA_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
  # include the cmake macro to add precompiled headers.
  include(AddPch)
endif()


# -----------------------------------------------------------------------------
# We need vtk, no matter what
# -----------------------------------------------------------------------------
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# -----------------------------------------------------------------------------
# SHARED LIBRARY
# -----------------------------------------------------------------------------
# For now we always compile all shared
set(${PROJECT_NAME}_BUILD_SHARED_LIBS 1)

# -----------------------------------------------------------------------------
# ITK
# -----------------------------------------------------------------------------
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
  
# -----------------------------------------------------------------------------
# The hardware shading extension by T. Peeters (BMIA - TUe)
# -----------------------------------------------------------------------------
if ( NOT VTK_USE_GLSL_SHADERS )
    message(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON to use the HWShading library. This option is only available with VTK>=5.1.")
endif()


# -----------------------------------------------------------------------------
# The SPHERICAL HARMONICS extension by Athena - INRIA Sophia Antipolis to use SphericalHarmonicVisualization of Maxime Descoteaux
# -----------------------------------------------------------------------------

if (APPLE OR WIN32)
    find_package( Boost REQUIRED)
endif()

find_path(${PROJECT_NAME}_DATA_DIR NAMES SHTest.vtk DOC "vtkInria Data directory")

# Create the list of include directories needed for vtkInria header files.
include(${${PROJECT_NAME}_SOURCE_DIR}/IncludeDirectories.cmake)

include_directories(
${${PROJECT_NAME}_INCLUDE_DIRS_BUILD_TREE}
${${PROJECT_NAME}_INCLUDE_DIRS_SOURCE_TREE}
)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${${PROJECT_NAME}_REQUIRED_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${${PROJECT_NAME}_REQUIRED_CXX_FLAGS}")
set(${PROJECT_NAME}_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)
add_subdirectory(HWShading)
add_subdirectory(itkAddOn)

if (VTKINRIA_BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

#-----------------------------------------------------------------------------
# doxygen (and other?) documentation

find_package(Doxygen)
if (DOXYGEN)
  option( LINK_EXTERNAL_DOC "Should the documentation be linked with external sources such as ITK?" NO )
  set(MY_LINK_EXTERNAL_DOC ${LINK_EXTERNAL_DOC})
  set(MY_DOXYGEN_BUILD_DIR ${PROJECT_BINARY_DIR}/Doxygen)

  configure_file(
    ${PROJECT_SOURCE_DIR}/Doxygen/doxygen.config.in
    ${MY_DOXYGEN_BUILD_DIR}/doxygen.config
  )

  if (MY_LINK_EXTERNAL_DOC)
    exec_program(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/vtkNightlyDoc.tag.tar.gz
    )
    exec_program(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/KWWidgetsNightlyDoc.tag.tar.gz
    )
    exec_program(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/InsightDoxygen.tag.tar.gz
    )
  else()
    exec_program(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/vtkNightlyDoc.tag
    )
    exec_program(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/KWWidgetsNightlyDoc.tag
    )
    exec_program(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/InsightDoxygen.tag
    )
  endif()

  add_custom_target(${PROJECT_NAME}-doc
    ${DOXYGEN}
    ${MY_DOXYGEN_BUILD_DIR}/doxygen.config
  )
endif()

