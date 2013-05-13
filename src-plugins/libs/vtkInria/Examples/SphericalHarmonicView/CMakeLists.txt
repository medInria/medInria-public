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

project(SHVisu)

include_directories(
  ../../vtkVisuManagement
  ../../vtkRenderingAddOn
  )

if (APPLE)
    set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()

add_executable(${PROJECT_NAME} 
  ${GUI_EXECUTABLE}
  SHManager.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkVisuManagement
  vtkRenderingAddOn
  )

