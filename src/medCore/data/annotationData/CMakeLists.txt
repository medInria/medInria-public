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

file(GLOB HEADERS
  *.h
  )
set(${PROJECT_NAME}_H
  ${HEADERS}
  ${${PROJECT_NAME}_H}
  PARENT_SCOPE
  )

file(GLOB CPP
  *.cpp
  )
set(${PROJECT_NAME}_CPP
  ${CPP}
  ${${PROJECT_NAME}_CPP}
  PARENT_SCOPE
  )
