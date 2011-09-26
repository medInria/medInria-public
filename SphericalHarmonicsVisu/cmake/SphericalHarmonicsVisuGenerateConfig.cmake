# Generate the MIPSConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# SphericalHarmonicsVisu.

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
SET(SphericalHarmonicsVisu_CONFIG_INSTALL_ONLY)

# The "use" file.
SET(SphericalHarmonicsVisu_USE_FILE ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/UseSphericalHarmonicsVisu.cmake)

# The build settings file.
SET(SphericalHarmonicsVisu_BUILD_SETTINGS_FILE ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/SphericalHarmonicsVisuBuildSettings.cmake)

# Library directory.
SET(SphericalHarmonicsVisu_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Runtime library directory.
SET(SphericalHarmonicsVisu_RUNTIME_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Determine the include directories needed.
SET(SphericalHarmonicsVisu_INCLUDE_DIRS_CONFIG
  ${SphericalHarmonicsVisu_INCLUDE_DIRS_BUILD_TREE}
  ${SphericalHarmonicsVisu_INCLUDE_DIRS_SOURCE_TREE}
  ${SphericalHarmonicsVisu_INCLUDE_DIRS_SYSTEM}
)


#-----------------------------------------------------------------------------
# Configure SphericalHarmonicsVisuConfig.cmake for the build tree.
CONFIGURE_FILE(${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/SphericalHarmonicsVisuConfig.cmake.in
               ${SphericalHarmonicsVisu_BINARY_DIR}/cmake/SphericalHarmonicsVisuConfig.cmake @ONLY IMMEDIATE)


#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(SphericalHarmonicsVisu_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/cmake/UseSphericalHarmonicsVisu.cmake)

# The build settings file.
SET(SphericalHarmonicsVisu_BUILD_SETTINGS_FILE ${CMAKE_INSTALL_PREFIX}/lib/cmake/SphericalHarmonicsVisuBuildSettings.cmake)

# Include directories.
SET(SphericalHarmonicsVisu_INCLUDE_DIRS_CONFIG
  ${CMAKE_INSTALL_PREFIX}/include
  ${SphericalHarmonicsVisu_INCLUDE_DIRS_SYSTEM}
)

# Link directories.
IF(CYGWIN AND BUILD_SHARED_LIBS)
  # In Cygwin programs directly link to the .dll files.
  SET(SphericalHarmonicsVisu_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/bin)
ELSE(CYGWIN AND BUILD_SHARED_LIBS)
  SET(SphericalHarmonicsVisu_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(CYGWIN AND BUILD_SHARED_LIBS)

# Runtime directories.
IF(WIN32)
  SET(SphericalHarmonicsVisu_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/bin)
ELSE(WIN32)
  SET(SphericalHarmonicsVisu_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(WIN32)


#-----------------------------------------------------------------------------
# Configure SphericalHarmonicsVisuConfig.cmake for the install tree.
CONFIGURE_FILE(${SphericalHarmonicsVisu_SOURCE_DIR}/cmake/SphericalHarmonicsVisuConfig.cmake.in
               ${SphericalHarmonicsVisu_BINARY_DIR}/Utilities/SphericalHarmonicsVisuConfig.cmake @ONLY IMMEDIATE)


INSTALL_FILES(/lib/SphericalHarmonicsVisu FILES ${SphericalHarmonicsVisu_BINARY_DIR}/Utilities/SphericalHarmonicsVisuConfig.cmake)
