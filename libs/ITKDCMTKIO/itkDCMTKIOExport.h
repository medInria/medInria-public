#pragma once

#if defined(WIN32)
    #ifdef ITKDCMTKIO_EXPORTS
        #define ITKDCMTKIO_EXPORT __declspec(dllexport) 
    #else
        #define ITKDCMTKIO_EXPORT __declspec(dllimport) 
    #endif
#else
    #define ITKDCMTKIO_EXPORT 
#endif


