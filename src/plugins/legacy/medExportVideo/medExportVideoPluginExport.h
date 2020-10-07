#pragma once

#ifdef WIN32
    #ifdef ExportVideoPlugin_EXPORTS
        #define EXPORTVIDEOPLUGIN_EXPORT __declspec(dllexport)
    #else
        #define EXPORTVIDEOPLUGIN_EXPORT __declspec(dllimport)
    #endif
#else
    #define EXPORTVIDEOPLUGIN_EXPORT
#endif
