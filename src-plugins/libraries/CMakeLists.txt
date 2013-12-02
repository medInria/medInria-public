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
## Embed medPch
## #############################################################################

file(GLOB MEDPLUGINSPCH_H
  medPluginsPch/*.h
  )
  
file(GLOB MEDPLUGINSPCH_CPP
  medPluginsPch/*.cpp
  )

  
## #############################################################################
## subdirs
## #############################################################################

add_subdirectory(dtkItkLogForwarder)
add_subdirectory(dtkVtkLogForwarder)

add_subdirectory(vtkInria)

# itkDcmtkIO need vtkInria. (it is not obvious?)
add_subdirectory(itkDcmtkIO)
add_subdirectory(itkDataImageBase)

add_subdirectory(registrationFactory)
add_subdirectory(itkProcessRegistration)







