#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

project (itkDataTensorImageTests)

# we need the directory with the test data
find_path(MEDINRIA_TEST_DATA_ROOT MedINRIA_test_data.txt
  DOC "The directory where medInria test data is located. It is required by the Testing tree."
  )

if (NOT MEDINRIA_TEST_DATA_ROOT)
    message(WARNING "Please set MEDINRIA_TEST_DATA_ROOT to the directory where MedINRIA test data is located. It is required by the Testing tree.")
else()
    # path where the tests will be
    set(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})

    find_package(TTK REQUIRED)
    include(${TTK_USE_FILE})

   set(MEDINRIA_PLUGINS_PATH "${medInria-plugins_DIR}/bin")

    # configure file with defines for MEDINRIA_TEST_DATA_ROOT
    configure_file( itkDataTensorImageTestConfig.h.in itkDataTensorImageTestConfig.h )

    ## #################################################################
    ## Setup CTest test wrapping a QTest (test with Qt testing framework)
    ## #################################################################

    # qtest uses one main function per test fixture
    # so for each one of them we need to create a new target

    set(${PROJECT_NAME}_QTESTS_SRCS
      itkDataTensorImageTest.cxx
      )

    set(${PROJECT_NAME}_MOC_CPPS
      itkDataTensorImageTest.cxx
      )

    qt4_wrap_cpp(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_CPPS})

    add_custom_target(${PROJECT_NAME}_itkDataTensorImageTest_moc ALL DEPENDS ${${PROJECT_NAME}_MOC_SRCS})

    add_executable(${PROJECT_NAME}_itkDataTensorImageTest ${${PROJECT_NAME}_QTESTS_SRCS})

    add_dependencies(${PROJECT_NAME}_itkDataTensorImageTest ${PROJECT_NAME}_itkDataTensorImageTest_moc)

    target_link_libraries(${PROJECT_NAME}_itkDataTensorImageTest
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

    add_test(${PROJECT_NAME}_itkDataTensorImageTest ${CXX_TEST_PATH}/${PROJECT_NAME}_itkDataTensorImageTest)
endif()

