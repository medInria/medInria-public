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

if(${PROJECT_NAME}_USE_PRECOMPILED_HEADERS)
  file(GLOB MEDPCH_H
    medPch/*.h
    )

  file(GLOB MEDPCH_CPP
    medPch/*.cpp
    )
endif()

## #############################################################################
## Add subdirectories
## #############################################################################

## be sure medCore  is first in the list, it will be used by the other lib.
add_subdirectory(medCore)

add_subdirectory(medTest)
add_subdirectory(medPacs)

add_subdirectory(medLog)
add_subdirectory(medImageIO)
add_subdirectory(medRegistration)

add_subdirectory(medVtkInria)

## #############################################################################
## complete medInria_INCLUDE_DIRS
## #############################################################################

set(${PROJECT_NAME}_INCLUDE_DIRS
    ${QT_HEADERS}
  ${medCore_INCLUDE_DIRS}
  ${medGui_INCLUDE_DIRS}
  ${medPacs_INCLUDE_DIRS}
  ${medTest_INCLUDE_DIRS}
  ${medSql_INCLUDE_DIRS}
  ${medLog_INCLUDE_DIRS}
  ${medImageIO_INCLUDE_DIRS}
  ${medRegistration_INCLUDE_DIRS}
  ${medVtkInria_INCLUDE_DIRS}
  ${${PROJECT_NAME}_INCLUDE_DIRS}
  PARENT_SCOPE
  )
  
