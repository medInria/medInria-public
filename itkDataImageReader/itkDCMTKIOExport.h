#ifndef ITKDCMTKIOEXPORT_H
#define ITKDCMTKIOEXPORT_H

#ifdef WIN32
    #ifdef ITKDCMTKIO_EXPORTS
        #define ITKDCMTKIO_EXPORT __declspec(dllexport) 
    #else
        #define ITKDCMTKIO_EXPORT __declspec(dllimport) 
    #endif
#else
    #define ITKDCMTKIO_EXPORT 
#endif

#endif
