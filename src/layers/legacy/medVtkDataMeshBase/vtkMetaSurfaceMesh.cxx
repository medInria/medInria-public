/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaSurfaceMesh.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellType.h>
#include <vtkCleanPolyData.h>
#include <vtkCommand.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkErrorCode.h>
#include <vtkExtractEdges.h>
#include <vtkIdList.h>
#include <vtkLegacyReaderVersion.h>
#include <vtkLine.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkOBJExporter.h>
#include <vtkOBJReader.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkUnsignedShortArray.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include <vtksys/SystemTools.hxx>

#include <QDebug>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaSurfaceMesh )

//----------------------------------------------------------------------------
vtkMetaSurfaceMesh::vtkMetaSurfaceMesh()
    : vtkMetaDataSet()
{
  this->Type = vtkMetaDataSet::VTK_META_SURFACE_MESH;
}

vtkMetaSurfaceMesh::vtkMetaSurfaceMesh(const vtkMetaSurfaceMesh& other)
    : vtkMetaDataSet(other)
{
}

//----------------------------------------------------------------------------
vtkMetaSurfaceMesh::~vtkMetaSurfaceMesh()
{
}

vtkMetaSurfaceMesh* vtkMetaSurfaceMesh::Clone()
{
  return new vtkMetaSurfaceMesh(*this);
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
    return nullptr;
  return vtkPolyData::SafeDownCast (this->DataSet);
}

//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::ReadVtkFile (const char* filename)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName (filename);

    try
    {
      reader->Update();
      if(reader->GetFileMajorVersion() <= vtkLegacyReaderMajorVersion)
      {
          this->SetDataSet (reader->GetOutput());
      }
      else
      {
          vtkErrorMacro(<< "Can not read file with version > " << vtkLegacyReaderMajorVersion
                        << "." << vtkLegacyReaderMinorVersion << ": " << filename << endl)
          throw vtkErrorCode::FileFormatError;
      }
    }
    catch (vtkErrorCode::ErrorIds error)
    {
      reader->CloseVTKFile();
      throw error;
    }
    reader->CloseVTKFile();
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
    cleanFilter->SetInputData(reader->GetOutput());
    cleanFilter->ConvertLinesToPointsOn();
    cleanFilter->ConvertPolysToLinesOn();
    cleanFilter->ConvertStripsToPolysOn();
    cleanFilter->PointMergingOn();
    cleanFilter->Update();
    vtkDataSetSurfaceFilter* surfaceFilter = vtkDataSetSurfaceFilter::New();
    surfaceFilter->SetInputData(cleanFilter->GetOutput());
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
        qDebug() << "Reading: " << filename;
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
                vtkErrorMacro(<<"unknown dataset type : "<<filename<<endl)
                throw vtkErrorCode::UnrecognizedFileTypeError;
        }
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
    vtkErrorMacro(<<"No DataSet to write"<<endl)
    throw vtkErrorCode::UserError;
  }

  vtkPolyData* c_mesh = vtkPolyData::SafeDownCast (this->DataSet);
  if (!c_mesh)
  {
    vtkErrorMacro(<<"DataSet is not a polydata object"<<endl)
    throw vtkErrorCode::UserError;
  }
  
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
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
  
}


//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::WriteVtpFile (const char* filename)
{
    if (!this->DataSet)
    {
        vtkErrorMacro(<<"No DataSet to write"<<endl)
        throw vtkErrorCode::UserError;
    }
    
    vtkPolyData* c_mesh = vtkPolyData::SafeDownCast (this->DataSet);
    if (!c_mesh)
    {
        vtkErrorMacro(<<"DataSet is not a polydata object"<<endl)
        throw vtkErrorCode::UserError;
    }
    
    vtkXMLPolyDataWriter* writer = vtkXMLPolyDataWriter::New();
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
}

