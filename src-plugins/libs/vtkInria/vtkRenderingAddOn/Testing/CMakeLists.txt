set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  SynchronizedViewsTest.cxx
  TestViewImage2D.cxx
  TestViewImage2DWithTracer.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

add_executable(${KIT}CxxTests 
  ${Tests}
  )

target_link_libraries(${KIT}CxxTests 
  ${PROJECT_NAME} 
  vtkRendering 
  vtkImaging
  )


foreach (test ${${PROJECT_NAME}Tests})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
endforeach() 
