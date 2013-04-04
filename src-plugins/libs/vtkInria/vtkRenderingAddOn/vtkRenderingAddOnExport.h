#pragma once

#ifdef WIN32
 #if defined (vtkRenderingAddOn_EXPORTS)
  #define VTK_RENDERINGADDON_EXPORT __declspec( dllexport )
 #else
  #define VTK_RENDERINGADDON_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_RENDERINGADDON_EXPORT
#endif


 

