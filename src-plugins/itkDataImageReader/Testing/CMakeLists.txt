set(PLUGIN_NAME itkDataImageReaderPlugin)

set(itkDataImageReaderPluginTests
  itkMetaDataImageReaderTest.cxx
  )

CREATE_TEST_SOURCELIST(Tests ${PLUGIN_NAME}CxxTests.cxx
  ${itkDataImageReaderPluginTests}
  )

add_executable(${PLUGIN_NAME}CxxTests 
  ${Tests}
  )

target_link_libraries(${PLUGIN_NAME}CxxTests
  dtkCore dtkLog
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
  add_test(itkMetaDataImageReaderTest ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests itkMetaDataImageReaderTest ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
endif()
