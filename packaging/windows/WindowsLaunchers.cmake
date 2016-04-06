##############################################################################
#
# medInria
#
# Copyright (c) INRIA 2013-2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

set(CURRENT_SRC_DIR ${CMAKE_SOURCE_DIR}/packaging/windows)

foreach (dir ${PRIVATE_PLUGINS_DIRS})
    set(DEV_PLUGINS_DIRS "${DEV_PLUGINS_DIRS}:${dir}/plugins/%1")
endforeach() 

set(WIN_TYPE x86)
if (CMAKE_GENERATOR MATCHES "Win64")
    set(WIN_TYPE x64)
endif()

set(VS_TYPE "VS90COMNTOOLS")
if (CMAKE_GENERATOR MATCHES "Visual Studio 10")
    set(VS_TYPE "VS100COMNTOOLS")
endif()

configure_file(${CURRENT_SRC_DIR}/medInria.bat.in     medInria.bat     @ONLY)
configure_file(${CURRENT_SRC_DIR}/medInria-dev.bat.in medInria-dev.bat @ONLY)
