SET(KIT KWAddOn)

SET(KWAddOnTests
  INRIAViz4DTest.cxx
  vtkKWPageViewTest.cxx
  vtkKWToolboxManagerTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${KWAddOnTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${KWWidgets_LIBRARIES}
  vtkCommon
  vtkDataManagement
  vtkDataManagementTCL
  ${vtkINRIA3D_LIB_RENDERING}
  ${vtkINRIA3D_LIB_RENDERING}TCL
  vtkVisuManagement
  KWAddOn
  )


FOREACH (test ${KWAddOnTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
