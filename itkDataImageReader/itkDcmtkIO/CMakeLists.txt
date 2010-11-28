### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Sun Nov 28 17:25:43 2010 (+0100)
## Version: $Id$
## Last-Updated: Sun Nov 28 17:35:37 2010 (+0100)
##           By: Julien Wintz
##     Update #: 5
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(ITKDCMTKIO)

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

if(ITK_BUILD_SHARED)
    install (TARGETS ITKDCMTKIO  LIBRARY DESTINATION lib)
endif(ITK_BUILD_SHARED)
