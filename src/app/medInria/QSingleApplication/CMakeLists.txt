################################################################################
#
# medInria

# Copyright (c) INRIA 2013. All rights reserved.

# See LICENSE.txt for details in the root of the sources or:
# https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
#
################################################################################

set(${TARGET_NAME}_HEADERS
  QSingleApplication/qtlocalpeer.h
  QSingleApplication/qtsingleapplication.h
  QSingleApplication/qtsinglecoreapplication.h
  ${${TARGET_NAME}_HEADERS}
  PARENT_SCOPE
  )

set(${TARGET_NAME}_SOURCES
  QSingleApplication/qtlocalpeer.cpp
  QSingleApplication/qtsingleapplication.cpp
  QSingleApplication/qtsinglecoreapplication.cpp
  ${${TARGET_NAME}_H}
  ${${TARGET_NAME}_SOURCES}
  PARENT_SCOPE
  )

set(${TARGET_NAME}_CFILES
  ${${TARGET_NAME}_SOURCES}
  ${${TARGET_NAME}_HEADERS}
  ${${TARGET_NAME}_CFILES}
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
