#pragma once

#ifdef WIN32
 #if defined (vtkWidgetsAddOn_EXPORTS)
  #define VTK_WIDGETSADDON_EXPORT __declspec( dllexport )
 #else
  #define VTK_WIDGETSADDON_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_WIDGETSADDON_EXPORT
#endif


 

