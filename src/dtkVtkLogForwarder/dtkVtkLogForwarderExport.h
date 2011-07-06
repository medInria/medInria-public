#ifndef DTKVTKLOGFORWARDEREXPORT_H
#define DTKVTKLOGFORWARDEREXPORT_H

#ifdef WIN32
    #ifdef dtkVtkLogForwarder_EXPORTS
        #define DTKVTKLOGFORWARDER_EXPORT __declspec(dllexport) 
    #else
        #define DTKVTKLOGFORWARDER_EXPORT __declspec(dllimport) 
    #endif
#else
    #define DTKVTKLOGFORWARDER_EXPORT
#endif

#endif

