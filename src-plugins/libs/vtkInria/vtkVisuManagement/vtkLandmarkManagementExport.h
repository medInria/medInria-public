#pragma once

#ifdef WIN32
 #if defined (vtkLandmarkManagement_EXPORTS)
  #define VTK_LANDMARKMANAGEMENT_EXPORT __declspec( dllexport )
 #else
  #define VTK_LANDMARKMANAGEMENT_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_LANDMARKMANAGEMENT_EXPORT
#endif


 
