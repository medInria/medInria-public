cmake_minimum_required(VERSION 2.8)

if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
SET(vtkInria_VERSION_MAJOR 1)
SET(vtkInria_VERSION_MINOR 1)
SET(vtkInria_VERSION_BUILD 0)
SET(vtkInria_VERSION
  "${vtkInria_VERSION_MAJOR}.${vtkInria_VERSION_MINOR}.${vtkInria_VERSION_BUILD}")

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
OPTION(vtkInria_BUILD_EXAMPLES "Build vtkInria examples." OFF)

# -----------------------------------------------------------------------------
# Add in the option for precompiled headers, default to ON
# -----------------------------------------------------------------------------
if(MSVC)
  option(vtkInria_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
  # include the cmake macro to add precompiled headers.
  include(AddPch)
endif(MSVC)


# -----------------------------------------------------------------------------
# We need vtk, no matter what
# -----------------------------------------------------------------------------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# VTK 5.0 really need some help. The evil VTK_WRAP_TCL3_INIT_DIR hack was
# fixed on the 5.0 branch but people have downloaded earlier 5.0 as well.
# Furthermore, old 5.0 can not be used once it has been installed, since
# vtkWrapperInit.data.in is not installed properly: report that sad fact.

INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")

IF("${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" EQUAL "5.0")
  IF(VTK_INSTALL_PREFIX)
    IF(NOT EXISTS "${VTK_CMAKE_DIR}/vtkWrapperInit.data.in")
      MESSAGE("Sorry, you are using a VTK 5.0 that can not be used properly once it has been installed. You can either download a more recent VTK 5.0 snapshot from the CVS repository, or simply point your project to your VTK build directory instead of your VTK install directory.")
    ENDIF(NOT EXISTS "${VTK_CMAKE_DIR}/vtkWrapperInit.data.in")
  ELSE(VTK_INSTALL_PREFIX)
    SET(VTK_WRAP_TCL3_INIT_DIR "${VTK_SOURCE_DIR}/Wrapping")
    SET(VTK_WRAP_PYTHON3_INIT_DIR "${VTK_SOURCE_DIR}/Wrapping")
  ENDIF(VTK_INSTALL_PREFIX)
ENDIF("${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" EQUAL "5.0")

# with vtk > 5.7 (true for 5.8.0) VTK does not include all you need when having TCL bindings
# does not compile on ubuntu in that case (fedora is fine...)
if(VTK_TCL_INCLUDE_DIR)
    include_directories("${VTK_TCL_INCLUDE_DIR}" "${VTK_TK_INCLUDE_DIR}")
endif(VTK_TCL_INCLUDE_DIR)

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
OPTION (vtkInria_USE_SPHERICALHARMONICS "If ON, compile the SPHERICAL HARMONICS extension of vtkInria." OFF)

IF( vtkInria_USE_SPHERICALHARMONICS )

  IF(APPLE OR WIN32)
    find_package( Boost REQUIRED)
  ENDIF(APPLE OR WIN32)

  find_path(vtkInria_DATA_DIR NAMES SHTest.vtk DOC "vtkInria Data directory")

ENDIF( vtkInria_USE_SPHERICALHARMONICS )

# Create the list of include directories needed for vtkInria header files.
INCLUDE(${vtkInria_SOURCE_DIR}/IncludeDirectories.cmake)

INCLUDE_DIRECTORIES(
${vtkInria_INCLUDE_DIRS_BUILD_TREE}
${vtkInria_INCLUDE_DIRS_SOURCE_TREE}
)

SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${vtkInria_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${vtkInria_REQUIRED_CXX_FLAGS}")
SET(vtkInria_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)
add_subdirectory(HWShading)
add_subdirectory(vtkItk)
add_subdirectory(itkAddOn)

IF(vtkInria_BUILD_EXAMPLES)
  add_subdirectory(Examples)
ENDIF(vtkInria_BUILD_EXAMPLES)

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

