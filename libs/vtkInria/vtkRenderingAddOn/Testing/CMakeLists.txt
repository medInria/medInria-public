SET(KIT vtkRenderingAddOn)

SET(vtkRenderingAddOnTests
  SynchronizedViewsTest.cxx
  TestViewImage2D.cxx
  TestViewImage2DWithTracer.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${vtkRenderingAddOnTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkRenderingAddOn 
  vtkRendering 
  vtkImaging
  )


FOREACH (test ${vtkRenderingAddOnTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
