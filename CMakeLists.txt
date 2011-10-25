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
SET(vtkINRIA3D_VERSION_MINOR 0)
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
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")


# -----------------------------------------------------------------------------
# Include Dart/CTest for testing
# -----------------------------------------------------------------------------
include(Dart)
include(CTest)


# -----------------------------------------------------------------------------
# Add in the option for building examples, default to ON
# -----------------------------------------------------------------------------
OPTION(vtkINRIA3D_BUILD_EXAMPLES "Build vtkINRIA3D examples." ON)

# -----------------------------------------------------------------------------
# Add in the option for precompiled headers, default to ON
# -----------------------------------------------------------------------------
if(MSVC)
  option(vtkINRIA3D_USE_PRECOMPILED_HEADERS     "Use precompiled headers"        true)
endif(MSVC)

# include the cmake macro to add precompiled headers.
include(AddPch)

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
OPTION(vtkINRIA3D_USE_ITK "If ON, Compile ITK extensions, like itkAddOn" OFF)
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
# wxWidgets
# -----------------------------------------------------------------------------
OPTION(vtkINRIA3D_USE_WXWINDOWS "If ON, Build the wxVtk extension and the wxSamples (requires ITK too)" OFF)
IF(vtkINRIA3D_USE_WXWINDOWS)

  IF(WIN32)
    SET(wxWidgets_USE_LIBS adv aui xrc xml html core base)
  ELSE(WIN32)
    SET(wxWidgets_USE_LIBS adv aui xrc xml html gl core base)
  ENDIF(WIN32)

  FIND_PACKAGE(wxWidgets2 REQUIRED) # use the version provided by vtkINRIA3D -> to find the aui
                                    # library (advanced user interface)
  INCLUDE(${wxWidgets_USE_FILE})

  # We make the distinction between shared VTK and wxWidgets. If wxWidgets is build
  # shared, all wx-derived addons here will be shared as well.
  IF( WIN32 )
    OPTION(wxWidgets_SHARED "Is wxWidgets built with shared libraries?" OFF)
  ELSE( WIN32 )
    OPTION(wxWidgets_SHARED "Is wxWidgets built with shared libraries?" ON)
  ENDIF( WIN32 )

  IF( wxWidgets_SHARED )
    SET( wxWidgets_BUILD_SHARED_LIBS "ON" )
    SET( wxWidgets_LIBS_STYLE "SHARED")
  ELSE ( wxWidgets_SHARED )
    SET( wxWidgets_BUILD_SHARED_LIBS "OFF")
    SET( wxWidgets_LIBS_STYLE "STATIC")
  ENDIF( wxWidgets_SHARED )

  IF(UNIX AND NOT APPLE) # our UNIX version depends on GTK-2, so find GTK-2 as well
    FIND_PACKAGE(GTK-2 REQUIRED)
    LINK_DIRECTORIES(
    ${GTK_LINK_DIRS}
    )

    SET(wxWidgets_LIBRARIES
    ${wxWidgets_LIBRARIES}
    ${GTK_LINK_LIBS}
    )
  ENDIF(UNIX AND NOT APPLE)

ENDIF(vtkINRIA3D_USE_WXWINDOWS)


# -----------------------------------------------------------------------------
# KWWidgets
# -----------------------------------------------------------------------------
OPTION(vtkINRIA3D_USE_KWWIDGETS "If ON, Build the vtkKW extension and KwVizz" OFF)
IF(vtkINRIA3D_USE_KWWIDGETS)
   FIND_PACKAGE(KWWidgets REQUIRED)
   INCLUDE(${KWWidgets_USE_FILE})
   OPTION ( vtkINRIA3D_USE_KWWIDGETS_OLD_API "For KWWidgets version anterior to version 1.788, please set this flag to ON" OFF )
ENDIF(vtkINRIA3D_USE_KWWIDGETS)



IF(APPLE)
  SET(CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS} -bind_at_load" ) # prevent linker warnings
  IF(NOT vtkINRIA3D_BUILD_SHARED_LIBS)
    SET(vtkINRIA3D_REQUIRED_C_FLAGS   "${vtkINRIA3D_REQUIRED_C_FLAGS}   -mlong-branch")
    SET(vtkINRIA3D_REQUIRED_CXX_FLAGS "${vtkINRIA3D_REQUIRED_CXX_FLAGS} -mlong-branch")
  ENDIF(NOT vtkINRIA3D_BUILD_SHARED_LIBS)
ENDIF(APPLE)


# -----------------------------------------------------------------------------
# The hardware shading extension by T. Peeters (BMIA - TUe)
# -----------------------------------------------------------------------------
OPTION (vtkINRIA3D_USE_HWSHADING "If ON, compile the hardware shading (HWShading) extension of vtkINRIA3D. Requires VTK>=5.1." OFF)

