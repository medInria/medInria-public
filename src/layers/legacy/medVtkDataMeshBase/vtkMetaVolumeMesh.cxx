/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaVolumeMesh.h>
#include "vtkObjectFactory.h"

#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtksys/SystemTools.hxx>

#include <vtkProperty.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>

#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkUnsignedShortArray.h>

#include <vtkErrorCode.h>




//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaVolumeMesh );


//----------------------------------------------------------------------------
vtkMetaVolumeMesh::vtkMetaVolumeMesh()
{
  this->Type = vtkMetaDataSet::VTK_META_VOLUME_MESH;
}

vtkMetaVolumeMesh::vtkMetaVolumeMesh(const vtkMetaVolumeMesh& other)
  : vtkMetaDataSet(other)
{
}

//----------------------------------------------------------------------------
vtkMetaVolumeMesh::~vtkMetaVolumeMesh()
{

}

vtkMetaVolumeMesh* vtkMetaVolumeMesh::Clone()
{
  return new vtkMetaVolumeMesh(*this);
}

//----------------------------------------------------------------------------
void vtkMetaVolumeMesh::Initialize()
{
  this->Superclass::Initialize();
  

  if (!this->DataSet)
    return;
  
  vtkProperty* property = vtkProperty::SafeDownCast(this->GetProperty());
  if (!property)
  {
    property = vtkProperty::New();
    this->SetProperty (property);
    property->Delete();
  }  
  
}

//----------------------------------------------------------------------------
vtkUnstructuredGrid* vtkMetaVolumeMesh::GetUnstructuredGrid() const
{
  if (!this->DataSet)
    return nullptr;
  return vtkUnstructuredGrid::SafeDownCast (this->DataSet);
}

//----------------------------------------------------------------------------
void vtkMetaVolumeMesh::ReadVtkFile (const char* filename)
{

  vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
  reader->SetFileName (filename);
  
  try
  {
    reader->Update();
    this->SetDataSet (reader->GetOutput());
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    reader->Delete();
    throw error;
  }
  reader->Delete();
}


//----------------------------------------------------------------------------
void vtkMetaVolumeMesh::Read (const char* filename)
{
  
  unsigned long format = vtkMetaVolumeMesh::CanReadFile (filename);

  try
  {
    
    std::cout << "Reading " << filename << "... ";
    switch (format)
    {
	case vtkMetaVolumeMesh::FILE_IS_VTK :
	  this->ReadVtkFile (filename);
	  break;
	case vtkMetaVolumeMesh::FILE_IS_MESH :
	  this->ReadMeshFile (filename);
	  break; 
	case vtkMetaVolumeMesh::FILE_IS_GMESH :
	  this->ReadGMeshFile (filename);
	  break; 
	default :
	  vtkErrorMacro(<<"unknown dataset type : "<<filename<<endl);
	  throw vtkErrorCode::UnrecognizedFileTypeError;
    }
    std::cout << "done." << std::endl;
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }
  
  this->SetFilePath (filename);  
}



//----------------------------------------------------------------------------
void vtkMetaVolumeMesh::WriteVtkFile (const char* filename)
{
  

  if (!this->DataSet)
  {
    vtkErrorMacro(<<"No DataSet to write"<<endl);
    throw vtkErrorCode::UserError;
  }
  vtkUnstructuredGrid* c_mesh = vtkUnstructuredGrid::SafeDownCast (this->DataSet);
  if (!c_mesh)
  {
    vtkErrorMacro(<<"DataSet is not a polydata object"<<endl);
    throw vtkErrorCode::UserError;
  }
  vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New();
  writer->SetFileName (filename);
  
  try
  {
    writer->SetInputData (c_mesh);
    writer->Write();
    writer->Delete();
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    writer->Delete();
    throw error;
  }
  
  this->SetFilePath (filename);  

}


//----------------------------------------------------------------------------
void vtkMetaVolumeMesh::Write (const char* filename)
{
  try
  {
    std::cout << "writing " << filename << "... ";
    this->WriteVtkFile (filename);
    std::cout << "done." << std::endl;
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }
}



//----------------------------------------------------------------------------
bool vtkMetaVolumeMesh::IsVtkExtension (const char* ext)
{
  if (strcmp (ext, ".vtk") == 0 ||
      strcmp (ext, ".vtu") == 0)
    return true;
  return false;
}
//----------------------------------------------------------------------------
bool vtkMetaVolumeMesh::IsMeshExtension (const char* ext)
{
  if (strcmp (ext, ".mesh") == 0)
    return true;
  return false;
}
//----------------------------------------------------------------------------
bool vtkMetaVolumeMesh::IsGMeshExtension (const char* ext)
{
  if (strcmp (ext, ".msh") == 0)
    return true;
  return false;
}

