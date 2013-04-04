#pragma once

#ifdef WIN32
    #ifdef vtkDataMeshWriterPlugin_EXPORTS
        #define VTKDATAMESHWRITERPLUGIN_EXPORT __declspec(dllexport) 
    #else
        #define VTKDATAMESHWRITERPLUGIN_EXPORT __declspec(dllimport) 
    #endif
#else
    #define VTKDATAMESHWRITERPLUGIN_EXPORT
#endif