IF( vtkINRIA3D_USE_HWSHADING )
  IF( NOT VTK_USE_GLSL_SHADERS )
    MESSAGE(FATAL_ERROR " VTK must be compiled with VTK_USE_GLSL_SHADERS at ON to use the HWShading library. This option is only available with VTK>=5.1.")
  ENDIF( NOT VTK_USE_GLSL_SHADERS )
ENDIF( vtkINRIA3D_USE_HWSHADING )


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





# Create the list of include directories needed for vtkINRIA3D header files.
INCLUDE(${vtkINRIA3D_SOURCE_DIR}/vtkINRIA3DIncludeDirectories.cmake)


INCLUDE_DIRECTORIES(
${vtkINRIA3D_INCLUDE_DIRS_BUILD_TREE}
${vtkINRIA3D_INCLUDE_DIRS_SOURCE_TREE}
)

SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}    ${vtkINRIA3D_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${vtkINRIA3D_REQUIRED_CXX_FLAGS}")


# -----------------------------------------------------------------------------
# Header file used to pass CMake settings to the source code
# -----------------------------------------------------------------------------
CONFIGURE_FILE(${vtkINRIA3D_SOURCE_DIR}/vtkINRIA3DConfigure.h.in
               ${vtkINRIA3D_BINARY_DIR}/vtkINRIA3DConfigure.h)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${${PROJECT_NAME}_BINARY_DIR}/vtkINRIA3DConfigure.h
      DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)


# -----------------------------------------------------------------------------
# Help other project use vtkINRIA3D
# -----------------------------------------------------------------------------
# Save the compiler settings so another project can import them.
SET(vtkINRIA3D_BUILD_SETTINGS_FILE ${vtkINRIA3D_BINARY_DIR}/vtkINRIA3DBuildSettings.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${vtkINRIA3D_BUILD_SETTINGS_FILE})
if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${vtkINRIA3D_BUILD_SETTINGS_FILE}
      DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)


CONFIGURE_FILE(${vtkINRIA3D_SOURCE_DIR}/UsevtkINRIA3D.cmake.in
               ${vtkINRIA3D_BINARY_DIR}/UsevtkINRIA3D.cmake COPYONLY IMMEDIATE)

# Create the vtkINRIA3DConfig.cmake file containing the vtkINRIA3D configuration.
INCLUDE (${vtkINRIA3D_SOURCE_DIR}/vtkGenerateINRIA3DConfig.cmake)

if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  install(FILES ${${PROJECT_NAME}_BINARY_DIR}/UsevtkINRIA3D.cmake
      DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR})
endif(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)

SET(vtkINRIA3D_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "")


add_subdirectory(vtkDataManagement)
add_subdirectory(vtkImageView)
add_subdirectory(vtkRenderingAddOn)
add_subdirectory(vtkWidgetsAddOn)
add_subdirectory(vtkVisuManagement)

IF( vtkINRIA3D_USE_HWSHADING )
  add_subdirectory(HWShading)
ENDIF( vtkINRIA3D_USE_HWSHADING )

if(vtkINRIA3D_USE_WXWINDOWS)
  add_subdirectory(wxAddOn)
  add_subdirectory(wxVtk)
endif(vtkINRIA3D_USE_WXWINDOWS)

if(vtkINRIA3D_USE_ITK)
  add_subdirectory(vtkItk)
  add_subdirectory(itkAddOn)
endif(vtkINRIA3D_USE_ITK)

if(vtkINRIA3D_USE_KWWIDGETS)
  add_subdirectory( KWAddOn )
endif(vtkINRIA3D_USE_KWWIDGETS)

IF(vtkINRIA3D_BUILD_EXAMPLES)
  add_subdirectory(Examples)
  if(vtkINRIA3D_USE_WXWINDOWS)
    add_subdirectory(wxSamples)
  endif(vtkINRIA3D_USE_WXWINDOWS)
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




# Packaging

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vtkINRIA3D")
SET(CPACK_PACKAGE_VENDOR "INRIA Sophia Antipolis, Asclepios Research Team.")
SET(CPACK_PACKAGE_VERSION_MAJOR "${vtkINRIA3D_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${vtkINRIA3D_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${vtkINRIA3D_VERSION_BUILD}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "vtkINRIA3D ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
"vtkINRIA3D-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

IF(WIN32)
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  SET(CPACK_NSIS_HELP_LINK "https://gforge.inria.fr/projects/vtkinria3d/")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www-sop.inria.fr/asclepios/")
  SET(CPACK_NSIS_CONTACT "vtkinria3d-users@lists.gforge.inria.fr")
ENDIF(WIN32)

INCLUDE(CPack)

