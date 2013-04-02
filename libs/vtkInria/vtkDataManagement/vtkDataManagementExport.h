#pragma once

#ifdef WIN32
 #if defined (vtkDataManagement_EXPORTS)
  #define VTK_DATAMANAGEMENT_EXPORT __declspec( dllexport )
 #else
  #define VTK_DATAMANAGEMENT_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_DATAMANAGEMENT_EXPORT
#endif


 

