#ifndef VTKWIDGETSADDONEXPORT_H
#define VTKWIDGETSADDONEXPORT_H

#ifdef WIN32
 #if defined (vtkWidgetsAddOn_EXPORTS)
  #define VTK_WIDGETSADDON_EXPORT __declspec( dllexport )
 #else
  #define VTK_WIDGETSADDON_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_WIDGETSADDON_EXPORT
#endif

#endif // VTKWIDGETSADDONEXPORT_H
 

