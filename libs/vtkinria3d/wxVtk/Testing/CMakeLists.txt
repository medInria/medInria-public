SET(KIT wxVtk)

SET(${KIT}Tests
  wxVTKRenderWindowInteractorTest.cxx
  wxVtkPageViewTest.cxx
#  wxVtkLandmarkWindowTest.cxx
  wxVtkViewImage2DWithSlidersTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ${wxVtkTests}
  )

ADD_EXECUTABLE(${KIT}CxxTests 
  ${Tests}
   wxMainApp.cxx
  )

TARGET_LINK_LIBRARIES(${KIT}CxxTests 
  ${wxWidgets_LIBRARIES}
  wxVtk
  vtkVisuManagement
  vtkRendering 
  vtkImaging
  )

FOREACH (test ${wxVtkTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${KIT}CxxTests ${TName}) 
ENDFOREACH (test) 
