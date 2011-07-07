## #################################################################
## Dependencies and Set Up
## #################################################################

# we need path to medinria plugins
FIND_PATH(MEDINRIA_PLUGINS_PATH libitkDataImageReaderPlugin.so
  DOC "The directory where medinria plugins are. It is required by the Testing tree."
  )

if(NOT MEDINRIA_PLUGINS_PATH)
    message(SEND_ERROR "Please set MEDINRIA_PLUGINS_PATH to the directory where MedINRIA plugins are located. It is required by the Testing tree.")
endif(NOT MEDINRIA_PLUGINS_PATH)

# and the directory with the test data
FIND_PATH(MEDINRIA_TEST_DATA_ROOT MedINRIA_test_data.txt
  DOC "The directory where medinria test data is located. It is required by the Testing tree."
  )

if(NOT MEDINRIA_TEST_DATA_ROOT)
    message(SEND_ERROR "Please set MEDINRIA_TEST_DATA_ROOT to the directory where MedINRIA test data is located. It is required by the Testing tree.")
endif(NOT MEDINRIA_TEST_DATA_ROOT)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

CONFIGURE_FILE( medSqlTest.config.h.cmake.in medSqlTest.config.h )

## #################################################################
## Setup CTest test wrapping a QTest (test with Qt testing framework)
## #################################################################

SET(${PROJECT_NAME}_QTESTS_SRCS
  medDatabaseImporterTest.cxx
  )

SET(${PROJECT_NAME}_MOC_CPPS
  medDatabaseImporterTest.cxx
  )

QT4_WRAP_CPP(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_CPPS})

ADD_CUSTOM_TARGET(${PROJECT_NAME}_medDatabaseImporterTest_moc ALL DEPENDS ${${PROJECT_NAME}_MOC_SRCS})

ADD_EXECUTABLE(${PROJECT_NAME}_medDatabaseImporterTest ${${PROJECT_NAME}_QTESTS_SRCS})

ADD_DEPENDENCIES(${PROJECT_NAME}_medDatabaseImporterTest ${PROJECT_NAME}_medDatabaseImporterTest_moc)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_medDatabaseImporterTest
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTGUI_LIBRARY} 
    ${QT_QTLOCATION_LIBRARY}
    ${QT_LIBRARIES}
    medSql
    dtkCore
    medCore
    )

ADD_TEST(${PROJECT_NAME}_medDatabaseImporterTest ${CXX_TEST_PATH}/${PROJECT_NAME}_medDatabaseImporterTest)

