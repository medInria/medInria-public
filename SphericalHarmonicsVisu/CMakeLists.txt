cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

PROJECT(SphericalHarmonicsVisu)
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
# SphericalHarmonicsVisu version number.
# -------------------------------------------------------------------------------------
SET(SphericalHarmonicsVisu_VERSION_MAJOR 1)
SET(SphericalHarmonicsVisu_VERSION_MINOR 0)
SET(SphericalHarmonicsVisu_VERSION_BUILD 0)
SET(SphericalHarmonicsVisu_VERSION 
  "${SphericalHarmonicsVisu_VERSION_MAJOR}.${SphericalHarmonicsVisu_VERSION_MINOR}.${SphericalHarmonicsVisu_VERSION_BUILD}")

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
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

# -----------------------------------------------------------------------------
# Include Dart/CTest for testing
# -----------------------------------------------------------------------------
include(Dart)
include(CTest)

#######################################################################
# External Libraries
#######################################################################
# -----------------------------------------------------------------------------
# ITK
# -----------------------------------------------------------------------------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

IF( ITK_BUILD_SHARED )
  SET( ITK_LIBS_STYLE "SHARED" )
ELSE( ITK_BUILD_SHARED )
  SET( ITK_LIBS_STYLE "STATIC" )
ENDIF( ITK_BUILD_SHARED )


#------------------------------------------------------------
# VTK library
#------------------------------------------------------------

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
    SET (VTK_LIBRARIES vtkIO vtkCommon)
ELSE(VTK_FOUND)
    MESSAGE(FATAL_ERROR "Please set VTK_DIR")
ENDIF(VTK_FOUND)


# -----------------------------------------------------------------------------
# SHARED LIBRARY
# -----------------------------------------------------------------------------
# We must remain consistent with the VTK build settings: VTK is shared => addons to VTK
# will be shared. VTK is static => things will be static.
IF( VTK_BUILD_SHARED_LIBS )
  SET(SphericalHarmonicsVisu_LIBS_STYLE "SHARED")
  SET (${PROJECT_NAME}_BUILD_SHARED_LIBS 1)
ELSE( VTK_BUILD_SHARED_LIBS )
  SET(SphericalHarmonicsVisu_LIBS_STYLE "STATIC")
  SET (${PROJECT_NAME}_BUILD_SHARED_LIBS 0)
ENDIF( VTK_BUILD_SHARED_LIBS )

# -------------------------------------------------------------------------------------
# Setup installation settings
# -------------------------------------------------------------------------------------
set(${PROJECT_NAME}_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})

if(NOT ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR "bin")
endif(NOT ${PROJECT_NAME}_INSTALL_BIN_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR "lib")
endif(NOT ${PROJECT_NAME}_INSTALL_LIB_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_DATA_DIR)
  set(${PROJECT_NAME}_INSTALL_DATA_DIR "share/${PROJECT_NAME}" CACHE INTERNAL "")
endif(NOT ${PROJECT_NAME}_INSTALL_DATA_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
  set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR "include")
endif(NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_MODULE_DIR)
  set(${PROJECT_NAME}_INSTALL_MODULE_DIR "module/${PROJECT_NAME}")
endif(NOT ${PROJECT_NAME}_INSTALL_MODULE_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_PACKAGE_DIR)
  set(${PROJECT_NAME}_INSTALL_PACKAGE_DIR ${VTK_INSTALL_PACKAGE_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_PACKAGE_DIR)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  set(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

if(NOT ${PROJECT_NAME}_INSTALL_NO_RUNTIME)
  set(${PROJECT_NAME}_INSTALL_NO_RUNTIME 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_RUNTIME)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION)
  set(${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION 0)
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DOCUMENTATION)

set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES)
if(${PROJECT_NAME}_BUILD_SHARED_LIBS)
  if(${PROJECT_NAME}_INSTALL_NO_RUNTIME AND ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES 1)
  endif(${PROJECT_NAME}_INSTALL_NO_RUNTIME AND ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
else(${PROJECT_NAME}_BUILD_SHARED_LIBS)
  if(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
    set(${PROJECT_NAME}_INSTALL_NO_LIBRARIES 1)
  endif(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
endif(${PROJECT_NAME}_BUILD_SHARED_LIBS)

# Create the list of include directories needed for SphericalHarmonicsVisu header files.
INCLUDE(${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/SphericalHarmonicsVisuIncludeDirectories.cmake)


INCLUDE_DIRECTORIES(
${SphericalHarmonicsVisu_INCLUDE_DIRS_BUILD_TREE}
${SphericalHarmonicsVisu_INCLUDE_DIRS_SOURCE_TREE}
)


#SET(CMAKE_CXX_FLAGS "-O3 -finline-functions -ffast-math -fexpensive-optimizations ")
#SET(CMAKE_EXE_LINKER_FLAGS "-O3 -finline-functions -ffast-math -fexpensive-optimizations")
SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${SphericalHarmonicsVisu_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${SphericalHarmonicsVisu_REQUIRED_CXX_FLAGS}")

# -----------------------------------------------------------------------------
# Header file used to pass CMake settings to the source code
# -----------------------------------------------------------------------------
CONFIGURE_FILE(${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/SphericalHarmonicsVisuConfigure.h.in
               ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/SphericalHarmonicsVisuConfigure.h)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${${PROJECT_NAME}_BINARY_DIR}/SphericalHarmonicsVisuConfigure.h
      DESTINATION ${SphericalHarmonicsVisu_INSTALL_INCLUDE_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)


# -----------------------------------------------------------------------------
# Help other project use SphericalHarmonicsVisu
# -----------------------------------------------------------------------------
# Save the compiler settings so another project can import them.
SET(SphericalHarmonicsVisu_BUILD_SETTINGS_FILE ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/SphericalHarmonicsVisuBuildSettings.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${SphericalHarmonicsVisu_BUILD_SETTINGS_FILE})
if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${SphericalHarmonicsVisu_BUILD_SETTINGS_FILE}
      DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)


CONFIGURE_FILE(${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/UseSphericalHarmonicsVisu.cmake.in
               ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/UseSphericalHarmonicsVisu.cmake COPYONLY IMMEDIATE)

# Create the SphericalHarmonicsVisuConfig.cmake file containing the SphericalHarmonicsVisu configuration.
INCLUDE (${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/SphericalHarmonicsVisuGenerateConfig.cmake)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${${PROJECT_NAME}_BINARY_DIR}/UseSphericalHarmonicsVisu.cmake
      DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

SET(SphericalHarmonicsVisu_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")


SUBDIRS(src)

# Packaging

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SphericalHarmonicsVisu")
SET(CPACK_PACKAGE_VENDOR "INRIA Sophia Antipolis, Athena Research Team.")
SET(CPACK_PACKAGE_VERSION_MAJOR "${SphericalHarmonicsVisu_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${SphericalHarmonicsVisu_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${SphericalHarmonicsVisu_VERSION_BUILD}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "SphericalHarmonicsVisu ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
"SphericalHarmonicsVisu-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

IF(WIN32)
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  SET(CPACK_NSIS_HELP_LINK "https://gforge.inria.fr/projects/SphericalHarmonicsVisu/")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www-sop.inria.fr/asclepios/")
  SET(CPACK_NSIS_CONTACT "SphericalHarmonicsVisu-users@lists.gforge.inria.fr")
ENDIF(WIN32)

INCLUDE(CPack)


# Help other projects use this one
# 
# INCLUDE(GenerateConfigFile)
# GenerateConfigFile(SphericalFunctionSHVisualization)

