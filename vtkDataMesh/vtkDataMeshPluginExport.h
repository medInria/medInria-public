
#ifndef VTKDATAMESHPLUGINEXPORT_H
#define VTKDATAMESHPLUGINEXPORT_H

#ifdef WIN32
    #ifdef vtkDataMeshPlugin_EXPORTS
        #define VTKDATAMESHPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define VTKDATAMESHPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define VTKDATAMESHPLUGIN_EXPORT
#endif

#endif

