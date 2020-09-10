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

#   For developpers.

foreach (dir ${PRIVATE_PLUGINS_DIRS})
	set(DEV_PLUGINS_DIRS "${DEV_PLUGINS_DIRS}:${dir}/bin/plugins")
endforeach() 

foreach (dir ${PRIVATE_PLUGINS_LEGACY_DIRS})
	set(DEV_PLUGINS_LEGACY_DIRS "${DEV_PLUGINS_LEGACY_DIRS}:${dir}/bin/plugins_legacy")
endforeach()

foreach (dir ${EXTERNAL_PROJECT_PLUGINS_LEGACY_DIRS})
        set(DEV_PLUGINS_LEGACY_DIRS "${DEV_PLUGINS_LEGACY_DIRS}:${dir}/bin/plugins_legacy")
endforeach()

ExternalProject_Get_Property(medInria binary_dir)

set(LOCATE "")
set(MEDINRIA_DIR ${CMAKE_BINARY_DIR})

if (APPLE)
  set(MEDINRIA_BIN ${binary_dir}/bin/MUSICardio.app/Contents/MacOS/MUSICardio)
else()
  set(MEDINRIA_BIN ${binary_dir}/bin/MUSICardio)
endif()

set(MEDINRIA_PLUGINS_DIRS "${binary_dir}/bin/plugins:${DEV_PLUGINS_DIRS}")
set(MEDINRIA_PLUGINS_LEGACY_DIRS "${binary_dir}/bin/plugins_legacy:${DEV_PLUGINS_LEGACY_DIRS}")

configure_file(${CURRENT_SRC_DIR}/MUSICardio.sh.in MUSICardio.sh @ONLY)

#   For end users.

file(READ "${CURRENT_SRC_DIR}/locate_bin.sh" LOCATE)
set(MEDINRIA_DIR "$(locate)")

if (APPLE)
  set(MEDINRIA_BIN "\${MEDINRIA_DIR}/bin/MUSICardio.app/Contents/MacOS/MUSICardio")
else()
  set(MEDINRIA_BIN "\${MEDINRIA_DIR}/bin/MUSICardio")
endif()

set(MEDINRIA_PLUGINS_DIRS "\${MEDINRIA_DIR}/plugins:\${MEDINRIA_DIR}/bin/plugins:\${MEDINRIA_USER_PLUGINS_DIRS}")
set(MEDINRIA_PLUGINS_LEGACY_DIRS "\${MEDINRIA_DIR}/plugins_legacy:\${MEDINRIA_DIR}/bin/plugins_legacy:\${MEDINRIA_USER_PLUGINS_DIRS_LEGACY}")

configure_file(${CURRENT_SRC_DIR}/MUSICardio.sh.in ${CURRENT_BIN_DIR}/MUSICardio_launcher.sh @ONLY)
install(PROGRAMS ${CURRENT_BIN_DIR}/MUSICardio_launcher.sh
        DESTINATION bin)
