# Generate the MIPSConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# vtkINRIA3D.

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
SET(vtkINRIA3D_CONFIG_INSTALL_ONLY)

# The "use" file.
SET(vtkINRIA3D_USE_FILE ${vtkINRIA3D_BINARY_DIR}/UsevtkINRIA3D.cmake)

# The build settings file.
SET(vtkINRIA3D_BUILD_SETTINGS_FILE ${vtkINRIA3D_BINARY_DIR}/vtkINRIA3DBuildSettings.cmake)

# Library directory.
SET(vtkINRIA3D_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Runtime library directory.
SET(vtkINRIA3D_RUNTIME_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Determine the include directories needed.
SET(vtkINRIA3D_INCLUDE_DIRS_CONFIG
  ${vtkINRIA3D_INCLUDE_DIRS_BUILD_TREE}
  ${vtkINRIA3D_INCLUDE_DIRS_SOURCE_TREE}
  ${vtkINRIA3D_INCLUDE_DIRS_SYSTEM}
)


#-----------------------------------------------------------------------------
# Configure vtkINRIA3DConfig.cmake for the build tree.
CONFIGURE_FILE(${vtkINRIA3D_SOURCE_DIR}/vtkINRIA3DConfig.cmake.in
               ${vtkINRIA3D_BINARY_DIR}/vtkINRIA3DConfig.cmake @ONLY IMMEDIATE)


#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(vtkINRIA3D_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/UsevtkINRIA3D.cmake)

# The build settings file.
SET(vtkINRIA3D_BUILD_SETTINGS_FILE ${CMAKE_INSTALL_PREFIX}/lib/vtkINRIA3DBuildSettings.cmake)

# Include directories.
SET(vtkINRIA3D_INCLUDE_DIRS_CONFIG
  ${CMAKE_INSTALL_PREFIX}/include
  ${vtkINRIA3D_INCLUDE_DIRS_SYSTEM}
)

# Link directories.
IF(CYGWIN AND BUILD_SHARED_LIBS)
  # In Cygwin programs directly link to the .dll files.
  SET(vtkINRIA3D_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/bin)
ELSE(CYGWIN AND BUILD_SHARED_LIBS)
  SET(vtkINRIA3D_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(CYGWIN AND BUILD_SHARED_LIBS)

# Runtime directories.
IF(WIN32)
  SET(vtkINRIA3D_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/bin)
ELSE(WIN32)
  SET(vtkINRIA3D_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(WIN32)


#-----------------------------------------------------------------------------
# Configure vtkINRIA3DConfig.cmake for the install tree.
CONFIGURE_FILE(${vtkINRIA3D_SOURCE_DIR}/vtkINRIA3DConfig.cmake.in
               ${vtkINRIA3D_BINARY_DIR}/Utilities/vtkINRIA3DConfig.cmake @ONLY IMMEDIATE)


INSTALL_FILES(/lib/vtkINRIA3D FILES ${vtkINRIA3D_BINARY_DIR}/Utilities/vtkINRIA3DConfig.cmake)
