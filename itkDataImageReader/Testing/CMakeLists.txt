SET(PLUGIN_NAME itkDataImageReaderPlugin)

SET(itkDataImageReaderPluginTests
  itkMetaDataImageReaderTest.cxx
  itkDCMTKDataImageReaderTest.cxx
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

  set(ITK_DATA_ROOT
  )
if (ITK_SOURCE_DIR)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Examples/Data)
  ADD_TEST(itkMetaDataImageReaderTest ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests itkMetaDataImageReaderTest ${CXX_TEST_PLUGIN_PATH} ${ITK_DATA_ROOT})
endif (ITK_SOURCE_DIR)

find_path(DICOM_DATA_ROOT "dicom.lst")

if (DICOM_DATA_ROOT)
  file(STRINGS "${DICOM_DATA_ROOT}/dicom.lst" DICOM_LIST)
  foreach(DICOM_DIR ${DICOM_LIST})
    ADD_TEST(itkDCMTKDataImageReaderTest_${DICOM_DIR} ${CXX_TEST_PATH}/${PLUGIN_NAME}CxxTests itkDCMTKDataImageReaderTest ${CXX_TEST_PLUGIN_PATH} "${DICOM_DATA_ROOT}/${DICOM_DIR}")
  endforeach()
endif(DICOM_DATA_ROOT)
