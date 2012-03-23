SET(KIT vtkVisuManagement)

SET(vtkVisuManagementTests
  vtkImageFromBoundsSourceTest.cxx
  vtkSphericalHarmonicSourceTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${vtkVisuManagementTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkVisuManagement
  vtkRendering 
  vtkImaging
  )


FOREACH (test ${vtkVisuManagementTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
