
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

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkWidgetsAddOn
  LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install_files(${vtkINRIA3D_INSTALL_INCLUDE_DIR} .h ${vtkWidgetsAddOn_SRCS})
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
