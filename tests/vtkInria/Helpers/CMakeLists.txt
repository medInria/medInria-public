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

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/simplevolume.vtk
		${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk
		COPYONLY
		)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/tensors.vtk
		${CMAKE_CURRENT_BINARY_DIR}/tensors.vtk
		COPYONLY
		)


set(KIT vtkHelpers)

set(vtkHelpersTests
interpolation.cxx
sliceactor.cxx
)


create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${vtkHelpersTests}
  )

add_executable(${KIT}CxxTests 
  ${Tests}
  )

target_link_libraries(${KIT}CxxTests 
  vtkHelpers 
  vtkRendering
  vtkIO
  )


add_test(interpolation ${CXX_TEST_PATH}/${KIT}CxxTests interpolation "${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk")
add_test(sliceactor    ${CXX_TEST_PATH}/${KIT}CxxTests sliceactor    "${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk")
