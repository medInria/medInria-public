#pragma once

#ifdef WIN32
 #if defined (vtkImageView_EXPORTS)
  #define VTK_IMAGEVIEW_EXPORT __declspec( dllexport )
 #else
  #define VTK_IMAGEVIEW_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_IMAGEVIEW_EXPORT
#endif


 

