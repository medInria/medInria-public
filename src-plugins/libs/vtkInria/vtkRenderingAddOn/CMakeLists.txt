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

project(vtkRenderingAddOn)

set(${PROJECT_NAME}_SRCS
  vtkFillImageWithPolyData.cxx
  vtkImageBlendWithMask.cxx
  vtkInteractorStyleImage2D.cxx
  vtkViewImage2DCommand.cxx
  vtkViewImage2DFullCommand.cxx
  vtkDataSet3DCroppingPlaneCallback.cxx
  vtkImage3DCroppingBoxCallback.cxx
  vtkImage3DImagePlaneCallback.cxx
  vtkImageOrientedPointWidgetCallback.cxx
  vtkImageTracerWidgetCallback.cxx
  vtkSynchronizedView.cxx
  vtkViewImage.cxx
  vtkViewImage2D.cxx
  vtkViewImage3D.cxx
  vtkViewImage2DWithOrientedPoint.cxx
  vtkViewImage2DWithTracer.cxx
)

if (VTKINRIA_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
    ${${PROJECT_NAME}_SRCS})
endif()

  # C++ library for the InterfaceManagers
  add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
  )
  
set(${PROJECT_NAME}_LIBS
  vtkWidgetsAddOn
  vtkCommon
  vtkRendering
  vtkWidgets
  vtkVolumeRendering
  vtkHybrid
  vtkImaging
  ITKCommon
  ITKBasicFilters
)

target_link_libraries(${PROJECT_NAME}
${${PROJECT_NAME}_LIBS}
)

if (VTKINRIA_BUILD_TESTING)
  add_subdirectory(Testing)
endif()


## #################################################################
## Install rules
## #################################################################
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 
