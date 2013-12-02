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

project(DataManager)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  ../../vtkVisuManagement
  )

if (APPLE)
    set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

add_executable(${PROJECT_NAME} ${GUI_EXECUTABLE}
  DataManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkDataManagement
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )

add_executable(FiberDataSetReader
  FiberDataSetReader.cxx
  )

target_link_libraries(FiberDataSetReader
  vtkDataManagement
  vtkRenderingAddOn
  vtkVisuManagement
  vtkRendering
  vtkIO
  )


