#######################################################################
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
#######################################################################

project(ITKNIFIIO)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(ITKDCMTKIO_SOURCES
  itkDCMTKImageIO.cxx)

set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_library(ITKDCMTKIO ${ITK_LIB_STYLE}
  ${ITKDCMTKIO_SOURCES})

target_link_libraries(ITKDCMTKIO
  ${DCMTK_LIBRARIES}
  itkIOAddOn
  ${ITK_LIBRARIES})

if (ITK_BUILD_SHARED)
    install (TARGETS ITKDCMTKIO  LIBRARY DESTINATION lib)
endif()
