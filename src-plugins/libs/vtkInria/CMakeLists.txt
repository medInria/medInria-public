cmake_minimum_required(VERSION 2.8)

PROJECT(vtkInria)

# -------------------------------------------------------------------------------------
# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings. Copied from ITK CMakeLists.
# -------------------------------------------------------------------------------------
IF(WIN32)
  IF(NOT BORLAND)
    IF(NOT CYGWIN)
      IF(NOT MINGW)
          ADD_DEFINITIONS(
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
      ENDIF(NOT MINGW)
    ENDIF(NOT CYGWIN)
  ENDIF(NOT BORLAND)
ENDIF(WIN32)

# -------------------------------------------------------------------------------------
# vtkInria version number.
# -------------------------------------------------------------------------------------
SET(${PROJECT_NAME}_VERSION_MAJOR 1)
SET(${PROJECT_NAME}_VERSION_MINOR 1)
SET(${PROJECT_NAME}_VERSION_BUILD 0)
SET(${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

# -------------------------------------------------------------------------------------
# Set the path to our FindXXX.cmake files
# -------------------------------------------------------------------------------------
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")


# -----------------------------------------------------------------------------
# Include Dart/CTest for testing
# -----------------------------------------------------------------------------
include(Dart)
include(CTest)

# -----------------------------------------------------------------------------
# Add in the option for building examples, default to ON
# -----------------------------------------------------------------------------
OPTION(VTKINRIA_BUILD_EXAMPLES "Build vtkInria examples." OFF)

# -----------------------------------------------------------------------------
# Add in the option for precompiled headers, default to ON
# -----------------------------------------------------------------------------
if(MSVC)
  option(VTKINRIA_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
  # include the cmake macro to add precompiled headers.
  include(AddPch)
endif(MSVC)


# -----------------------------------------------------------------------------
# We need vtk, no matter what
# -----------------------------------------------------------------------------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# -----------------------------------------------------------------------------
# SHARED LIBRARY
# -----------------------------------------------------------------------------
# For now we always compile all shared
SET (${PROJECT_NAME}_BUILD_SHARED_LIBS 1)

# -----------------------------------------------------------------------------
# ITK
# -----------------------------------------------------------------------------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})
  
# -----------------------------------------------------------------------------
# The hardware shading extension by T. Peeters (BMIA - TUe)
# -----------------------------------------------------------------------------
IF( NOT VTK_USE_GLSL_SHADERS )
    MESSAGE(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON to use the HWShading library. This option is only available with VTK>=5.1.")
ENDIF( NOT VTK_USE_GLSL_SHADERS )


# -----------------------------------------------------------------------------
# The SPHERICAL HARMONICS extension by Athena - INRIA Sophia Antipolis to use SphericalHarmonicVisualization of Maxime Descoteaux
# -----------------------------------------------------------------------------
OPTION (VTKINRIA_USE_SPHERICALHARMONICS "If ON, compile the SPHERICAL HARMONICS extension of vtkInria." OFF)

IF( VTKINRIA_USE_SPHERICALHARMONICS )

  IF(APPLE OR WIN32)
    find_package( Boost REQUIRED)
  ENDIF(APPLE OR WIN32)

  find_path(${PROJECT_NAME}_DATA_DIR NAMES SHTest.vtk DOC "vtkInria Data directory")

ENDIF( VTKINRIA_USE_SPHERICALHARMONICS )

# Create the list of include directories needed for vtkInria header files.
INCLUDE(${${PROJECT_NAME}_SOURCE_DIR}/IncludeDirectories.cmake)

INCLUDE_DIRECTORIES(
${${PROJECT_NAME}_INCLUDE_DIRS_BUILD_TREE}
${${PROJECT_NAME}_INCLUDE_DIRS_SOURCE_TREE}
)

SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${${PROJECT_NAME}_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${${PROJECT_NAME}_REQUIRED_CXX_FLAGS}")
SET(${PROJECT_NAME}_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)
add_subdirectory(HWShading)
add_subdirectory(itkAddOn)

IF(VTKINRIA_BUILD_EXAMPLES)
  add_subdirectory(Examples)
ENDIF(VTKINRIA_BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# doxygen (and other?) documentation

FIND_PACKAGE(Doxygen)
IF (DOXYGEN)
  OPTION( LINK_EXTERNAL_DOC "Should the documentation be linked with external sources such as ITK?" NO )
  SET(MY_LINK_EXTERNAL_DOC ${LINK_EXTERNAL_DOC})
  SET(MY_DOXYGEN_BUILD_DIR ${PROJECT_BINARY_DIR}/Doxygen)

  CONFIGURE_FILE (
    ${PROJECT_SOURCE_DIR}/Doxygen/doxygen.config.in
    ${MY_DOXYGEN_BUILD_DIR}/doxygen.config
  )

  IF (MY_LINK_EXTERNAL_DOC)
    EXEC_PROGRAM(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/vtkNightlyDoc.tag.tar.gz
    )
    EXEC_PROGRAM(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/KWWidgetsNightlyDoc.tag.tar.gz
    )
    EXEC_PROGRAM(${CMAKE_COMMAND} ${MY_DOXYGEN_BUILD_DIR}
      ARGS -E tar xvz ${PROJECT_SOURCE_DIR}/Doxygen/InsightDoxygen.tag.tar.gz
    )
  ELSE (MY_LINK_EXTERNAL_DOC)
    EXEC_PROGRAM(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/vtkNightlyDoc.tag
    )
    EXEC_PROGRAM(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/KWWidgetsNightlyDoc.tag
    )
    EXEC_PROGRAM(${CMAKE_COMMAND}
      ARGS -E remove ${MY_DOXYGEN_BUILD_DIR}/InsightDoxygen.tag
    )
  ENDIF (MY_LINK_EXTERNAL_DOC)

  ADD_CUSTOM_TARGET(${PROJECT_NAME}-doc
    ${DOXYGEN}
    ${MY_DOXYGEN_BUILD_DIR}/doxygen.config
  )
ENDIF(DOXYGEN)

