### CMakeLists.txt --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Wed Jan  6 12:59:15 2010 (+0100)
## Version: $Id$
## Last-Updated: Wed Jan  6 13:18:18 2010 (+0100)
##           By: Julien Wintz
##     Update #: 7
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

add_custom_target(doc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html index.html
  COMMENT "-- Generating documentation")

subdirs(user)
subdirs(developer)
