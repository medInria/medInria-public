################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

project(medCoreTests)

## #############################################################################
## QSS Parser Test
## #############################################################################

include_directories(${medCore_INCLUDE_DIRS}
                   )
add_executable(medQssParserTest
               medQSSParserTest.cpp
               medQSSParserTest.h
              )
target_link_libraries(medQssParserTest
                      ${QT_LIBRARIES}
                      dtkCore
                      medCore
                     )
add_test(medQssParserTest ${CMAKE_BINARY_DIR}/bin/medQssParserTest)


## #############################################################################
## Data Manager Test
## #############################################################################
#include_directories(${medCore_INCLUDE_DIRS}
#                    ${medTest_INCLUDE_DIRS}
#                   )
#add_executable(medDataManagerTest
#               medDataManagerTest.cpp
#               medDataManagerTest.h
#              )
#target_link_libraries(medDataManagerTest
#                      ${QT_LIBRARIES}
#                      dtkCore
#                      medCore
#                      medTest
#                     )
#add_test(medDataManagerTest ${CMAKE_BINARY_DIR}/bin/medDataManagerTest)
