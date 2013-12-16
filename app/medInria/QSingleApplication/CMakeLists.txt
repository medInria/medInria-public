################################################################################
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
################################################################################

## #############################################################################
## List Sources
## #############################################################################

set(${PROJECT_NAME}_H
  QSingleApplication/qtlocalpeer.h
  QSingleApplication/qtsingleapplication.h
  QSingleApplication/qtsinglecoreapplication.h
  ${${PROJECT_NAME}_H}
  PARENT_SCOPE
  )
  
set(${PROJECT_NAME}_SOURCES
  QSingleApplication/qtlocalpeer.cpp
  QSingleApplication/qtsingleapplication.cpp
  QSingleApplication/qtsinglecoreapplication.cpp
  ${${PROJECT_NAME}_H}
  ${${PROJECT_NAME}_SOURCES}
  PARENT_SCOPE
  )

## #############################################################################
## OS specificities
## #############################################################################

set_source_files_properties(QSingleApplication/qtlocalpeer.cpp 
  DEPENDS QSingleApplication/qtlockedfile.cpp
  )

if (WIN)
  set_source_files_properties(QSingleApplication/qtlocalpeer.cpp 
    DEPENDS QSingleApplication/qtlockedfile_win.cpp
    )
else()
  set_source_files_properties(QSingleApplication/qtlocalpeer.cpp 
    DEPENDS QSingleApplication/qtlockedfile_unix.cpp
    )
endif()
