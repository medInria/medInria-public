#ifndef VTKHELPERSEXPORT_H
#define VTKHELPERSEXPORT_H

#ifdef WIN32
 #if defined (vtkHelpers_EXPORTS)
  #define VTK_HELPERS_EXPORT __declspec( dllexport )
 #else
  #define VTK_HELPERS_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_HELPERS_EXPORT
#endif

#endif // VTKHELPERSEXPORT_H
 

