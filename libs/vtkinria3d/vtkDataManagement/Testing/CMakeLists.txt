SET(KIT vtkDataManagement)

SET(vtkDataManagementTests
  vtkMetaDataSetTest.cxx
  vtkMetaImageDataTest.cxx
  vtkMetaSurfaceMeshTest.cxx
  vtkMetaVolumeMeshTest.cxx
  vtkDataManagerTest.cxx
)

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${vtkDataManagementTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  vtkCommon
  vtkDataManagement
  )


FOREACH (test ${vtkDataManagementTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
