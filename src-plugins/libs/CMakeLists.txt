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

if (MEDINRIA_REQUIERS_ITKPROCESSREGISTRATION)
    add_subdirectory(itkProcessRegistration)
    add_subdirectory(registrationFactory)
endif()

if (MEDINRIA_REQUIERS_ITKDATAIMAGEBASE)
    add_subdirectory(itkDataImageBase)
endif()

if (MEDINRIA_REQUIERS_ITKDCMTKIO)
    add_subdirectory(ITKDCMTKIO)
endif()

if ( MEDINRIA_REQUIRES_ITK )
    add_subdirectory(dtkItkLogForwarder)
endif()

if ( MEDINRIA_REQUIRES_VTK )
    add_subdirectory(dtkVtkLogForwarder)
endif()

if ( MEDINRIA_REQUIRES_VTKINRIA )
    add_subdirectory(vtkInria)
endif()

