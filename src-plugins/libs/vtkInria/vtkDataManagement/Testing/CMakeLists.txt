set(KIT ${PROJECT_NAME})

set(${PROJECT_NAME}Tests
  vtkMetaDataSetTest.cxx
  vtkMetaImageDataTest.cxx
  vtkMetaSurfaceMeshTest.cxx
  vtkMetaVolumeMeshTest.cxx
  vtkDataManagerTest.cxx
)

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

add_executable(${KIT}CxxTests 
  ${Tests}
  )

target_link_libraries(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkCommon
  )


foreach (test ${${PROJECT_NAME}Tests})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
endforeach() 
