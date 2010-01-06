### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Tue Jan  5 11:11:49 2010 (+0100)
## Version: $Id$
## Last-Updated: Wed Jan  6 13:17:31 2010 (+0100)
##           By: Julien Wintz
##     Update #: 24
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(developer_doc)

set(DOXYGEN_FIND_QUIETLY TRUE)

find_package(Doxygen)

## #################################################################
## Build rules
## #################################################################

if (DOXYGEN_FOUND)

  configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile IMMEDIATE @ONLY)

  add_custom_target(developer_doc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/css css
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img img
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/js js
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/header.html header.html
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/footer.html footer.html
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMENT "-- Generating developer documentation")

else (DOXYGEN_FOUND)

  message(INFO "Can't find doxygen. Can't build developer documentation")

endif (DOXYGEN_FOUND)
