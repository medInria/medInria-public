#pragma once

#ifdef WIN32
    #ifdef dtkItkLogForwarder_EXPORTS
        #define DTKITKLOGFORWARDER_EXPORT __declspec(dllexport) 
    #else
        #define DTKITKLOGFORWARDER_EXPORT __declspec(dllimport) 
    #endif
#else
    #define DTKITKLOGFORWARDER_EXPORT
#endif



