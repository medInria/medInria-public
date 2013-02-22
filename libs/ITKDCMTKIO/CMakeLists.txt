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

find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})

set(MEDINRIA_REQUIRES_VTK TRUE) # for itkMultiThreadedImageIO in vtkInria3d

set (${PROJECT_NAME}_HEADERS 
    itkDCMTKImageIO.h)

set(${PROJECT_NAME}_SOURCES
  itkDCMTKImageIO.cxx
)

set(LIBRARY_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medInria-plugins_LIBRARY_OUTPUT_DIRECTORY})
set(ARCHIVE_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medInria-plugins_ARCHIVE_OUTPUT_DIRECTORY})
set(RUNTIME_OUTPUT_DIRECTORY    ${PROJECT_BINARY_DIR}/${medInria-plugins_RUNTIME_OUTPUT_DIRECTORY})
set(EXECUTABLE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${medInria-plugins_RUNTIME_OUTPUT_DIRECTORY})


add_library(ITKDCMTKIO ${ITK_LIB_STYLE}
  ${ITKDCMTKIO_SOURCES}
)

target_link_libraries(ITKDCMTKIO
  ${DCMTK_LIBRARIES}
  itkIOAddOn
  ${ITK_LIBRARIES}
)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME})
install (TARGETS ITKDCMTKIO  
    RUNTIME DESTINATION ${medInria-plugins_BINARY_OUTPUT_DIRECTORY}
    LIBRARY DESTINATION ${medInria-plugins_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE DESTINATION ${medInria-plugins_ARCHIVE_OUTPUT_DIRECTORY} )

