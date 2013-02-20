#ifndef VTKDATAMANAGEMENTEXPORT_H
#define VTKDATAMANAGEMENTEXPORT_H

#ifdef WIN32
 #if defined (vtkDataManagement_EXPORTS)
  #define VTK_DATAMANAGEMENT_EXPORT __declspec( dllexport )
 #else
  #define VTK_DATAMANAGEMENT_EXPORT __declspec( dllimport )
 #endif
#else
    #define VTK_DATAMANAGEMENT_EXPORT
#endif

#endif // VTKDATAMANAGEMENTEXPORT_H
 

