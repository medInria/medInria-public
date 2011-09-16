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


if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkWidgetsAddOn
  LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install(DIRECTORY ./ 
        DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
