project (itkDataTensorImageTests)

# we need the directory with the test data
FIND_PATH(MEDINRIA_TEST_DATA_ROOT MedINRIA_test_data.txt
  DOC "The directory where medInria test data is located. It is required by the Testing tree."
  )

if(NOT MEDINRIA_TEST_DATA_ROOT)
    message(WARNING "Please set MEDINRIA_TEST_DATA_ROOT to the directory where MedINRIA test data is located. It is required by the Testing tree.")
else()
    # path where the tests will be
    SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

    find_package(medInria REQUIRED)
    include(${medInria_USE_FILE})

    find_package(medInria-plugins REQUIRED)
    include(${medInria-plugins_USE_FILE})

    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})

    find_package(TTK REQUIRED)
    include(${TTK_USE_FILE})

   SET(MEDINRIA_PLUGINS_PATH "${medInria-plugins_DIR}/plugins")

    # configure file with defines for MEDINRIA_TEST_DATA_ROOT
    CONFIGURE_FILE( itkDataTensorImageTestConfig.h.in itkDataTensorImageTestConfig.h )

    ## #################################################################
    ## Setup CTest test wrapping a QTest (test with Qt testing framework)
    ## #################################################################

    # qtest uses one main function per test fixture
    # so for each one of them we need to create a new target

    SET(${PROJECT_NAME}_QTESTS_SRCS
      itkDataTensorImageTest.cxx
      )

    SET(${PROJECT_NAME}_MOC_CPPS
      itkDataTensorImageTest.cxx
      )

    QT4_WRAP_CPP(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_CPPS})

    ADD_CUSTOM_TARGET(${PROJECT_NAME}_itkDataTensorImageTest_moc ALL DEPENDS ${${PROJECT_NAME}_MOC_SRCS})

    ADD_EXECUTABLE(${PROJECT_NAME}_itkDataTensorImageTest ${${PROJECT_NAME}_QTESTS_SRCS})

    ADD_DEPENDENCIES(${PROJECT_NAME}_itkDataTensorImageTest ${PROJECT_NAME}_itkDataTensorImageTest_moc)

    TARGET_LINK_LIBRARIES(${PROJECT_NAME}_itkDataTensorImageTest
        ${QT_QTCORE_LIBRARY}
        ${QT_QTTEST_LIBRARY}
        ${QT_QTLOCATION_LIBRARY}
        ${QT_LIBRARIES}
        medSql
        dtkCore dtkLog
        medCore
        ITKTensor
        ${ITK_LIBRARIES}
        )

    ADD_TEST(${PROJECT_NAME}_itkDataTensorImageTest ${CXX_TEST_PATH}/${PROJECT_NAME}_itkDataTensorImageTest)
endif()

