SET(PLUGIN_NAME itkDataImageReaderPlugin)

SET(itkDataImageReaderPluginTests
  itkMetaDataImageReaderTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${itkDataImageReaderPluginTests}
  )

ADD_EXECUTABLE(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${PLUGIN_NAME}CxxTests
  dtkCore
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
  ADD_TEST(itkMetaDataImageReaderTest ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests itkMetaDataImageReaderTest ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
endif (ITK_SOURCE_DIR)
