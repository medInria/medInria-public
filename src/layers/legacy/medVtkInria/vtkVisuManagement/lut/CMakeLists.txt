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

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  PARENT_SCOPE
  )
