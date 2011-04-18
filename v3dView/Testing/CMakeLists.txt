SET(PLUGIN_NAME v3dView)

SET(v3dViewPluginTests
  v3dViewTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${v3dViewPluginTests}
  )

ADD_EXECUTABLE(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${PLUGIN_NAME}CxxTests
  dtkCore
  ${ITK_LIBRARIES}
  vtkImageView
  vtkHybrid
  vtkImaging
  ${QT_LIBRARIES}
  )

set(CXX_TEST_PATH        ${EXECUTABLE_OUTPUT_PATH})
set(CXX_TEST_PLUGIN_PATH ${LIBRARY_OUTPUT_PATH})
if(MSVC)
  set(CXX_TEST_PLUGIN_PATH ${LIBRARY_OUTPUT_PATH}/Release)
endif(MSVC)

  set(ITK_DATA_ROOT
  )
if (ITK_SOURCE_DIR)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Examples/Data)
endif (ITK_SOURCE_DIR)

FOREACH (test ${v3dViewPluginTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests ${TName} ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
ENDFOREACH (test) 
