/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaSurfaceMesh.h>
#include "vtkObjectFactory.h"

#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
// #include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkUnsignedShortArray.h>
#include <vtkCellType.h>
#include <vtkLine.h>
#include <vtkExtractEdges.h>

#include <vtkCleanPolyData.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkOBJReader.h>
#include <vtkOBJExporter.h>

#include <vtksys/SystemTools.hxx>

#include <vtkCellType.h>

#include <vtkLookupTable.h>

#include <vtkErrorCode.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaSurfaceMesh );
vtkCxxRevisionMacro(vtkMetaSurfaceMesh, "$Revision: 788 $");

//----------------------------------------------------------------------------
vtkMetaSurfaceMesh::vtkMetaSurfaceMesh()
{
  this->Type = vtkMetaDataSet::VTK_META_SURFACE_MESH;
}

//----------------------------------------------------------------------------
vtkMetaSurfaceMesh::~vtkMetaSurfaceMesh()
{
}

//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::Initialize()
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
vtkPolyData* vtkMetaSurfaceMesh::GetPolyData() const
{
  if (!this->DataSet)
    return NULL;
  return vtkPolyData::SafeDownCast (this->DataSet);
}

//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::ReadVtkFile (const char* filename)
{
  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName (filename);
  
  try
  {
    reader->Update();
    this->SetDataSet (reader->GetOutput());

    //Needed to extract info (Patient name + ID) from CARTO files
    if (reader->GetHeader())
    {
        std::string headerStr(reader->GetHeader());
        std::istringstream header(headerStr);
        std::string info;
        std::vector<std::string> patientInfo;
        if (headerStr.find_first_not_of('"')) //Absence of "
        {
            while ( std::getline( header, info, ' ' ) )
                patientInfo.push_back(info);
        }
        else
        {
            std::string temp = "";
            while ( std::getline( header, info, ' ' ) )
            {
                if (info.at(0) == '"')
                {
                    info.erase(0,1);
                    if(info.at(info.size()-1) == '"') // "Word"
                    {
                        info.erase(info.size()-1, 1);
                        patientInfo.push_back(info);
                    }
                    else // "Word
                        temp += ' ' + info;
                }
                else if (info.at(info.size()-1) == '"') // Word"
                {
                    info.erase(info.size()-1, 1);
                    patientInfo.push_back(temp+ ' ' +info);
                    temp = "";
                }
                else
                {
                    if (temp.empty()) // "X" Word "Y"
                    {
                        patientInfo.push_back(info);
                    }
                    else // "X Word Y"
                        temp += ' ' +info;
                }
            }
        }
        if (patientInfo.size()>3)
        {
            std::string patientName, patientID;
            patientName = patientInfo[2] + " " + patientInfo[1];
            patientID = patientInfo[3];
            this->SetMetaData( "PatientName", patientName);
            this->SetMetaData( "PatientID", patientID);
        }
    }
    //if(reader->GetOutput()->GetPointData()->GetScalars())
    //    if(reader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable())
    //        this->SetLookupTable(reader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable());

  }
  catch (vtkErrorCode::ErrorIds error)
  {
    reader->Delete();
    throw error;
  }
  reader->Delete();
}

