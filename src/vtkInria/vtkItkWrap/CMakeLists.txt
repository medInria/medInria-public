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

project(vtkItkWrap)


## #############################################################################
## List Sources
## #############################################################################

file(GLOB ${PROJECT_NAME}_H
  *.h
  )
  
file(GLOB ${PROJECT_NAME}_TXX
  *.txx
  )


## #############################################################################
## add library
## #############################################################################

add_library(${PROJECT_NAME} SHARED #just to have it in the EDI
  ${${PROJECT_NAME}_TXX}
  ${${PROJECT_NAME}_H}
  )
  

