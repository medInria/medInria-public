
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/simplevolume.vtk
		${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk
		COPYONLY
		)

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/tensors.vtk
		${CMAKE_CURRENT_BINARY_DIR}/tensors.vtk
		COPYONLY
		)


SET(KIT vtkHelpers)

SET(vtkHelpersTests
interpolation.cxx
sliceactor.cxx
)


CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${vtkHelpersTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkHelpers 
  vtkRendering
  vtkIO
  )


ADD_TEST(interpolation ${CXX_TEST_PATH}/${KIT}CxxTests interpolation "${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk")
ADD_TEST(sliceactor    ${CXX_TEST_PATH}/${KIT}CxxTests sliceactor    "${CMAKE_CURRENT_BINARY_DIR}/simplevolume.vtk")
