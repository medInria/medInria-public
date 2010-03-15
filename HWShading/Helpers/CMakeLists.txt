
SET(Helpers_classes
vtkImageDataInterpolator
vtkBetterDataReader
vtkDoubleToUCharFilter
vtkImageSliceActor
vtkImageOrthogonalSlicesActor
)

FOREACH(class ${Helpers_classes})
  SET(Helpers_SRCS 
  ${Helpers_SRCS} ${class}.h ${class}.cxx )
ENDFOREACH(class)

ADD_LIBRARY(vtkHelpers ${vtkINRIA3D_LIBS_STYLE}
${Helpers_SRCS} 
)

TARGET_LINK_LIBRARIES(vtkHelpers
vtkCommon
vtkIO
vtkFiltering
vtkImaging
vtkGraphics
vtkRendering
)

IF( BUILD_TESTING )
  ADD_SUBDIRECTORY( Testing )
ENDIF( BUILD_TESTING )

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install_targets(
  ${vtkINRIA3D_INSTALL_LIB_DIR}
  RUNTIME_DIRECTORY ${vtkINRIA3D_INSTALL_BIN_DIR}
  vtkHelpers
  )
endif(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install_files(${vtkINRIA3D_INSTALL_INCLUDE_DIR} .h ${Helpers_SRCS})
endif(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
