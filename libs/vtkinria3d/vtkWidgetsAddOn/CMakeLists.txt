project(vtkWidgetsAddOn)

SET(vtkWidgetsAddOn_SRCS
vtkDistanceWidget2.cxx
vtkImageOrientedPointWidget.cxx
vtkOrientationAnnotation.cxx
vtkOrientedBoxWidget.cxx
vtkRulerWidget.cxx
vtkInteractorStyleTrackballCamera2.cxx
)

SET(vtkWidgetsAddOn_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
)


    
# C++ library for the InterfaceManagers
ADD_LIBRARY(vtkWidgetsAddOn ${vtkINRIA3D_LIBS_STYLE}
 ${vtkWidgetsAddOn_SRCS}
)
  
TARGET_LINK_LIBRARIES(vtkWidgetsAddOn  
 ${vtkWidgetsAddOn_LIBS}
)


  IF (VTK_WRAP_TCL)
   
    VTK_WRAP_TCL3( vtkWidgetsAddOnLib
      vtkWidgetsAddOnTCL_SRCS
      "${vtkWidgetsAddOn_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )

    ADD_LIBRARY(vtkWidgetsAddOnTCL ${vtkINRIA3D_LIBS_STYLE}
    ${vtkWidgetsAddOnTCL_SRCS}
    )
    TARGET_LINK_LIBRARIES(vtkWidgetsAddOnTCL
    vtkWidgetsAddOn
    vtkFilteringTCL
    vtkRenderingTCL
    vtkWidgetsTCL
    )
 
    INSTALL_TARGETS(/lib vtkWidgetsAddOnTCL)

  ENDIF(VTK_WRAP_TCL)


