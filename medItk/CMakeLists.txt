### CMakeLists.txt ---
## 
## Author: Julien Wintz
## Copyright (C) 2008 - Julien Wintz, Inria.
## Created: Thu May 28 12:53:44 2009 (+0200)
## Version: $Id$
## Last-Updated: Thu May 28 13:12:56 2009 (+0200)
##           By: Julien Wintz
##     Update #: 3
######################################################################
## 
### Commentary: 
## 
######################################################################
## 
### Change log:
## 
######################################################################

project(medItk)

set(${PROJECT_NAME}_HEADERS
)

set(${PROJECT_NAME}_SOURCES
medItk.cpp
)

## #################################################################
## MSVC config
## #################################################################

source_group("Generated\\Metaobject Files" REGULAR_EXPRESSION "moc_.*c[xp]*$" )
source_group("Generated\\Python Modules" REGULAR_EXPRESSION ".*py")
source_group("Generated\\CMake Rules" REGULAR_EXPRESSION ".*rule")
source_group("Header Files" FILES ${${PROJECT_NAME}_HEADERS})

## #################################################################
## Build rules
## #################################################################

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
else(ITK_FOUND)
  message(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
endif(ITK_FOUND)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES} 
  ${${PROJECT_NAME}_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HEADERS}")

target_link_libraries(${PROJECT_NAME}
  ITKBasicFilters
)
