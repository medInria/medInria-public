
SET(Helpers_classes
vtkImageDataInterpolator
vtkBetterDataReader
vtkDoubleToUCharFilter
vtkImageSliceActor
vtkImageOrthogonalSlicesActor
)

FOREACH(class ${Helpers_classes})
  SET(Helpers_SRCS    ${Helpers_SRCS}    ${class}.cxx)
  SET(Helpers_Headers ${Helpers_Headers} ${class}.h)
ENDFOREACH(class)

ADD_LIBRARY(vtkHelpers ${vtkINRIA3D_LIBS_STYLE} ${Helpers_SRCS})

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
ENDIF()

if(NOT vtkINRIA3D_INSTALL_NO_LIBRARIES)
  install(TARGETS vtkHelpers
          LIBRARY DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR}
          RUNTIME DESTINATION ${vtkINRIA3D_INSTALL_BIN_DIR}
          ARCHIVE DESTINATION ${vtkINRIA3D_INSTALL_LIB_DIR})
endif()

if(NOT vtkINRIA3D_INSTALL_NO_DEVELOPMENT)
  install(FILES ${Helpers_Headers} DESTINATION ${vtkINRIA3D_INSTALL_INCLUDE_DIR} COMPONENT devel)
endif()
