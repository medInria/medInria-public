#pragma once

#ifdef WIN32
 #if defined (vtkVisuManagement_EXPORTS)
  #define VTK_VISUMANAGEMENT_EXPORT __declspec( dllexport )
 #else
  #define VTK_VISUMANAGEMENT_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_VISUMANAGEMENT_EXPORT
#endif


 

