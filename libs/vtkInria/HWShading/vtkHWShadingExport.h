#pragma once

#ifdef WIN32
 #if defined (vtkHWShading_EXPORTS)
  #define VTK_HWSHADING_EXPORT __declspec( dllexport )
 #else
  #define VTK_HWSHADING_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_HWSHADING_EXPORT
#endif


 