//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::WriteOBJFile (const char* filename)
{
    Q_UNUSED(filename)
    vtkErrorMacro(<<"Not yet implemented"<<endl)
}


//----------------------------------------------------------------------------
void vtkMetaSurfaceMesh::Write (const char* filename)
{
    try
    {
        if (vtkMetaSurfaceMesh::IsVtpExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
        {
            this->WriteVtpFile (filename);
        }
        else if (vtkMetaSurfaceMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
        {
            qDebug()<<"Writing: "<<filename;
            this->WriteVtkFile (filename);
        }
        else
        {
            qDebug()<<"Can not write: " << filename << " vtu extension is for unstructured grid mesh";
            throw vtkErrorCode::UserError;
        }
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
    {
        return true;
    }
    return false;
}

bool vtkMetaSurfaceMesh::IsVtuExtension (const char* ext)
{
    if (strcmp (ext, ".vtu") == 0)
    {
        return true;
    }
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
    {
        // medit .mesh format must have 'MeshVersionFormatted' as header
        if (vtkMetaDataSet::IsMeditFormat(filename))
        {
            return vtkMetaSurfaceMesh::FILE_IS_MESH;
        }
        return 0;
    }

    if (vtkMetaSurfaceMesh::IsOBJExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    {
        return vtkMetaSurfaceMesh::FILE_IS_OBJ;
    }

    if (vtkMetaSurfaceMesh::IsVtpExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    {
        return vtkMetaSurfaceMesh::FILE_IS_VTP;
    }
    
    if (vtkMetaSurfaceMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    {
        vtkPolyDataReader* reader = vtkPolyDataReader::New();
        reader->SetFileName (filename);
        if (reader->IsFilePolyData())
        {
            reader->Delete();
            return vtkMetaSurfaceMesh::FILE_IS_VTK;
        }
        reader->Delete();
    }

    return 0;
}


void vtkMetaSurfaceMesh::ReadMeshFile (const char* filename)
{
    // lambda to position the stream where needed
    auto positionStream = [](std::ifstream& file, int nbline, std::string& line)
    {
        bool result = true;
        //not sure if the order of cells (edges, triangles, tetra) is always the same or not
        //to be sure we go back to the beginning
        file.clear();
        file.seekg(0);
        if(nbline > 1)
        {
            int curentLine = 0;
            while (curentLine != nbline && getline(file, line))
            {
                curentLine++;
            }
            if (file.bad() || nbline > curentLine)
            {
                result =  false;
            }
        }
        else
        {
         result = false;
        }
        return result;
    };

    ifstream fileInput(filename );
    if(fileInput.fail())
    {
        vtkErrorMacro("File not found\n")
        throw vtkErrorCode::FileNotFoundError;
    }

    QHash<QString, QPair<int,int>> elementsNameHash;
    QStringList elementsNames;
    elementsNames << "Vertices" << "Edges" << "Triangles" << "Ridges";
    for (int i = 0; i< elementsNames.size(); i++)
    {
        elementsNameHash[elementsNames[i]] = qMakePair(0, 0);
    }

    int curLine = 0;
    std::string line;

    while(getline(fileInput, line))
    {
        curLine++;
        for (int i = 0; i< elementsNames.size(); i++)
        {
            if (elementsNameHash[elementsNames[i]].first == curLine - 1
                    && elementsNameHash[elementsNames[i]].first != 0)
            {
                std::istringstream  lineStream(line);
                lineStream >> elementsNameHash[elementsNames[i]].second;
            }

            if (line.find(elementsNames[i].toStdString(), 0) != std::string::npos)
            {
                 elementsNameHash[elementsNames[i]].first = curLine;
            }
        }
    }

    int nbOfCells = 0;
    for (int i = 1; i< elementsNames.size()-1; i++)
    {
        nbOfCells += elementsNameHash[elementsNames[i]].second;
    }

    int nbOfPoints = elementsNameHash[elementsNames[0]].second;

    vtkPoints* points = vtkPoints::New();
    points->SetNumberOfPoints(nbOfPoints);

    vtkUnsignedShortArray* pointarray = vtkUnsignedShortArray::New();
    pointarray->SetName("Point array");
    pointarray->Allocate(nbOfPoints);

    vtkUnsignedShortArray* cellarray  = vtkUnsignedShortArray::New();
    cellarray->SetName("Zones");
    cellarray->Allocate(nbOfCells);

    vtkUnsignedShortArray* ridgesarray  = vtkUnsignedShortArray::New();
    ridgesarray->SetName("Ridges");
    ridgesarray->SetNumberOfComponents(1);
    ridgesarray->SetNumberOfTuples(nbOfCells);
    ridgesarray->FillValue(0);

    vtkPolyData* outputmesh = vtkPolyData::New();
    outputmesh->Allocate(nbOfCells);

    //Get Vertices and their values
    if(positionStream(fileInput, elementsNameHash[elementsNames[0]].first+1, line))
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[0]].second)
        {
            std::istringstream  lineStream(line);
            double pos[3];
            unsigned short ref;
            lineStream >> pos[0] >> pos[1] >> pos[2] >> ref;
            points->SetPoint(curLine, pos[0], pos[1], pos[2]);
            pointarray->InsertNextValue(ref);
            curLine++;
        }
    }
    //Get Edges and their values
    if(positionStream(fileInput, elementsNameHash[elementsNames[1]].first+1, line))
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[1]].second)
        {
            std::istringstream  lineStream(line);
            unsigned int pos[2];
            unsigned short ref;
            lineStream >> pos[0] >> pos[1] >> ref;
            vtkIdList* idlist = vtkIdList::New();
            idlist->InsertNextId(pos[0]-1);
            idlist->InsertNextId(pos[1]-1);

            outputmesh->InsertNextCell(VTK_LINE, idlist);
            idlist->Delete();
            cellarray->InsertNextValue(ref);
            curLine++;
        }
    }
    //Get Triangles and their values
    if(positionStream(fileInput, elementsNameHash[elementsNames[2]].first+1, line))
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[2]].second)
        {
            std::istringstream  lineStream(line);
            unsigned int pos[3];
            unsigned short ref;
            lineStream >> pos[0] >> pos[1] >> pos[2] >> ref;
            vtkIdList* idlist = vtkIdList::New();
            idlist->InsertNextId(pos[0]-1);
            idlist->InsertNextId(pos[1]-1);
            idlist->InsertNextId(pos[2]-1);

            outputmesh->InsertNextCell(VTK_TRIANGLE, idlist);
            idlist->Delete();
            cellarray->InsertNextValue(ref);
            curLine++;
        }
    }
    //Get Ridges if present will be add as array on cell
    if(positionStream(fileInput, elementsNameHash[elementsNames[3]].first+1, line) &&
            elementsNameHash[elementsNames[1]].second > 0)
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[3]].second)
        {
            std::istringstream  lineStream(line);
            unsigned int pos;
            lineStream >> pos;
            //This works because the edges were the first cells added
            ridgesarray->SetValue(pos-1, 1);
            curLine++;
        }
    }

    outputmesh->SetPoints(points);
    if (outputmesh->GetPointData())
    {
        outputmesh->GetPointData()->AddArray(pointarray);
    }
    if (outputmesh->GetCellData())
    {
        outputmesh->GetCellData()->AddArray(cellarray);
        if(elementsNameHash[elementsNames[3]].second > 0 && elementsNameHash[elementsNames[1]].second > 0)
        {
            outputmesh->GetCellData()->AddArray(ridgesarray);
        }
    }

    this->SetDataSet(outputmesh);

    points->Delete();
    pointarray->Delete();
    cellarray->Delete();
    ridgesarray->Delete();
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
  extractor->SetInputData (this->GetPolyData());
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
