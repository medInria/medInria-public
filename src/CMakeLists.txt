###############################################################################
#
# Libraries used by many plugins.
#
###############################################################################


if (MEDINRIA_ENABLE_REGISTRATION_PLUGINS OR MEDINRIA_BUILD_ALL_PLUGINS)
    add_subdirectory(itkProcessRegistration)
endif()

if (MEDINRIA_BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN)
    add_subdirectory(ITKDCMTKIO)
endif()

if ( MEDINRIA_REQUIRES_ITK )
    add_subdirectory(dtkItkLogForwarder)
endif()

if ( MEDINRIA_REQUIRES_VTK )
    add_subdirectory(dtkVtkLogForwarder)
endif()

