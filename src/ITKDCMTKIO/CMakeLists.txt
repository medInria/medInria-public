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

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(ITK_LIB_STYLE "STATIC")
if(ITK_BUILD_SHARED)
  set(ITK_LIB_STYLE "SHARED")
endif (ITK_BUILD_SHARED)

find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

find_package(vtkINRIA3D REQUIRED) # for itkMultiThreadedImageIO
include(${vtkINRIA3D_USE_FILE})

set(ITKDCMTKIO_SOURCES
  itkDCMTKImageIO.cxx
)

set(LIBRARY_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medinria-plugins_LIBRARY_OUTPUT_DIRECTORY})
set(ARCHIVE_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medinria-plugins_ARCHIVE_OUTPUT_DIRECTORY})
set(RUNTIME_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medinria-plugins_RUNTIME_OUTPUT_DIRECTORY})
set(EXECUTABLE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${medinria-plugins_RUNTIME_OUTPUT_DIRECTORY})


add_library(ITKDCMTKIO ${ITK_LIB_STYLE}
  ${ITKDCMTKIO_SOURCES}
)

target_link_libraries(ITKDCMTKIO
  ${DCMTK_LIBRARIES}
  itkIOAddOn
  ${ITK_LIBRARIES}
)

if(ITK_BUILD_SHARED)
    install (TARGETS ITKDCMTKIO  
    RUNTIME DESTINATION ${medinria-plugins_BINARY_OUTPUT_DIRECTORY}
    LIBRARY DESTINATION ${medinria-plugins_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE DESTINATION ${medinria-plugins_ARCHIVE_OUTPUT_DIRECTORY} )
endif(ITK_BUILD_SHARED)
