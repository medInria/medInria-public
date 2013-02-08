project(vtkRenderingAddOn)

  SET(vtkRenderingAddOn_SRCS
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

if(vtkInria_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(vtkRenderingAddOn_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
    ${vtkRenderingAddOn_SRCS})
endif(vtkInria_USE_PRECOMPILED_HEADERS)

  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkRenderingAddOn SHARED
  ${vtkRenderingAddOn_SOURCES_PCH}
  ${vtkRenderingAddOn_SRCS}
  )
  
SET(vtkRenderingAddOn_LIBS
    vtkWidgetsAddOn
    vtkCommon
    vtkRendering
    vtkWidgets
    vtkVolumeRendering
    vtkHybrid
    vtkImaging
)

IF( vtkInria_USE_ITK )
   SET(vtkRenderingAddOn_LIBS
   ITKCommon
   ITKBasicFilters
   ${vtkRenderingAddOn_LIBS}
   )
ENDIF( vtkInria_USE_ITK )


  TARGET_LINK_LIBRARIES(vtkRenderingAddOn
  ${vtkRenderingAddOn_LIBS}
  )

  IF (VTK_WRAP_TCL)
    SET_SOURCE_FILES_PROPERTIES(
      ${vtkRenderingAddOn_SRCS}
      WRAP_EXCLUDE
    )
    VTK_WRAP_TCL3(
      vtkRenderingAddOnLib
      vtkRenderingAddOnTCL_SRCS
      "${vtkRenderingAddOn_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )

    ADD_LIBRARY(vtkRenderingAddOnTCL SHARED
    ${vtkRenderingAddOnTCL_SRCS}
    )
    TARGET_LINK_LIBRARIES(vtkRenderingAddOnTCL
    vtkRenderingAddOn
    vtkCommonTCL
    vtkRenderingTCL
    vtkWidgetsTCL
    vtkVolumeRenderingTCL
    vtkHybridTCL
    vtkImagingTCL
    vtkRenderingAddOn
    )
 
    if(NOT vtkInria_INSTALL_NO_LIBRARIES)
      install(TARGETS vtkRenderingAddOnTCL
      LIBRARY DESTINATION ${vtkInria_INSTALL_LIB_DIR}
      RUNTIME DESTINATION ${vtkInria_INSTALL_BIN_DIR}
      ARCHIVE DESTINATION ${vtkInria_INSTALL_LIB_DIR}
      )
    endif(NOT vtkInria_INSTALL_NO_LIBRARIES)

  ENDIF(VTK_WRAP_TCL)

  IF(BUILD_TESTING)
    ADD_SUBDIRECTORY(Testing)
  ENDIF(BUILD_TESTING)

