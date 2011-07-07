### medBuild.cmake ---
##
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Tue Apr  6 21:41:29 2010 (+0200)
## Version: $Id$
## Last-Updated: Tue May  3 16:50:17 2011 (+0200)
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


## ###################################################################
## INCLUDE RULES
## ###################################################################
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/medGui
    ${PROJECT_SOURCE_DIR}/src/medGui/commonWidgets
    ${PROJECT_SOURCE_DIR}/src/medGui/factories
    ${PROJECT_SOURCE_DIR}/src/medGui/LUT
    ${PROJECT_SOURCE_DIR}/src/medGui/settingsWidgets
    ${PROJECT_SOURCE_DIR}/src/medGui/toolboxes
    ${PROJECT_SOURCE_DIR}/src/medGui/viewContainers)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})


## ###################################################################
## INSTALL RULES
## ###################################################################
if( EXISTS ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)

install(FILES
  ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake
  DESTINATION
  cmake)

endif( EXISTS ${${PROJECT_NAME}_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
   AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
   AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)
