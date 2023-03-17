##############################################################################
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

set(CURRENT_SRC_DIR ${CMAKE_SOURCE_DIR}/packaging/unix)
set(CURRENT_BIN_DIR ${CMAKE_BINARY_DIR}/packaging/unix)

# Install a launcher scripts for the application with right environment variable

## For developpers
# Needed on macOS to find plugins. On linux you can also directly use ./medInria-build/bin/medInria
ExternalProject_Get_Property(medInria binary_dir)
set(LOCATE "") 
set(MEDINRIA_DIR ${CMAKE_BINARY_DIR})
if (APPLE) 
 set(MEDINRIA_BIN ${binary_dir}/bin/medInria.app/Contents/MacOS/medInria)
else() 
 set(MEDINRIA_BIN ${binary_dir}/bin/medInria)
endif()
set(MEDINRIA_PLUGINS_DIRS "${binary_dir}/bin/plugins")
set(MEDINRIA_PLUGINS_LEGACY_DIRS "${binary_dir}/bin/plugins_legacy")
configure_file(${CURRENT_SRC_DIR}/medInria.sh.in medInria.sh @ONLY)

## For end users / packaging
file(READ "${CURRENT_SRC_DIR}/locate_bin.sh" LOCATE)
set(MEDINRIA_DIR "$(locate)")
if (APPLE)
  set(MEDINRIA_BIN "\${MEDINRIA_DIR}/bin/medInria.app/Contents/MacOS/medInria")
else()
  set(MEDINRIA_BIN "\${MEDINRIA_DIR}/bin/medInria")
endif()
configure_file(${CURRENT_SRC_DIR}/medInria.sh.in ${CURRENT_BIN_DIR}/medInria_launcher.sh @ONLY)
install(PROGRAMS ${CURRENT_BIN_DIR}/medInria_launcher.sh DESTINATION bin)
