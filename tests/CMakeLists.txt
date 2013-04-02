### CMakeLists.txt --- 
## 

######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(medTestDb)
add_subdirectory(medQSSParserTest)
add_subdirectory(medPluginGenerator)

enable_testing()

add_test(medTestDb ${CMAKE_BINARY_DIR}/bin/medTestDb ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeTmp)

if (MSVC)
  # We would like to use ${CTEST_CONFIGURATION_TYPE} but there seems to be no way to
  # escape the $ so that the variable is expanded at test run time.
  # set(CXX_TEST_CONFIGURATION "\${CTEST_CONFIGURATION_TYPE}")
  set(CXX_TEST_CONFIGURATION Release)

  #May need to add zlib path?
  set(PATH_STRING_FOR_TESTS "${medInria_DIR}/bin/${CXX_TEST_CONFIGURATION}")
  set(PATH_STRING_FOR_TESTS "${PATH_STRING_FOR_TESTS};${dtk_DIR}/bin/${CXX_TEST_CONFIGURATION}")
  set(PATH_STRING_FOR_TESTS "${PATH_STRING_FOR_TESTS};${QT_LIBRARY_DIR}")
else()
  set(PATH_STRING_FOR_TESTS "$ENV{PATH}")
endif()

# ';' deliminates stringlists in CMake, but here we want to preserve them.)
string(REPLACE ";" "\\;" PATH_STRING_FOR_TESTS "${PATH_STRING_FOR_TESTS}")

set_tests_properties(medTestDb PROPERTIES ENVIRONMENT "PATH=${PATH_STRING_FOR_TESTS}")
