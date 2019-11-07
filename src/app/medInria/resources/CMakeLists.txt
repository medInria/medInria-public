################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

file(GLOB ${TARGET_NAME}_QRC
  *.qrc
  )
set(${TARGET_NAME}_QRC
  ${${TARGET_NAME}_QRC}
  PARENT_SCOPE
  )

file(GLOB ${TARGET_NAME}_QSS
  *.qss
  )
set(${TARGET_NAME}_QSS
  ${${TARGET_NAME}_QSS}
  PARENT_SCOPE
  )

file(GLOB ${TARGET_NAME}_HTML
  *.html
  )
set(${TARGET_NAME}_HTML
  ${${TARGET_NAME}_HTML}
  PARENT_SCOPE
  )


