### CMakeLists.txt ---
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu May 28 12:53:44 2009 (+0200)
## Version: $Id$
## Last-Updated: Thu May 28 13:12:56 2009 (+0200)
##           By: Julien Wintz
##     Update #: 3
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medItk)

set(${PROJECT_NAME}_HEADERS
itkMultiThreadedImageIOBase.h
itkDCMTKImageIO.h
)

set(${PROJECT_NAME}_SOURCES
medItk.cpp
itkMultiThreadedImageIOBase.cxx
itkDCMTKImageIO.cxx
)

## #################################################################
## MSVC config
## #################################################################

source_group("Generated\\Metaobject Files" REGULAR_EXPRESSION "moc_.*c[xp]*$" )
source_group("Generated\\Python Modules" REGULAR_EXPRESSION ".*py")
source_group("Generated\\CMake Rules" REGULAR_EXPRESSION ".*rule")
source_group("Header Files" FILES ${${PROJECT_NAME}_HEADERS})

## #################################################################
## Build rules
## #################################################################

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

find_package(vtkINRIA3D)
include(${vtkINRIA3D_USE_FILE})

find_package(VTK)
include(${VTK_USE_FILE})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES} 
  ${${PROJECT_NAME}_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HEADERS}")

target_link_libraries(${PROJECT_NAME}
  ${ITK_LIBRARIES}
  ${DCMTK_LIBRARIES}
)

if (vtkINRIA3D_FOUND AND VTK_FOUND)
   add_executable(itkDcmtkReaderTest
   itkDcmtkReaderTest.cxx
   )
   target_link_libraries(itkDcmtkReaderTest
   ${PROJECT_NAME}
   vtkRenderingAddOn
   vtkRendering
   vtkIO
   vtkDataManagement
   ${ITK_LIBRARIES}
   ${DCMTK_LIBRARIES}
   z
   )
endif (vtkINRIA3D_FOUND AND VTK_FOUND)
