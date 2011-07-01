## #################################################################
## Dependencies
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

## #################################################################
## Setup CTest test source file
## #################################################################

# Add tests' source files to this variable
SET(${PROJECT_NAME}Tests
  testWithCTest.cxx
  medDatabaseControllerImplTest.cxx
  )

# This command creates a driver that will run all the tests
CREATE_TEST_SOURCELIST(Tests ${PROJECT_NAME}TestDriver.cxx
  ${${PROJECT_NAME}Tests}
  )

ADD_EXECUTABLE(${PROJECT_NAME}CxxTests 
  ${Tests}
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}CxxTests
  ${QT_LIBRARIES}
  medSql
  )

SET_TARGET_PROPERTIES(${PROJECT_NAME}CxxTests PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  )

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# This commands adds a new test that will be run by the driver
# the name of the test is the first parameter and the third parameter is the string
# that the driver will use to call the test, hence has to be the exact name of the test function
ADD_TEST(testWithCTest ${CXX_TEST_PATH}/${PROJECT_NAME}CxxTests testWithCTest)
ADD_TEST(medDatabaseControllerImplTest ${CXX_TEST_PATH}/${PROJECT_NAME}CxxTests medDatabaseControllerImplTest ${MEDINRIA_PLUGINS_PATH} ${MEDINRIA_TEST_DATA_ROOT})


## #################################################################
## Setup CTest test wrapping a QTest (test with Qt testing framework)
## #################################################################

SET(${PROJECT_NAME}_QTESTS_SRCS
  testWithQtTestLib.cxx
  )

SET(${PROJECT_NAME}_MOC_CPPS
  testWithQtTestLib.cxx
  )

QT4_WRAP_CPP(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_CPPS})

ADD_CUSTOM_TARGET(${PROJECT_NAME}_testmoc ALL DEPENDS ${${PROJECT_NAME}_MOC_SRCS})

ADD_EXECUTABLE(${PROJECT_NAME}_qt_tests ${${PROJECT_NAME}_QTESTS_SRCS})

ADD_DEPENDENCIES(${PROJECT_NAME}_qt_tests ${PROJECT_NAME}_testmoc)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_qt_tests
            ${QT_QTCORE_LIBRARY}
            ${QT_QTTEST_LIBRARY}
            ${QT_QTGUI_LIBRARY} 
            ${QT_QTLOCATION_LIBRARY}
        )

ADD_TEST(${PROJECT_NAME}_qt_tests ${CXX_TEST_PATH}/${PROJECT_NAME}_qt_tests)
