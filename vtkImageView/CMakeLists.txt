project(vtkImageView)

# find_package(VTK REQUIRED)
# include(${VTK_USE_FILE})

# if(vtkINRIA3D_USE_ITK)
#   find_package(ITK REQUIRED)
#   include(${ITK_USE_FILE})
# endif(vtkINRIA3D_USE_ITK)

set(${PROJECT_NAME}_SOURCES
vtkImageView.cxx
vtkImageView2D.cxx
vtkImageView3D.cxx
vtkInteractorStyleImageView2D.cxx
vtkImageView2DCommand.cxx
vtkImageViewCornerAnnotation.cxx
vtkAxes2DWidget.cxx
vtkDataSet2DWidget.cxx
vtkImageViewCollection.cxx
vtkImageView3DCroppingBoxCallback.cxx
vtkSmartVolumeMapper.cxx
)

add_library(${PROJECT_NAME} ${vtkINRIA3D_LIBS_STYLE}
${${PROJECT_NAME}_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
vtkRendering
vtkWidgets
vtkWidgetsAddOn
vtkVolumeRendering
)

IF( vtkINRIA3D_USE_ITK )
   target_link_libraries(${PROJECT_NAME}
   ITKCommon
   ITKBasicFilters
   )
ENDIF( vtkINRIA3D_USE_ITK )

target_link_libraries(
${PROJECT_NAME}
)


IF(BUILD_TESTING)
  add_executable(vtkImageViewTest
    vtkImageViewTest.cxx
)
  target_link_libraries(vtkImageViewTest
    ${PROJECT_NAME}
#vtkVisuManagement
)
ENDIF(BUILD_TESTING)



IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)


if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkImageView
      LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
      RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install(DIRECTORY  ./
      DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR}
      FILES_MATCHING PATTERN "*.h"
      )
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
