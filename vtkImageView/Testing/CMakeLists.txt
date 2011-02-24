SET(KIT vtkImageView)

SET(vtkImageViewTests
  vtkImageViewTest1.cxx
  vtkImageViewTest2.cxx
  SynchronizedViews3Test.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${vtkImageViewTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkImageView
  vtkRendering 
  vtkImaging
  )


FOREACH (test ${vtkImageViewTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
