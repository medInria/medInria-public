/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkMetaVolumeMesh.h 836 2008-05-20 08:53:15Z tmansi $
Language:  C++
Author:    $Author: tmansi $
Date:      $Date: 2008-05-20 10:53:15 +0200 (Tue, 20 May 2008) $
Version:   $Revision: 836 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkMetaVolumeMesh_h_
#define _vtkMetaVolumeMesh_h_
#include "vtkDataManagementExport.h"
#include <vtkMetaDataSet.h>

class vtkUnstructuredGrid;


/**
   \class vtkMetaVolumeMesh vtkMetaVolumeMesh.h "vtkMetaVolumeMesh.h"
   \brief Concrete implementation of vtkMetaDataSet for volumic mesh handling
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a vtkDataSet.
   Specific case of a volumic mesh, hendles, read and writes vtkUntructuredGrid object
   
   \see
   vtkMetaImageData vtkMetaSurfaceMesh vtkMetaDataSet
   vtkMetaDataSetSequence vtkDataManager
*/


class VTK_DATAMANAGEMENT_EXPORT vtkMetaVolumeMesh: public vtkMetaDataSet
{
 public:

  static vtkMetaVolumeMesh* New();
  vtkTypeRevisionMacro(vtkMetaVolumeMesh,vtkMetaDataSet);

  //BTX
  enum
  {
    FILE_IS_VTK = 1,
    FILE_IS_MESH,
    FILE_IS_OBJ,
    FILE_IS_GMESH,
    LAST_FILE_ID
  };
  //ETX

  virtual void Read (const char* filename);
  
  virtual void Write (const char* filename);
  
  vtkUnstructuredGrid* GetUnstructuredGrid (void) const;

  
  static bool         IsVtkExtension (const char* ext);
  static bool         IsMeshExtension (const char* ext);
  static bool         IsGMeshExtension (const char* ext);
  static unsigned int CanReadFile (const char* filename);

  virtual const char* GetDataSetType (void) const
  {
    return "VolumeMesh";
  }
  
 protected:
  vtkMetaVolumeMesh();
  ~vtkMetaVolumeMesh();

  virtual void Initialize (void);
  virtual void ReadVtkFile(const char* filename);
  virtual void ReadMeshFile(const char* filename);
  virtual void ReadGMeshFile(const char* filename);
  virtual void WriteVtkFile (const char* filename);


 private:
  
  vtkMetaVolumeMesh(const vtkMetaVolumeMesh&);      // Not implemented.
  void operator=(const vtkMetaVolumeMesh&);        // Not implemented.

};

#endif

