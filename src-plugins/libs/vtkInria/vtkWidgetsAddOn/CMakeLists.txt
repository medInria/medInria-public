project(vtkWidgetsAddOn)

set(${PROJECT_NAME}_SRCS
vtkDistanceWidget2.cxx
vtkImageOrientedPointWidget.cxx
vtkOrientationAnnotation.cxx
vtkOrientedBoxWidget.cxx
vtkRulerWidget.cxx
vtkInteractorStyleTrackballCamera2.cxx
)

set(${PROJECT_NAME}_LIBS
 vtkCommon
 vtkFiltering 
 vtkRendering
 vtkWidgets
)

# C++ library for the InterfaceManagers
add_library(${PROJECT_NAME} SHARED
 ${${PROJECT_NAME}_SRCS}
)
  
target_link_libraries(${PROJECT_NAME}  
 ${${PROJECT_NAME}_LIBS}
)

## #################################################################
## Install rules
## #################################################################
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 
