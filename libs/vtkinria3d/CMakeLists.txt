cmake_minimum_required(VERSION 2.6)


IF(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
  cmake_policy(SET CMP0005 OLD)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)


PROJECT(vtkINRIA3D)

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
# vtkINRIA3D version number.
# -------------------------------------------------------------------------------------
SET(vtkINRIA3D_VERSION_MAJOR 1)
SET(vtkINRIA3D_VERSION_MINOR 1)
SET(vtkINRIA3D_VERSION_BUILD 0)
SET(vtkINRIA3D_VERSION
  "${vtkINRIA3D_VERSION_MAJOR}.${vtkINRIA3D_VERSION_MINOR}.${vtkINRIA3D_VERSION_BUILD}")


# -------------------------------------------------------------------------------------
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
# -------------------------------------------------------------------------------------
if(WIN32)
  set(${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY lib)
  set(${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY bin)
  set(${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY bin)
else(WIN32)
  set(${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY lib)
  set(${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY bin)
  set(${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY lib)
endif(WIN32)

set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_LIBRARY_OUTPUT_DIRECTORY})
set(ARCHIVE_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_ARCHIVE_OUTPUT_DIRECTORY})
set(RUNTIME_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${${PROJECT_NAME}_RUNTIME_OUTPUT_DIRECTORY})
set(CXX_TEST_PATH          ${EXECUTABLE_OUTPUT_PATH})


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
OPTION(vtkINRIA3D_BUILD_EXAMPLES "Build vtkINRIA3D examples." OFF)

# -----------------------------------------------------------------------------
# Add in the option for precompiled headers, default to ON
# -----------------------------------------------------------------------------
if(MSVC)
  option(vtkINRIA3D_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
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
# We must remain consistent with the VTK build settings: VTK is shared => addons to VTK
# will be shared. VTK is static => things will be static.
IF( VTK_BUILD_SHARED_LIBS )
  SET(vtkINRIA3D_LIBS_STYLE "SHARED")
  SET (${PROJECT_NAME}_BUILD_SHARED_LIBS 1)
ELSE( VTK_BUILD_SHARED_LIBS )
  SET(vtkINRIA3D_LIBS_STYLE "STATIC")
  SET (${PROJECT_NAME}_BUILD_SHARED_LIBS 0)
ENDIF( VTK_BUILD_SHARED_LIBS )


# -----------------------------------------------------------------------------
# ITK
# -----------------------------------------------------------------------------
OPTION(vtkINRIA3D_USE_ITK "If ON, Compile ITK extensions, like itkAddOn" ON)
IF(vtkINRIA3D_USE_ITK)
  FIND_PACKAGE(ITK REQUIRED)
  INCLUDE(${ITK_USE_FILE})

  IF( ITK_BUILD_SHARED )
    SET( ITK_LIBS_STYLE "SHARED" )
  ELSE( ITK_BUILD_SHARED )
    SET( ITK_LIBS_STYLE "STATIC" )
  ENDIF( ITK_BUILD_SHARED )
ENDIF(vtkINRIA3D_USE_ITK)


# -----------------------------------------------------------------------------
# The hardware shading extension by T. Peeters (BMIA - TUe)
# -----------------------------------------------------------------------------
OPTION (vtkINRIA3D_USE_HWSHADING "If ON, compile the hardware shading (HWShading) extension of vtkINRIA3D. Requires VTK>=5.1." ON)

IF( vtkINRIA3D_USE_HWSHADING )
  IF( NOT VTK_USE_GLSL_SHADERS )
    MESSAGE(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON to use the HWShading library. This option is only available with VTK>=5.1.")
  ENDIF( NOT VTK_USE_GLSL_SHADERS )
ENDIF( vtkINRIA3D_USE_HWSHADING )

# -----------------------------------------------------------------------------
# The SPHERICAL HARMONICS extension by Athena - INRIA Sophia Antipolis to use SphericalHarmonicVisualization of Maxime Descoteaux
# -----------------------------------------------------------------------------
OPTION (vtkINRIA3D_USE_SPHERICALHARMONICS "If ON, compile the SPHERICAL HARMONICS extension of vtkINRIA3D." OFF)

IF( vtkINRIA3D_USE_SPHERICALHARMONICS )
  IF( NOT vtkINRIA3D_USE_ITK )
    MESSAGE(FATAL_ERROR " ITK is needed ON to use the Spherical Harmonics Functions Visualization")
  ENDIF( NOT vtkINRIA3D_USE_ITK )

  IF(APPLE OR WIN32)
    find_package( Boost REQUIRED)
  ENDIF(APPLE OR WIN32)

  find_path(vtkINRIA3D_DATA_DIR NAMES SHTest.vtk DOC "vtkInria3d Data directory")

ENDIF( vtkINRIA3D_USE_SPHERICALHARMONICS )

# Create the list of include directories needed for vtkINRIA3D header files.
INCLUDE(${vtkINRIA3D_SOURCE_DIR}/vtkINRIA3DIncludeDirectories.cmake)

INCLUDE_DIRECTORIES(
${vtkINRIA3D_INCLUDE_DIRS_BUILD_TREE}
${vtkINRIA3D_INCLUDE_DIRS_SOURCE_TREE}
)

SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${vtkINRIA3D_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${vtkINRIA3D_REQUIRED_CXX_FLAGS}")
SET(vtkINRIA3D_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)

IF( vtkINRIA3D_USE_HWSHADING )
  add_subdirectory(HWShading)
ENDIF( vtkINRIA3D_USE_HWSHADING )

IF(vtkINRIA3D_USE_ITK)
  add_subdirectory(vtkItk)
  add_subdirectory(itkAddOn)
ENDIF(vtkINRIA3D_USE_ITK)

IF(vtkINRIA3D_BUILD_EXAMPLES)
  add_subdirectory(Examples)
ENDIF(vtkINRIA3D_BUILD_EXAMPLES)

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
