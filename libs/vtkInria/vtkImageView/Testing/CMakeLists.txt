SET(KIT ${PROJECT_NAME})

SET(${PROJECT_NAME}Tests
  vtkImageViewTest1.cxx
  vtkImageViewTest2.cxx
  vtkImageViewTest3.cxx
  SynchronizedViews3Test.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkRendering 
  vtkImaging
  )

FOREACH (test ${${PROJECT_NAME}Tests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
