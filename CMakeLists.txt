################################################################################
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

cmake_minimum_required(VERSION 3.3)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

if(NOT DEFINED ${MEDINRIA_SUPERBUILD_VERSION})
    set(MEDINRIA_SUPERBUILD_VERSION 3.2.0)
endif()

SET(CMAKE_CXX_STANDARD 17)

project(MEDINRIA_SUPERBUILD VERSION ${MEDINRIA_SUPERBUILD_VERSION})

add_subdirectory(superbuild)
