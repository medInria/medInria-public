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

set(CXX_TEST_PATH        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if(MSVC)
  set(CXX_TEST_PLUGIN_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release)
endif(MSVC)

set(ITK_DATA_ROOT
  )

if (ITK_SOURCE_DIR)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Examples/Data)
  ADD_TEST(itkMetaDataImageReaderTest ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests itkMetaDataImageReaderTest ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
endif (ITK_SOURCE_DIR)
