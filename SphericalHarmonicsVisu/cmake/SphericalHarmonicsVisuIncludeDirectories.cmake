#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(SphericalHarmonicsVisu_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(SphericalHarmonicsVisu_INCLUDE_DIRS_BUILD_TREE
${SphericalHarmonicsVisu_BINARY_DIR}
)


# These directories are always needed.
SET(SphericalHarmonicsVisu_INCLUDE_DIRS_SOURCE_TREE
${SphericalHarmonicsVisu_SOURCE_DIR}/include/
${SphericalHarmonicsVisu_SOURCE_DIR}/src/
)
