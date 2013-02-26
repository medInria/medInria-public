###############################################################################
#
# Libraries used by many plugins.
#
###############################################################################


if (MEDINRIA_REQUIERS_ITKPROCESSREGISTRATION)
    add_subdirectory(itkProcessRegistration)
    add_subdirectory(hRegistrationFactory)
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

