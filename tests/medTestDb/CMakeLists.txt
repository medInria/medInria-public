### CMakeLists.txt ---
##
## Author: John Stark
## Copyright (C) 2011 - Thibaud Kloczko, Inria.
## Created: Wed May 25 16:00:00 2011 (+0100)
## Version: $Id$
## Last-Updated: Mon Jun 20 18:17:39 2011 (+0200)
##           By: Julien Wintz
##     Update #: 2
######################################################################
##
### Commentary:
##
######################################################################
##
### Change log:
##
######################################################################

## #################################################################
## Build rules
## #################################################################


project(medTestDb)

set(${PROJECT_NAME}_HEADERS_MOC
medTestDbApp.h)

set (${PROJECT_NAME}_SOURCES
main.cpp
medTestDbApp.cpp)

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_executable(${PROJECT_NAME}
${${PROJECT_NAME}_SOURCES_MOC}
${${PROJECT_NAME}_SOURCES}
${${PROJECT_NAME}_HEADERS_MOC}
)

target_link_libraries(${PROJECT_NAME}
  medCore medSql medTest dtkCore)
