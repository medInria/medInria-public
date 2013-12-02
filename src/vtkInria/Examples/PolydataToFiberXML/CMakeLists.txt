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

project(PolydataToFiberXML)

include_directories(
  ../../vtkRenderingAddOn
  ../../vtkDataManagement
  )

add_executable(${PROJECT_NAME}
  PolydataToFiberXML.cxx
  )

target_link_libraries(${PROJECT_NAME}
  vtkDataManagement
  vtkRendering
  vtkIO
  )


