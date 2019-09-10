/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medVtkDataMeshBaseExport.h"

#include <vtkMetaDataSet.h>

class vtkUnstructuredGrid;


/**
   \class vtkMetaVolumeMesh vtkMetaVolumeMesh.h "vtkMetaVolumeMesh.h"
   \brief Concrete implementation of vtkMetaDataSet for volumic mesh handling
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a vtkDataSet.
   Specific case of a volumic mesh, hendles, read and writes vtkUntructuredGrid object
   
   \see
   vtkMetaSurfaceMesh vtkMetaDataSet
   vtkMetaDataSetSequence vtkDataManager
*/


class MEDVTKDATAMESHBASE_EXPORT vtkMetaVolumeMesh: public vtkMetaDataSet
{
 public:

  static vtkMetaVolumeMesh* New();
  vtkTypeMacro(vtkMetaVolumeMesh,vtkMetaDataSet);

  virtual vtkMetaVolumeMesh* Clone() override;

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
  
  vtkUnstructuredGrid* GetUnstructuredGrid() const;

  
  static bool         IsVtkExtension (const char* ext);
  static bool         IsMeshExtension (const char* ext);
  static bool         IsGMeshExtension (const char* ext);
  static unsigned int CanReadFile (const char* filename);

  virtual const char* GetDataSetType() const
  {
    return "VolumeMesh";
  }
  
protected:
  vtkMetaVolumeMesh();
  vtkMetaVolumeMesh(const vtkMetaVolumeMesh&);
  ~vtkMetaVolumeMesh();

  virtual void Initialize();
  virtual void ReadVtkFile(const char* filename);
  virtual void ReadMeshFile(const char* filename);
  virtual void ReadGMeshFile(const char* filename);
  virtual void WriteVtkFile (const char* filename);


private:
  void operator=(const vtkMetaVolumeMesh&);        // Not implemented.

};



