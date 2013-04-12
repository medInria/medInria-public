### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Sun Nov 28 17:25:43 2010 (+0100)
## Version: $Id$
## Last-Updated: Mon Feb 25 18:31:37 2013 (+0100)
##           By: Julien Wintz
##     Update #: 37
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

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
IF(APPLE)
  include(FixDCMTKMacInstall)
  FixDCMTKMacInstall()
ENDIF(APPLE)

set(MEDINRIA_REQUIRES_VTK TRUE) # for itkMultiThreadedImageIO in vtkInria3d

set (${PROJECT_NAME}_HEADERS 
    itkDCMTKImageIO.h)

set(${PROJECT_NAME}_SOURCES
  itkDCMTKImageIO.cxx
)

set(LIBRARY_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_library(ITKDCMTKIO "SHARED"
  ${ITKDCMTKIO_SOURCES}
)

target_link_libraries(ITKDCMTKIO
  ${DCMTK_LIBRARIES}
  itkIOAddOn
  ${ITK_LIBRARIES}
)

if(APPLE)
  include(FixDCMTKMacLink)
  FixDCMTKMacLink(ITKDCMTKIO)
endif(APPLE)

