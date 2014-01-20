/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkMetaDataSet.h>
#include "medVtkInriaExport.h"

class vtkPolyData;


/**
   \class vtkMetaSurfaceMesh vtkMetaSurfaceMesh.h "vtkMetaSurfaceMesh.h"
   \brief Concrete implementation of vtkMetaDataSet for surface mesh handling
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a vtkDataSet.
   You can use it as a tool for handling surface mesh, modify it, Add some scalars to it, etc.
   Use the Read function to read vtkPolyData format files, as well as .mesh files.
   Use the Write function to write it into a vtkPolyData format files.
   And you can associate to the metamesh a specific vtkProperty
   
   
   \see
   vtkMetaImageData vtkMetaDataSet vtkMetaVolumeMesh
   vtkMetaDataSetSequence vtkDataManager
*/



class MEDVTKINRIA_EXPORT vtkMetaSurfaceMesh: public vtkMetaDataSet
{
 public:

  static vtkMetaSurfaceMesh* New();
  vtkTypeRevisionMacro(vtkMetaSurfaceMesh,vtkMetaDataSet);

  //BTX
  enum
  {
    FILE_IS_VTK = 1,
    FILE_IS_VTP,
    FILE_IS_MESH,
    FILE_IS_OBJ,
    LAST_FILE_ID
  };
  //ETX
  
  /**
     Overwridden methods for read and write surface meshes
      can only read and write vtkPolyData format files
  */
  virtual void Read (const char* filename);  
  virtual void Write (const char* filename);

  /**
     Get method to get the vtkDataSet as a vtkPolyData
  */
  vtkPolyData* GetPolyData() const;

  /**
     Static methods for I/O
  */
  static bool         IsVtkExtension (const char* ext);
  static bool         IsVtpExtension (const char* ext);
  static bool         IsMeshExtension (const char* ext);
  static bool         IsOBJExtension (const char* ext);
  static unsigned int CanReadFile (const char* filename);

  /**
     Get the type of the metadataset as string
  */
  virtual const char* GetDataSetType() const
  {
    return "SurfaceMesh";
  }

  virtual void CreateWirePolyData();

 protected:
  vtkMetaSurfaceMesh();
  ~vtkMetaSurfaceMesh();

  virtual void ReadVtkFile(const char* filename);
  virtual void ReadVtpFile(const char* filename);
  virtual void ReadMeshFile(const char* filename);
  virtual void ReadOBJFile(const char* filename);
  virtual void WriteOBJFile(const char* filename);
  virtual void WriteVtkFile (const char* filename);
  virtual void WriteVtpFile (const char* filename);

  /**
     Method called everytime the dataset changes for initialization
  */
  virtual void Initialize();
  
 private:
  
  vtkMetaSurfaceMesh(const vtkMetaSurfaceMesh&);       // Not implemented.
  void operator=(const vtkMetaSurfaceMesh&);        // Not implemented.

};



