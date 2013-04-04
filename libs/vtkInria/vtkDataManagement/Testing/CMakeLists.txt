SET(KIT ${PROJECT_NAME})

SET(${PROJECT_NAME}Tests
  vtkMetaDataSetTest.cxx
  vtkMetaImageDataTest.cxx
  vtkMetaSurfaceMeshTest.cxx
  vtkMetaVolumeMeshTest.cxx
  vtkDataManagerTest.cxx
)

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${${PROJECT_NAME}Tests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${PROJECT_NAME}
  vtkCommon
  )


FOREACH (test ${${PROJECT_NAME}Tests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
