/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaVolumeMesh.h>

#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkErrorCode.h>
#include <vtkIdList.h>
#include <vtkLegacyReaderVersion.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkUnsignedShortArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <vtksys/SystemTools.hxx>


#include <QDebug>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaVolumeMesh )

//----------------------------------------------------------------------------
vtkMetaVolumeMesh::vtkMetaVolumeMesh()
    : vtkMetaDataSet()
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
      reader->Delete();
      throw error;
    }
    reader->CloseVTKFile();
    reader->Delete();
}

void vtkMetaVolumeMesh::ReadVtuFile(const char* filename)
{
    vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
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
        qDebug() << "Reading: " << filename;
        switch (format)
        {
            case vtkMetaVolumeMesh::FILE_IS_VTK :
                this->ReadVtkFile (filename);
                break;
            case vtkMetaVolumeMesh::FILE_IS_VTU:
                this->ReadVtuFile(filename);
                break;
            case vtkMetaVolumeMesh::FILE_IS_MESH :
                this->ReadMeshFile (filename);
                break;
            case vtkMetaVolumeMesh::FILE_IS_GMESH :
                this->ReadGMeshFile (filename);
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
void vtkMetaVolumeMesh::WriteVtkFile (const char* filename)
{
    if (!this->DataSet)
    {
        vtkErrorMacro(<<"No DataSet to write"<<endl)
        throw vtkErrorCode::UserError;
    }
    vtkUnstructuredGrid* c_mesh = vtkUnstructuredGrid::SafeDownCast (this->DataSet);
    if (!c_mesh)
    {
        vtkErrorMacro(<<"DataSet is not a polydata object"<<endl)
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
void vtkMetaVolumeMesh::WriteVtuFile (const char* filename)
{
    if (!this->DataSet)
    {
        vtkErrorMacro(<<"No DataSet to write"<<endl)
        throw vtkErrorCode::UserError;
    }

    vtkUnstructuredGrid* c_mesh = vtkUnstructuredGrid::SafeDownCast (this->DataSet);
    if (!c_mesh)
    {
        vtkErrorMacro(<<"DataSet is not a polydata object"<<endl)
        throw vtkErrorCode::UserError;
    }

    vtkXMLUnstructuredGridWriter* writer = vtkXMLUnstructuredGridWriter::New();
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
void vtkMetaVolumeMesh::Write (const char* filename)
{
    try
    {
        if (vtkMetaVolumeMesh::IsVtuExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
        {
            qDebug()<<"Writing: "<<filename;
            this->WriteVtuFile (filename);
        }
        else if (vtkMetaVolumeMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
        {
            qDebug()<<"Writing: "<<filename;
            this->WriteVtkFile (filename);
        }
        else
        {
            qDebug()<<"Can not write: " << filename << " vtp extension is for polydata mesh";
            throw vtkErrorCode::UserError;
        }
    }
    catch (vtkErrorCode::ErrorIds error)
    {
        throw error;
    }
}

//----------------------------------------------------------------------------
bool vtkMetaVolumeMesh::IsVtpExtension (const char* ext)
{
    if (strcmp (ext, ".vtp") == 0)
    {
        return true;
    }
    return false;
}

bool vtkMetaVolumeMesh::IsVtuExtension (const char* ext)
{
    if (strcmp (ext, ".vtu") == 0)
    {
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
bool vtkMetaVolumeMesh::IsVtkExtension (const char* ext)
{
    if (strcmp (ext, ".vtk") == 0)
    {
        return true;
    }
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
        // medit .mesh format must have 'MeshVersionFormatted' as header
        if (vtkMetaDataSet::IsMeditFormat(filename))
        {
            // Additionally, check if there are any tetrahedra
            std::ifstream file(filename);
            if (vtkMetaDataSet::PlaceStreamCursor(file, "Tetrahedra"))
            {
                return vtkMetaVolumeMesh::FILE_IS_MESH;
            }
        }
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
        {
            return vtkMetaVolumeMesh::FILE_IS_GMESH;
        }
        else
        {
            return 0;
        }
    }

    if (vtkMetaVolumeMesh::IsVtuExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
        {
            return vtkMetaVolumeMesh::FILE_IS_VTU;
        }

    if (vtkMetaVolumeMesh::IsVtkExtension(vtksys::SystemTools::GetFilenameLastExtension(filename).c_str()))
    {
            vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
            reader->SetFileName (filename);
            if (reader->IsFileUnstructuredGrid ())
            {
                reader->Delete();
                return vtkMetaVolumeMesh::FILE_IS_VTK;
            }
            reader->Delete();
    }

    return 0;
}

void vtkMetaVolumeMesh::ReadMeshFile (const char* filename)
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
    elementsNames << "Vertices" << "Edges" << "Triangles" << "Tetrahedra" << "Ridges";
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

    vtkUnstructuredGrid* outputmesh = vtkUnstructuredGrid::New();
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
    //Get Tetras and their values
    if(positionStream(fileInput, elementsNameHash[elementsNames[3]].first+1, line))
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[3]].second)
        {
            std::istringstream  lineStream(line);
            unsigned int pos[4];
            unsigned short ref;
            lineStream >> pos[0] >> pos[1] >> pos[2] >> pos[3] >> ref;
            vtkIdList* idlist = vtkIdList::New();
            idlist->InsertNextId(pos[0]-1);
            idlist->InsertNextId(pos[1]-1);
            idlist->InsertNextId(pos[2]-1);
            idlist->InsertNextId(pos[3]-1);

            outputmesh->InsertNextCell(VTK_TETRA, idlist);
            idlist->Delete();
            cellarray->InsertNextValue(ref);
            curLine++;
        }
    }
    //Get Ridges if present will be add as array on cell
    if(positionStream(fileInput, elementsNameHash[elementsNames[4]].first+1, line) &&
            elementsNameHash[elementsNames[1]].second > 0)
    {
        curLine = 0;
        while (getline(fileInput, line) && curLine < elementsNameHash[elementsNames[4]].second)
        {
            std::istringstream  lineStream(line);
            unsigned int pos;
            lineStream >> pos;
            //This work because the edges were the first cells added
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
        if(elementsNameHash[elementsNames[4]].second > 0 && elementsNameHash[elementsNames[1]].second > 0)
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

void vtkMetaVolumeMesh::ReadGMeshFile (const char* filename)
{
    std::ifstream file (filename );
    char str[256];

    if(file.fail())
    {
        vtkErrorMacro("File not found\n")
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
            vtkErrorMacro("No point in file\n")
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

    file >> str;
    while( (strcmp (str, "$ELM") != 0) )
    {
        if (file.fail())
        {
            points->Delete();
            pointarray->Delete();
            cellarray->Delete();
            outputmesh->Delete();
            vtkErrorMacro("No tetrahedron in file\n")
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

    this->SetDataSet (outputmesh);

    points->Delete();
    pointarray->Delete();
    cellarray->Delete();
    outputmesh->Delete();
}
