### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Tue Feb  2 20:18:37 2010 (+0100)
## Version: $Id$
## Last-Updated: Fri Apr  2 11:23:11 2010 (+0200)
##           By: Julien Wintz
##     Update #: 39
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
## Retrieve dependencies
## #################################################################

set(DOXYGEN_FIND_QUIETLY TRUE)

find_package(Doxygen)

## #################################################################
## Build rules
## #################################################################

if (DOXYGEN_FOUND)
  configure_file(Doxyfile.in     ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile IMMEDIATE @ONLY)
  configure_file(qhc/medinria.qhcp.in ${CMAKE_CURRENT_BINARY_DIR}/medinria.qhcp IMMEDIATE @ONLY)

  add_custom_target(doc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.xml  ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenHeader.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenHeader.html
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenFooter.html ${CMAKE_CURRENT_BINARY_DIR}/DoxygenFooter.html
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img      ${CMAKE_CURRENT_BINARY_DIR}/html/img
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/css      ${CMAKE_CURRENT_BINARY_DIR}/html/css
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND qhelpgenerator html/index.qhp -o medinria.qch
    COMMAND qcollectiongenerator medinria.qhcp -o medinria.qhc
    COMMENT "-- Generating embedded documentation"
    )

  install(FILES ${CMAKE_BINARY_DIR}/doc/medinria.qhc DESTINATION doc)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION doc)
else (DOXYGEN_FOUND)
  message(INFO "Can't find doxygen. Can't build documentation")
endif (DOXYGEN_FOUND)
