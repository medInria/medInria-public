#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

set(Helpers_classes
    vtkImageDataInterpolator
    vtkBetterDataReader
    vtkDoubleToUCharFilter
    vtkImageSliceActor
    vtkImageOrthogonalSlicesActor
    )

foreach (class ${Helpers_classes})
  set(Helpers_SRCS    ${Helpers_SRCS}    ${class}.cxx)
  set(Helpers_Headers ${Helpers_Headers} ${class}.h)
endforeach()

add_library(vtkHelpers SHARED ${Helpers_SRCS})

target_link_libraries(vtkHelpers
  vtkCommon
  vtkIO
  vtkFiltering
  vtkImaging
  vtkGraphics
  vtkRendering
  )

if ( VTKINRIA_BUILD_TESTING )
  add_subdirectory( Testing )
endif()

## #################################################################
## Install rules
## #################################################################
install(TARGETS vtkHelpers
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${PROJECT_NAME}) 
