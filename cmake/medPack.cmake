### medPack.cmake --- 
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu Jun 10 10:21:17 2010 (+0200)
## Version: $Id$
## Last-Updated: Thu Jun 10 10:41:55 2010 (+0200)
##           By: Julien Wintz
##     Update #: 12
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

if(UNIX AND NOT APPLE)
  set (CPACK_DEBIAN_PACKAGE_DEPENDS "dtk (>= ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})") 
endif(UNIX AND NOT APPLE)
