
set(PLUGIN_NAME v3dView)

set(v3dViewPluginTests
  v3dViewTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${v3dViewPluginTests}
  )

ADD_EXECUTABLE(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${PLUGIN_NAME}CxxTests
  medGui
  medCore
  dtkCore dtkLog  
  ${ITK_LIBRARIES}
  vtkImageView
  vtkHybrid
  vtkImaging
  ${QT_LIBRARIES}
  )

set_target_properties(${PLUGIN_NAME}CxxTests PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)

set(CXX_TEST_PATH        ${EXECUTABLE_OUTPUT_PATH})
set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if (MSVC)
  set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release)
endif()

  set(ITK_DATA_ROOT
  )
if (ITK_SOURCE_DIR)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Examples/Data)
endif()

FOREACH (test ${v3dViewPluginTests})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests ${TName} ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
ENDFOREACH (test) 
