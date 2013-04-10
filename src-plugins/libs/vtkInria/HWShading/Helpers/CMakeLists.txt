

set(Helpers_classes
    vtkImageDataInterpolator
    vtkBetterDataReader
    vtkDoubleToUCharFilter
    vtkImageSliceActor
    vtkImageOrthogonalSlicesActor
    )

FOREACH(class ${Helpers_classes})
  set(Helpers_SRCS    ${Helpers_SRCS}    ${class}.cxx)
  set(Helpers_Headers ${Helpers_Headers} ${class}.h)
ENDFOREACH(class)

ADD_LIBRARY(vtkHelpers SHARED ${Helpers_SRCS})

TARGET_LINK_LIBRARIES(vtkHelpers
  vtkCommon
  vtkIO
  vtkFiltering
  vtkImaging
  vtkGraphics
  vtkRendering
  )

if ( VTKINRIA_BUILD_TESTING )
  ADD_SUBDIRECTORY( Testing )
endif()


