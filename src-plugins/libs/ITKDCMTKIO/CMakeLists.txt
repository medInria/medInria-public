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

## #################################################################
## Install rules
## #################################################################
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 

