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

SET(${PROJECT_NAME}_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(${PROJECT_NAME}_INCLUDE_DIRS_BUILD_TREE
  ${${PROJECT_NAME}_BINARY_DIR}
  ${${PROJECT_NAME}_BINARY_DIR}/Shaders/
)


# These directories are always needed.
SET(${PROJECT_NAME}_INCLUDE_DIRS_SOURCE_TREE
  ${${PROJECT_NAME}_SOURCE_DIR}
  ${${PROJECT_NAME}_SOURCE_DIR}/HWShading/
  ${${PROJECT_NAME}_SOURCE_DIR}/HWShading/Shaders/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkVisuManagement/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkWidgetsAddOn/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkDataManagement/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkItk/
  ${${PROJECT_NAME}_SOURCE_DIR}/itkAddOn/
  ${${PROJECT_NAME}_SOURCE_DIR}/itkAddOn/Registration/
  ${${PROJECT_NAME}_SOURCE_DIR}/itkAddOn/IO/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkRenderingAddOn/
  ${${PROJECT_NAME}_SOURCE_DIR}/vtkImageView/
)

IF(Boost_FOUND)
  MESSAGE("Boost Include DIRS " ${Boost_INCLUDE_DIRS})
  SET(${PROJECT_NAME}_INCLUDE_DIRS_SOURCE_TREE
    ${${PROJECT_NAME}_INCLUDE_DIRS_SOURCE_TREE}
    ${Boost_INCLUDE_DIRS}
  )
ENDIF( Boost_FOUND )
