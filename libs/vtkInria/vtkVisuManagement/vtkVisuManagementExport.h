#ifndef VTKVISUMANAGEMENTEXPORT_H
#define VTKVISUMANAGEMENTEXPORT_H

#ifdef WIN32
 #if defined (vtkWidgetsAddOn_EXPORTS)
  #define VTK_VISUMANAGEMENT_EXPORT __declspec( dllexport )
 #else
  #define VTK_VISUMANAGEMENT_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_VISUMANAGEMENT_EXPORT
#endif

#endif // VTKVISUMANAGEMENTEXPORT_H
 