//----------------------------------------------------------------------------
unsigned int vtkMetaVolumeMesh::CanReadFile (const char* filename)
{

  if (vtkMetaVolumeMesh::IsMeshExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
  {
    // check if there is any tetrahedron...
    
    std::ifstream file (filename );
    char str[256];
    file >> str;
    
    if(file.fail())
    {
      return 0;
    }
    
  
    while( (!file.fail()) && (strcmp (str, "Tetrahedra") != 0) && (strcmp (str, "End") != 0) && (strcmp (str, "END") != 0) )
    {
      file >> str;
    }
    
    if (strcmp (str, "Tetrahedra") == 0)
      return vtkMetaVolumeMesh::FILE_IS_MESH;
    else
      return 0;
  }


  if (vtkMetaVolumeMesh::IsGMeshExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
  {
    // check if there is any tetrahedron...
    
    std::ifstream file (filename );
    char str[256];
    file >> str;
    
    if(file.fail())
    {
      return 0;
    }
    
  
    while( (!file.fail()) && (strcmp (str, "$ELM") != 0) && (strcmp (str, "$ENDELM") != 0) )
    {
      file >> str;
    }
    
    if (strcmp (str, "$ELM") == 0)
      return vtkMetaVolumeMesh::FILE_IS_GMESH;
    else
      return 0;
  }
  

  if (!vtkMetaVolumeMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    return 0;
  
  vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
  reader->SetFileName (filename);
  if (reader->IsFileUnstructuredGrid ())
  {
    reader->Delete();
    return vtkMetaVolumeMesh::FILE_IS_VTK;
  }
  reader->Delete();
  return 0;
}




void vtkMetaVolumeMesh::ReadMeshFile (const char* filename)
{

  std::ifstream file (filename );
  char str[256];

  if(file.fail())
  {
    vtkErrorMacro("File not found\n");
    throw vtkErrorCode::FileNotFoundError;
  }


  vtkPoints* points = vtkPoints::New();
  vtkUnsignedShortArray* pointarray = vtkUnsignedShortArray::New();
  vtkUnsignedShortArray* cellarray  = vtkUnsignedShortArray::New();
  vtkUnstructuredGrid* outputmesh = vtkUnstructuredGrid::New();
  
  unsigned short ref = 0;
  
  file >> str;
  while((strcmp (str, "Vertices") != 0) && (strcmp (str, "End") != 0) && (strcmp (str, "END") != 0) )
  {
    if (file.fail())
    {
      points->Delete();
      pointarray->Delete();
      cellarray->Delete();
      outputmesh->Delete();
      vtkErrorMacro("No point in file\n");
      throw vtkErrorCode::CannotOpenFileError;
    }
    file >> str;
  }

  if((strcmp (str, "End") == 0) || (strcmp (str, "END") == 0))
  {
    vtkErrorMacro(<<"Unexpected end of file"<<endl);
    points->Delete();
    pointarray->Delete();
    cellarray->Delete();
    outputmesh->Delete();
    throw vtkErrorCode::PrematureEndOfFileError;
  }
  
  unsigned int NVertices = 0;
  file >>  NVertices;
  points->SetNumberOfPoints (NVertices);

  pointarray->SetName ("Point array");
  pointarray->Allocate(NVertices);
  
  // read vertex position 
  for(unsigned int i=0; i<NVertices; i++)
  {
    double pos[3];
    file >> pos[0] >> pos[1] >> pos[2] >> ref;
    points->SetPoint (i, pos[0], pos[1], pos[2]);
    pointarray->InsertNextValue(ref);
  }

  outputmesh->SetPoints (points);

  if (outputmesh->GetPointData())
  {
    outputmesh->GetPointData()->AddArray (pointarray);
  }
  
  file >> str;
  
  while( (strcmp (str, "Tetrahedra") != 0) && (strcmp (str, "End") != 0) && (strcmp (str, "END") != 0) )
  {
    if (file.fail())
    {
      points->Delete();
      pointarray->Delete();
      cellarray->Delete();
      outputmesh->Delete();
      vtkErrorMacro("No tetrahedron in file\n");
      throw vtkErrorCode::CannotOpenFileError;
    }

    file >> str;
  }

  if((strcmp (str, "End") == 0) || (strcmp (str, "END") == 0) )
  {
    vtkErrorMacro(<<"Unexpected end of file"<<endl);
    points->Delete();
    pointarray->Delete();
    cellarray->Delete();
    outputmesh->Delete();
    
    throw vtkErrorCode::PrematureEndOfFileError;
  }
  
    
  unsigned int NTetrahedra;
  
  file >>  NTetrahedra;
  outputmesh->Allocate (NTetrahedra);
  cellarray->SetName ("Zones");
  cellarray->Allocate(NTetrahedra);
  for(unsigned int i=0; i<NTetrahedra; i++)
  {
    unsigned int ids[4];
    file >> ids[0] >> ids[1] >> ids[2] >> ids[3] >> ref;
    vtkIdList* idlist = vtkIdList::New();
    idlist->InsertNextId (ids[0]-1);
    idlist->InsertNextId (ids[1]-1);
    idlist->InsertNextId (ids[2]-1);
    idlist->InsertNextId (ids[3]-1);
    
    outputmesh->InsertNextCell (VTK_TETRA, idlist);
    idlist->Delete();
    cellarray->InsertNextValue(ref);
  }

  if (outputmesh->GetCellData())
  {
    outputmesh->GetCellData()->AddArray (cellarray);
  }
  
  this->SetDataSet (outputmesh);

  points->Delete();
  pointarray->Delete();
  cellarray->Delete();
  outputmesh->Delete();

  
    
}



void vtkMetaVolumeMesh::ReadGMeshFile (const char* filename)
{

  std::ifstream file (filename );
  char str[256];

  if(file.fail())
  {
    vtkErrorMacro("File not found\n");
    throw vtkErrorCode::FileNotFoundError;
  }


  vtkPoints* points = vtkPoints::New();
  vtkUnsignedShortArray* pointarray = vtkUnsignedShortArray::New();
  vtkUnsignedShortArray* cellarray  = vtkUnsignedShortArray::New();
  vtkUnstructuredGrid* outputmesh = vtkUnstructuredGrid::New();
  
  unsigned short ref = 0;
  
  file >> str;
  while( (strcmp (str, "$NOD") != 0) )
  {
    if (file.fail())
    {
      points->Delete();
      pointarray->Delete();
      cellarray->Delete();
      outputmesh->Delete();
      vtkErrorMacro("No point in file\n");
      throw vtkErrorCode::CannotOpenFileError;
    }
    file >> str;
  }

  unsigned int NVertices = 0;
  file >>  NVertices;
  points->SetNumberOfPoints( NVertices );

  pointarray->SetName ("Point array");
  pointarray->Allocate(NVertices);

  // read vertex position 
  for( unsigned int i = 0; i < NVertices; i++ )
  {
    double pos[3];
    file >> ref >> pos[0] >> pos[1] >> pos[2];
    points->SetPoint(i, pos[0], pos[1], pos[2]);
    pointarray->InsertNextValue( ref - 1 );
  }
  outputmesh->SetPoints (points);

//   if (outputmesh->GetPointData())
//   {
//     outputmesh->GetPointData()->AddArray (pointarray);
//   }


  file >> str;
  while( (strcmp (str, "$ELM") != 0) )
  {
    if (file.fail())
    {
      points->Delete();
      pointarray->Delete();
      cellarray->Delete();
      outputmesh->Delete();
      vtkErrorMacro("No tetrahedron in file\n");
      throw vtkErrorCode::CannotOpenFileError;
    }
    file >> str;
  }

  unsigned int NTetrahedra;
  file >>  NTetrahedra;

  outputmesh->Allocate (NTetrahedra);
  cellarray->SetName ("Zones");
  cellarray->Allocate(NTetrahedra);
  for( unsigned int i = 0; i < NTetrahedra; i++ )
  {
    unsigned int ids[4];
    unsigned int b1, b2, b3, b4;
    file >> ref >> b1 >> b2 >> b3 >> b4 >> ids[0] >> ids[1] >> ids[2] >> ids[3];
    vtkIdList* idlist = vtkIdList::New();
    idlist->InsertNextId(ids[0] - 1);
    idlist->InsertNextId(ids[1] - 1);
    idlist->InsertNextId(ids[2] - 1);
    idlist->InsertNextId(ids[3] - 1);
    
    outputmesh->InsertNextCell (VTK_TETRA, idlist);
    idlist->Delete();
    cellarray->InsertNextValue(ref);
  }

//   if (outputmesh->GetCellData())
//   {
//     outputmesh->GetCellData()->AddArray (cellarray);
//   }
  
  this->SetDataSet (outputmesh);

  points->Delete();
  pointarray->Delete();
  cellarray->Delete();
  outputmesh->Delete();
}

