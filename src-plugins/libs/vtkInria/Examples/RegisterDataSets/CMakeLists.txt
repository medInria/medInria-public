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

project(RegisterDataSets)

include_directories(
  ../../vtkImageView
  ../../vtkDataManagement
  )

include_directories(${PROJECT_SOURCE_DIR})

if (APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    RegisterDataSets.cxx
    )  
else()
  add_executable(${PROJECT_NAME}
    RegisterDataSets.cxx
    )  
endif()

target_link_libraries(${PROJECT_NAME}
  vtkImageView
  vtkDataManagement
  )
