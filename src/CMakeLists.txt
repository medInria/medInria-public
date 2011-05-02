###############################################################################
#
# Libraries used by many plugins.
#
###############################################################################


if (MEDINRIA_BUILD_REGISTRATION_PLUGINS)
    add_subdirectory(itkProcessRegistration)
endif()

if (MEDINRIA_BUILD_ITKDCMTKDATAIMAGEREADER_PLUGIN)
    add_subdirectory(ITKDCMTKIO)
endif()

if ( ${MEDINRIA_REQUIRES_VTK} )
    add_subdirectory(dtkItkLogForwarder)
endif()

if ( ${MEDINRIA_REQUIRES_VTK} )
    add_subdirectory(dtkVtkLogForwarder)
endif()

