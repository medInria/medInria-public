
#ifndef COMPOSITEDATATYPEPLUGINEXPORT_H
#define COMPOSITEDATATYPEPLUGINEXPORT_H

#ifdef WIN32
    #ifdef CompositeDataTypePlugin_EXPORTS
        #define COMPOSITEDATATYPEPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define COMPOSITEDATATYPEPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define COMPOSITEDATATYPEPLUGIN_EXPORT
#endif

#endif

