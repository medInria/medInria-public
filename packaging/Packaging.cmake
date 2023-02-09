##############################################################################
#
# medInria
#
# Copyright (c) INRIA 2018. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

# Set common CPACK variables

set(CPACK_PACKAGE_NAME ${APPLICATION_NAME} CACHE STRING "Name of the package for the superproject" FORCE)
mark_as_advanced(CPACK_PACKAGE_NAME)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_NAME} - medical image visualisation and processing tool")
set(CPACK_PACKAGE_DESCRIPTION         "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR              ${PACKAGE_VENDOR})
set(CPACK_PACKAGE_CONTACT             "${PROJECT_CONTACT}")
set(CPACK_PACKAGE_VERSION_MAJOR       ${${PROJECT_NAME}_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR       ${${PROJECT_NAME}_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH       ${${PROJECT_NAME}_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION             ${${PROJECT_NAME}_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/${DESCRIPTION_PACKAGING}")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/${LICENSE_FILE_PATH}")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_SOURCE_DIR}/${README_FILE}")
set(CPACK_RESOURCE_FILE_WELCOME       "${CMAKE_SOURCE_DIR}/${WELCOME_FILE}")

set(CPACK_BINARY_STGZ OFF)
set(CPACK_BINARY_TBZ2 OFF)
set(CPACK_BINARY_TGZ ON)
set(CPACK_BINARY_TZ OFF)

option(CPACK_SOURCE_RPM  "Enable to build RPM source packages"  OFF)
option(CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
option(CPACK_SOURCE_TGZ  "Enable to build TGZ source packages"  OFF)
option(CPACK_SOURCE_TXZ  "Enable to build TXZ source packages"  OFF)
option(CPACK_SOURCE_TZ   "Enable to build TZ source packages"   OFF)
option(CPACK_SOURCE_ZIP  "Enable to build ZIP source packages"  OFF)

# Set cpack variables specific to the plateform

if (WIN32)
    set(WIN_PACK_DIR ${CMAKE_SOURCE_DIR}/packaging/windows)
    include(${WIN_PACK_DIR}/WindowsLaunchers.cmake)
    include(${WIN_PACK_DIR}/WindowsPackaging.cmake)
endif()

if (APPLE)
    include(${CMAKE_SOURCE_DIR}/packaging/apple/ApplePackaging.cmake)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    include(${CMAKE_SOURCE_DIR}/packaging/linux/LinuxPackaging.cmake)
endif()

if (UNIX)
    include(${CMAKE_SOURCE_DIR}/packaging/unix/Launchers.cmake)
endif()

# Include cpack modules

include (InstallRequiredSystemLibraries)
include (CPack)
