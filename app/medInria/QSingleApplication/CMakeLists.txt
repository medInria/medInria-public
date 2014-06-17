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

set(${PROJECT_NAME}_HEADERS
  QSingleApplication/qtlocalpeer.h
  QSingleApplication/qtsingleapplication.h
  QSingleApplication/qtsinglecoreapplication.h
  ${${PROJECT_NAME}_HEADERS}
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

set(${PROJECT_NAME}_CFILES
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_CFILES}
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
