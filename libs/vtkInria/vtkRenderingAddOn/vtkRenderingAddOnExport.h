#ifndef VTKRENDERINGADDONEXPORT_H
#define VTKRENDERINGADDONEXPORT_H

#ifdef WIN32
 #if defined (vtkRenderingAddOn_EXPORTS)
  #define VTK_RENDERINGADDON_EXPORT __declspec( dllexport )
 #else
  #define VTK_RENDERINGADDON_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_RENDERINGADDON_EXPORT
#endif

#endif // VTKVISUMANAGEMENTEXPORT_H
 

