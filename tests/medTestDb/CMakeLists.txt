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

project(medTestDb)

set(${PROJECT_NAME}_HEADERS_MOC medTestDbApp.h)

set (${PROJECT_NAME}_SOURCES main.cpp medTestDbApp.cpp)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SOURCES_MOC}
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS_MOC})

target_link_libraries(${PROJECT_NAME} medCore medSql medTest dtkCore)
