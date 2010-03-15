
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


  # C++ library for the InterfaceManagers
  ADD_LIBRARY(vtkRenderingAddOn ${vtkINRIA3D_LIBS_STYLE}
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

IF( vtkINRIA3D_USE_ITK )
   SET(vtkRenderingAddOn_LIBS
   ITKCommon
   ITKBasicFilters
   ${vtkRenderingAddOn_LIBS}
   )
ENDIF( vtkINRIA3D_USE_ITK )


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

    ADD_LIBRARY(vtkRenderingAddOnTCL ${vtkINRIA3D_LIBS_STYLE}
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
 
    if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
      install_targets(
      ${vtkINRIA3D_INSTALL_LIB_DIR}
      RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
      vtkRenderingAddOnTCL
      )
    endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  ENDIF(VTK_WRAP_TCL)

  IF(BUILD_TESTING)
    ADD_SUBDIRECTORY(Testing)
  ENDIF(BUILD_TESTING)

  if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
    install_targets(
    ${vtkINRIA3D_INSTALL_LIB_DIR}
    RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
    vtkRenderingAddOn
    )
  endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

  if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
    install_files(${vtkINRIA3D_INSTALL_INCLUDE_DIR} .h ${vtkRenderingAddOn_SRCS})
  endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
