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

# Set common CPACK variables

set(CPACK_PACKAGE_NAME medInria CACHE STRING "Name of the package for medInria superproject")
mark_as_advanced(CPACK_PACKAGE_NAME)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_NAME} - medical image visualisation and processing tool")

set(CPACK_PACKAGE_DESCRIPTION 
    "${CPACK_PACKAGE_NAME} is a medical image visualization and processing tool developed at Inria by the teams Asclepios, Athena, Parietal and Visages.")

set(CPACK_PACKAGE_VENDOR  http://med.inria.fr/)
set(CPACK_PACKAGE_CONTACT "medInria Team <medinria-userfeedback@inria.fr>")

set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/packaging/Description.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/packaging/Readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/packaging/Welcome.txt")

set(CPACK_BINARY_STGZ OFF)
set(CPACK_BINARY_TBZ2 OFF)
set(CPACK_BINARY_TGZ ON)
set(CPACK_BINARY_TZ OFF)

set(CPACK_SOURCE_TBZ2 OFF)
set(CPACK_SOURCE_TGZ OFF)
set(CPACK_SOURCE_TZ OFF)
set(CPACK_SOURCE_ZIP OFF)

# Set cpack variables specific to the plateform

if (WIN32)
    set(WIN_PACK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows)
    include(${WIN_PACK_DIR}/WindowsLaunchers.cmake)
#    include(${WIN_PACK_DIR}/WindowsPackaging.cmake)
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
