project(vtkImageView)

# find_package(VTK REQUIRED)
# include(${VTK_USE_FILE})

# if(vtkInria_USE_ITK)
#   find_package(ITK REQUIRED)
#   include(${ITK_USE_FILE})
# endif(vtkInria_USE_ITK)

set(${PROJECT_NAME}_HEADERS
vtkImageView.h
vtkImageView2D.h
vtkImageView3D.h
vtkInteractorStyleImageView2D.h
vtkImageView2DCommand.h
vtkImageViewCornerAnnotation.h
vtkAxes2DWidget.h
vtkDataSet2DWidget.h
vtkImageViewCollection.h
vtkImageView3DCroppingBoxCallback.h
vtkSmartVolumeMapper.h
)

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
if(vtkInria_USE_PRECOMPILED_HEADERS)
  add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInria3dPch/vtkInria3dPch.h" "../vtkInria3dPch/vtkInria3dPch.cpp" 
    ${${PROJECT_NAME}_SOURCES})
endif(vtkInria_USE_PRECOMPILED_HEADERS)

add_library(${PROJECT_NAME} SHARED
${${PROJECT_NAME}_SOURCES_PCH}
${${PROJECT_NAME}_SOURCES}
${${PROJECT_NAME}_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
vtkRendering
vtkWidgets
vtkWidgetsAddOn
vtkVolumeRendering
vtkVisuManagement
)

IF( vtkInria_USE_ITK )
   target_link_libraries(${PROJECT_NAME}
   ITKCommon
   ITKBasicFilters
   )
ENDIF( vtkInria_USE_ITK )

  IF (VTK_WRAP_TCL)
   
    VTK_WRAP_TCL3( vtkImageViewLib
      vtkImageViewTCL_SRCS
      "${vtkImageView_SRCS}"
      ""
      "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}"
    )

    ADD_LIBRARY(vtkImageViewTCL SHARED
    ${vtkImageViewTCL_SRCS}
    )
    TARGET_LINK_LIBRARIES(vtkImageViewTCL
    vtkImageView
    vtkCommonTCL
    vtkRenderingTCL
    vtkWidgetsTCL
    vtkWidgetsAddOnTCL
    vtkVolumeRenderingTCL
    vtkVisuManagementTCL
    )
  
    INSTALL_TARGETS(/lib vtkImageViewTCL)

  ENDIF(VTK_WRAP_TCL)



IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

# include header files in group for visual studio
SOURCE_GROUP( "Header Files" FILES ${${PROJECT_NAME}_HEADERS})


