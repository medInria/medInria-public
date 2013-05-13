#######################################################################
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
#######################################################################

include_directories("..")


add_executable(itkGenerateThumbnails
  itkGenerateThumbnails.cxx)
  
target_link_libraries(itkGenerateThumbnails
  ${ITK_LIBRARIES})
