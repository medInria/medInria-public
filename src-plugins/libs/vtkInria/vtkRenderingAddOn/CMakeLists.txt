project(vtkRenderingAddOn)

SET(${PROJECT_NAME}_SRCS
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
  ADD_LIBRARY(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES_PCH}
  ${${PROJECT_NAME}_SRCS}
  )
  
SET(${PROJECT_NAME}_LIBS
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

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
${${PROJECT_NAME}_LIBS}
)

IF(VTKINRIA_BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(VTKINRIA_BUILD_TESTING)