void vtkMetaSurfaceMesh::ReadVtpFile(const char* filename)
{
    vtkXMLPolyDataReader* reader = vtkXMLPolyDataReader::New();
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
void vtkMetaSurfaceMesh::ReadOBJFile (const char* filename)
{

  vtkOBJReader* reader = vtkOBJReader::New();
  reader->SetFileName (filename);
  
  try
  {
    reader->Update();
    vtkCleanPolyData* cleanFilter = vtkCleanPolyData::New();
    cleanFilter->SetInput(reader->GetOutput());
    cleanFilter->ConvertLinesToPointsOn();
    cleanFilter->ConvertPolysToLinesOn();
    cleanFilter->ConvertStripsToPolysOn();
    cleanFilter->PointMergingOn();
    cleanFilter->Update();
    vtkDataSetSurfaceFilter* surfaceFilter = vtkDataSetSurfaceFilter::New();
    surfaceFilter->SetInput(cleanFilter->GetOutput());
    surfaceFilter->Update();
    this->SetDataSet (surfaceFilter->GetOutput());
    cleanFilter->Delete();
    surfaceFilter->Delete();
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    reader->Delete();
    throw error;
  }
  reader->Delete();
}


//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::Read (const char* filename)
{

  try
  {
    
    std::cout << "Reading : " << filename <<"... ";
    switch (vtkMetaSurfaceMesh::CanReadFile (filename))
    {
	case vtkMetaSurfaceMesh::FILE_IS_VTK :
	  this->ReadVtkFile (filename);
	  break;
    case vtkMetaSurfaceMesh::FILE_IS_VTP :
      this->ReadVtpFile (filename);
      break;
	case vtkMetaSurfaceMesh::FILE_IS_MESH :
	  this->ReadMeshFile (filename);
	  break;
	case vtkMetaSurfaceMesh::FILE_IS_OBJ :
	  this->ReadOBJFile (filename);
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
void vtkMetaSurfaceMesh::WriteVtkFile (const char* filename)
{
  if (!this->DataSet)
  {
    vtkErrorMacro(<<"No DataSet to write"<<endl);
    throw vtkErrorCode::UserError;
  }

  vtkPolyData* c_mesh = vtkPolyData::SafeDownCast (this->DataSet);
  if (!c_mesh)
  {
    vtkErrorMacro(<<"DataSet is not a polydata object"<<endl);
    throw vtkErrorCode::UserError;
  }
  
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName (filename);
  
  try
  {
    writer->SetInput (c_mesh);
    writer->Write();
    writer->Delete();
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    writer->Delete();
    throw error;
  }
  
}


//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::WriteVtpFile (const char* filename)
{
    if (!this->DataSet)
    {
        vtkErrorMacro(<<"No DataSet to write"<<endl);
        throw vtkErrorCode::UserError;
    }
    
    vtkPolyData* c_mesh = vtkPolyData::SafeDownCast (this->DataSet);
    if (!c_mesh)
    {
        vtkErrorMacro(<<"DataSet is not a polydata object"<<endl);
        throw vtkErrorCode::UserError;
    }
    
    vtkXMLPolyDataWriter* writer = vtkXMLPolyDataWriter::New();
    writer->SetFileName (filename);
    
    try
    {
        writer->SetInput (c_mesh);
        writer->Write();
        writer->Delete();
    }
    catch (vtkErrorCode::ErrorIds error)
    {
        writer->Delete();
        throw error;
    }
}

//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::WriteOBJFile (const char* filename)
{
  vtkErrorMacro(<<"Not yet implemented"<<endl);
}


//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::Write (const char* filename)
{
  try
  {
    std::cout<<"writing "<<filename<<"... ";
      
      if (vtkMetaSurfaceMesh::IsVtpExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
          this->WriteVtpFile (filename);
      else
          this->WriteVtkFile (filename);
      
    std::cout<<"done."<<std::endl;
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }
  this->SetFilePath (filename);

}

bool vtkMetaSurfaceMesh::IsVtpExtension (const char* ext)
{
    if (strcmp (ext, ".vtp") == 0)
        return true;
    
    return false;
}

//----------------------------------------------------------------------------
bool vtkMetaSurfaceMesh::IsVtkExtension (const char* ext)
{
  if (strcmp (ext, ".fib") == 0 ||
      strcmp (ext, ".vtk") == 0)
    return true;
  return false;
}

//----------------------------------------------------------------------------
bool vtkMetaSurfaceMesh::IsMeshExtension (const char* ext)
{
  if (strcmp (ext, ".mesh") == 0)
    return true;
  return false;
}

//----------------------------------------------------------------------------
bool vtkMetaSurfaceMesh::IsOBJExtension (const char* ext)
{
  if (strcmp (ext, ".obj") == 0)
    return true;
  return false;
}


//----------------------------------------------------------------------------
unsigned int vtkMetaSurfaceMesh::CanReadFile (const char* filename)
{

  if (vtkMetaSurfaceMesh::IsMeshExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    return vtkMetaSurfaceMesh::FILE_IS_MESH;

  if (vtkMetaSurfaceMesh::IsOBJExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
  {
    vtkOBJReader* reader = vtkOBJReader::New();
    reader->SetFileName (filename);
    try
    {
      reader->Update();
    }
    catch(vtkErrorCode::ErrorIds)
    {
      reader->Delete();
      return 0;
    }
    reader->Delete();
    return vtkMetaSurfaceMesh::FILE_IS_OBJ;
  }
  
    if (vtkMetaSurfaceMesh::IsVtpExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    {
        vtkXMLPolyDataReader* reader = vtkXMLPolyDataReader::New();
        reader->SetFileName (filename);
        try
        {
            reader->Update();
        }
        catch  (vtkErrorCode::ErrorIds)
        {
            reader->Delete();
            return 0;
        }
        reader->Delete();
        return vtkMetaSurfaceMesh::FILE_IS_VTP;
    }
    
  if (!vtkMetaSurfaceMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    return 0;
  
  try
  {
    vtkPolyDataReader* reader = vtkPolyDataReader::New();
    reader->SetFileName (filename);
    if (reader->IsFilePolyData ())
    {
      reader->Delete();
      return vtkMetaSurfaceMesh::FILE_IS_VTK;
    }
    reader->Delete();
  }
  catch (vtkErrorCode::ErrorIds)
  {
  }

  return 0;
}


void vtkMetaSurfaceMesh::ReadMeshFile (const char* filename)
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
  vtkPolyData* outputmesh = vtkPolyData::New();
  
  unsigned short ref = 0;
  
  file >> str;
  while( (strcmp (str, "Vertices") != 0) && (strcmp (str, "End") != 0) && (strcmp (str, "END") != 0) )
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

  if((strcmp (str, "End") == 0) || (strcmp (str, "END") == 0) )
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
  while( (strcmp (str, "Triangles") != 0) && (strcmp (str, "End") != 0) && (strcmp (str, "END") != 0) )
  {
    if (file.fail())
    {
      points->Delete();
      pointarray->Delete();
      cellarray->Delete();
      outputmesh->Delete();
      vtkErrorMacro("No triangle in file\n");
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
  
    
  unsigned int NTriangles;
  
  file >>  NTriangles;
  outputmesh->Allocate (NTriangles);
  cellarray->SetName ("Zones");
  cellarray->Allocate(NTriangles);
  for(unsigned int i=0; i<NTriangles; i++)
  {
    unsigned int ids[3];
    file >> ids[0] >> ids[1] >> ids[2] >> ref;
    vtkIdList* idlist = vtkIdList::New();
    idlist->InsertNextId (ids[0]-1);
    idlist->InsertNextId (ids[1]-1);
    idlist->InsertNextId (ids[2]-1);
    
    outputmesh->InsertNextCell (VTK_TRIANGLE, idlist);
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

// void vtkMetaSurfaceMesh::CreateWirePolyDataOld()
// {
//   if (this->GetWirePolyData())
//     return;

//   vtkPolyData* dataset = vtkPolyData::SafeDownCast (this->GetDataSet());
  
//   if (!dataset)
//     return;

//   vtkCellArray* celldata = vtkCellArray::New();
//   celldata->Initialize();

//   for(unsigned int i=0;i<dataset->GetPolys()->GetNumberOfCells();++i)
//   {
//     vtkIdList* pointids = vtkIdList::New();
//     dataset->GetCellPoints(i, pointids);
//     if (pointids->GetNumberOfIds() != 3)
//     {
//       std::cout<<"strange triangle"<<std::endl;
//       continue;
//     }
    
//     vtkLine* line1 = vtkLine::New();
//     line1->GetPointIds()->SetId (0, pointids->GetId (0));
//     line1->GetPointIds()->SetId (1, pointids->GetId (2));
//     celldata->InsertNextCell (line1);
//     line1->Delete();
//     vtkLine* line2 = vtkLine::New();
//     line2->GetPointIds()->SetId (0, pointids->GetId (2));
//     line2->GetPointIds()->SetId (1, pointids->GetId (1));
//     celldata->InsertNextCell (line2);
//     line2->Delete();
//     vtkLine* line3 = vtkLine::New();
//     line3->GetPointIds()->SetId (0, pointids->GetId (1));
//     line3->GetPointIds()->SetId (1, pointids->GetId (0));
//     celldata->InsertNextCell (line3);
//     line3->Delete();
    
//     pointids->Delete();
//     i++;
    
//   }

// //   dataset->SetLines (celldata);
//   celldata->Delete();

//   dataset->Modified();
//   this->Modified();
  
  
// }


void vtkMetaSurfaceMesh::CreateWirePolyData()
{
  if (this->GetWirePolyData())
    return;

  if (!this->GetPolyData())
    return;

  vtkExtractEdges* extractor = vtkExtractEdges::New();
  extractor->SetInput (this->GetPolyData());
  extractor->Update();

  this->SetWirePolyData (extractor->GetOutput());

  extractor->Delete();
  

//   vtkPolyData* dataset = vtkPolyData::New();
//   dataset->SetPoints (this->GetPolyData()->GetPoints());
//   dataset->Allocate();

  
//   for(unsigned int i=0;i<this->GetPolyData()->GetPolys()->GetNumberOfCells();++i)
//   {
//     vtkIdList* pointids = vtkIdList::New();
//     this->GetPolyData()->GetCellPoints(i, pointids);
//     if (pointids->GetNumberOfIds() != 3)
//     {
//       vtkWarningMacro(<<"warning : wrong type of cells !"<<endl);
//       pointids->Delete();
//       dataset->Delete();
      
//       return;
//     }
    
//     vtkIdList* list = vtkIdList::New();
//     list->SetNumberOfIds(2);
    
//     list->SetId (0, pointids->GetId (0));
//     list->SetId (1, pointids->GetId (1));
//     dataset->InsertNextCell (VTK_LINE, list);
//     list->SetId (0, pointids->GetId (1));
//     list->SetId (1, pointids->GetId (2));
//     dataset->InsertNextCell (VTK_LINE, list);
//     list->SetId (0, pointids->GetId (2));
//     list->SetId (1, pointids->GetId (0));
//     dataset->InsertNextCell (VTK_LINE, list);

//     list->Delete();
//     pointids->Delete();
//     i++;
    
//   }

//   this->SetWirePolyData (dataset);
//   dataset->Delete();
  
  this->Modified();
}
