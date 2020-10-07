/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataManager.h>
#include "vtkObjectFactory.h"

#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaDataSetSequence.h>

#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkErrorCode.h>
#include <vtkDirectory.h>
#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <sstream>
#include <time.h>

#ifdef WIN32
#define snprintf sprintf_s
#endif

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkDataManager );



//----------------------------------------------------------------------------
vtkDataManager::vtkDataManager()
{
}

//----------------------------------------------------------------------------
vtkDataManager::~vtkDataManager()
{
    // should delete all metadatasets...
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        this->MetaDataSetList[i]->Delete();
    }
    this->MetaDataSetList.clear();
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkDataManager::GetMetaDataSet (unsigned int id) const
{
    if (id >= this->MetaDataSetList.size())
        return nullptr;
    return this->MetaDataSetList[id];
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkDataManager::GetMetaDataSet (const char *name) const
{
    unsigned int id = 0;

    if (!this->IsNameInManager(name, id))
        return nullptr;

    return this->MetaDataSetList[id];
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkDataManager::GetMetaDataSetFromTag (const char *tag) const
{
    unsigned int id = 0;

    if (!this->IsTagInManager(tag, id))
        return nullptr;

    return this->MetaDataSetList[id];
}

//----------------------------------------------------------------------------
void vtkDataManager::AddMetaDataSet (vtkMetaDataSet *metadataset)
{
    if (!metadataset)
        return;

    srand (clock());

    // put a random color
    int i1 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i2 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i3 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    double rand_1 = (double)(i1)/(100.0);
    double rand_2 = (double)(i2)/(100.0);
    double rand_3 = (double)(i3)/(100.0);
    double r = rand_1, g = rand_2, b = rand_3;
    if (i1 < 33)
        r = 1;
    else if (i1 < 66)
        g = 1;
    else if (i1 < 100)
        b = 1;


    vtkProperty *prop = vtkProperty::SafeDownCast(metadataset->GetProperty());

    if (prop)
        prop->SetColor (r, g, b);

    bool nameisok = false;
    unsigned int adding = 1;

//   std::string name_str = vtksys::SystemTools::GetFilenameWithoutExtension (metadataset->GetName());
//   metadataset->SetName(name_str.c_str());

    if (!this->GetMetaDataSet(metadataset->GetName()))
        nameisok = true;

    while (!nameisok)
    {
        std::ostringstream name;
        name <<metadataset->GetName()<<"("<<adding<<")";

        metadataset->SetName(name.str().c_str());
        metadataset->SetTag(name.str().c_str());
        if (!this->GetMetaDataSet(metadataset->GetName()))
            nameisok = true;
    }

    this->MetaDataSetList.push_back (metadataset);
    metadataset->Register(this);
}

void vtkDataManager::UpdateSequencesToTime (double time)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence)
        {
            sequence->UpdateToTime (time);
        }
    }
}

void vtkDataManager::UpdateSequencesToIndex (unsigned int id)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence)
        {
            sequence->UpdateToIndex (id);
        }
    }
}

void vtkDataManager::UpdateSequencesMatchingTagToTime (const char *tag, double time)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence && (strcmp (sequence->GetTag(), tag) == 0))
        {
            sequence->UpdateToTime (time);
        }
    }
}

double vtkDataManager::GetSequencesRangeMin()
{
    double min = 9999999;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence)
        {
            double temp = sequence->GetMinTime();
            if (temp < min)
                min = temp;
        }
    }
    return min;
}

double vtkDataManager::GetSequencesRangeMax()
{
    double max = -9999999;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence)
        {
            double temp = sequence->GetMaxTime();
            if (temp > max)
                max = temp;
        }
    }
    return max;
}

unsigned int vtkDataManager::GetSequencesMaxNumber()
{
    unsigned int max = 0;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = nullptr;
        sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);
        if (sequence)
        {
            unsigned int temp = sequence->GetNumberOfMetaDataSets();
            if (temp > max)
                max = temp;
        }
    }
    return max;
}

//----------------------------------------------------------------------------
unsigned int vtkDataManager::GetNumberOfMetaDataSet() const
{
    return this->MetaDataSetList.size();
}

//----------------------------------------------------------------------------
unsigned int vtkDataManager::GetNumberOfTypedMetaDataSet (unsigned int type) const
{
    unsigned int ret = 0;
    for (unsigned int i = 0; i < this->MetaDataSetList.size(); i++)
    {
        if (this->MetaDataSetList[i]->GetType() == type)
            ret++;
    }
    return ret;
}

//----------------------------------------------------------------------------
std::vector<vtkMetaDataSet*> vtkDataManager::GetMetaDataSetListFromTag (const char *tag) const
{
    std::vector<vtkMetaDataSet*> list;
    for (unsigned int i = 0; i < this->MetaDataSetList.size(); i++)
    {
        if (strcmp (this->MetaDataSetList[i]->GetTag(), tag) == 0)
            list.push_back (this->MetaDataSetList[i]);
    }
    return list;
}

std::vector<vtkMetaDataSetSequence*> vtkDataManager::GetSequenceListFromTag (const char *tag) const
{
    std::vector<vtkMetaDataSetSequence*> list;
    for (unsigned int i = 0; i < this->MetaDataSetList.size(); i++)
    {
        vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSetList[i]);

        if (sequence && (strcmp (sequence->GetTag(), tag) == 0))
            list.push_back (sequence);
    }

    return list;
}

//----------------------------------------------------------------------------
std::vector<vtkMetaDataSet*> vtkDataManager::GetTypedMetaDataSetList (unsigned int type) const
{
    std::vector<vtkMetaDataSet*> list;
    for (unsigned int i = 0; i < this->MetaDataSetList.size(); i++)
    {
        if (this->MetaDataSetList[i]->GetType() == type)
            list.push_back (this->MetaDataSetList[i]);
    }
    return list;
}

//----------------------------------------------------------------------------
bool vtkDataManager::IsInManager (vtkMetaDataSet *metadataset, unsigned int &id) const
{
    if (!metadataset)
        return false;
    for (unsigned int i = 0; i < this->MetaDataSetList.size(); i++)
    {
        if (this->MetaDataSetList[i] == metadataset)
        {
            id = i;
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
bool vtkDataManager::IsNameInManager (const char *name, unsigned int &id) const
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if (strcmp (name, this->MetaDataSetList[i]->GetName()) == 0)
        {
            id = i;
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
bool vtkDataManager::IsTagInManager (const char *tag, unsigned int &id) const
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if (strcmp (tag, this->MetaDataSetList[i]->GetTag()) == 0)
        {
            id = i;
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
const char*vtkDataManager::CreateDefaultName (unsigned int type, const char *filename)
{
    char *ret = new char[512];


    if (filename)
    {
        std::string filename_str = vtksys::SystemTools::GetFilenameWithoutExtension(vtksys::SystemTools::GetFilenameName (filename));

        snprintf (ret, 512, "%s", filename_str.c_str());
        unsigned int id = 0;

        if (!this->IsNameInManager (ret, id))
            return ret;
    }


    bool isused = true;
    int id = 0;

    while (isused == true)
    {
        switch (type)
        {
            case vtkMetaDataSet::VTK_META_IMAGE_DATA:
                snprintf (ret, 512, "image_%i", id);
                break;
            case vtkMetaDataSet::VTK_META_SURFACE_MESH:
                snprintf (ret, 512, "mesh_%i", id);
                break;
            case vtkMetaDataSet::VTK_META_VOLUME_MESH:
                snprintf (ret, 512, "tetra_%i", id);
                break;
            default:
                snprintf (ret, 512, "unknown_%i", id);
                break;
        }
        unsigned int dummy = 0;
        isused = this->IsNameInManager (ret, dummy);
        id++;
    }
    return ret;
}

//----------------------------------------------------------------------------
void vtkDataManager::RemoveMetaDataSet (vtkMetaDataSet *metadataset)
{
    if (!metadataset)
        return;

    std::vector<vtkMetaDataSet*> templist = this->MetaDataSetList;
    this->MetaDataSetList.clear();

    for (unsigned int i=0; i<templist.size(); i++)
    {
        if (templist[i] != metadataset)
            this->MetaDataSetList.push_back (templist[i]);
        else
            templist[i]->Delete();
    }
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkDataManager::ReadFile (const char *filename, const char *name, bool forsequence)
{
    if (vtksys::SystemTools::FileIsDirectory (filename))
    {
        return this->ScanDirectoryForSequence (filename);
    }

    vtkMetaDataSet *metadataset = nullptr;

    if (vtkMetaVolumeMesh::CanReadFile (filename))
    {
        metadataset = vtkMetaVolumeMesh::New();
    }
    else if (vtkMetaSurfaceMesh::CanReadFile (filename))
    {
        metadataset = vtkMetaSurfaceMesh::New();
    }
    else
    {
        vtkErrorMacro(<<"unknown file format : "<<filename<<endl);
        throw vtkErrorCode::UnrecognizedFileTypeError;
    }

    try
    {
        metadataset->Read(filename);

        if (name == nullptr)
            metadataset->SetName (vtksys::SystemTools::GetFilenameWithoutExtension(vtksys::SystemTools::GetFilenameName (filename).c_str()).c_str());
        else
            metadataset->SetName (name);

        this->AddMetaDataSet(metadataset);
        metadataset->Delete();

        return metadataset;
    }
    catch (vtkErrorCode::ErrorIds error)
    {
        metadataset->Delete();
        throw error;
    }


    return nullptr;
}

//----------------------------------------------------------------------------
void vtkDataManager::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent <<" has " <<this->MetaDataSetList.size()<<" MetaDataSets ... " << endl;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        this->MetaDataSetList[i]->PrintSelf (os, indent);
    }
}

//----------------------------------------------------------------------------
void vtkDataManager::ScanDirectory (const char *dirname)
{
    vtkDirectory *directory = vtkDirectory::New();
    int ret = directory->Open(dirname);
    if (!ret)
    {
        vtkErrorMacro(<<"Cannot open directory"<<dirname<<endl);
        throw vtkErrorCode::CannotOpenFileError;
    }

    std::string directoryname = dirname;
    std::string::size_type slashpos;

    // check slash position
#ifdef WIN32
    slashpos = directoryname.rfind ("\\"); // last occurence of "/"
    if (slashpos != directoryname.size()-1)
        directoryname+="\\";
#else
    slashpos = directoryname.rfind ("/"); // last occurence of "/"
    if (slashpos != directoryname.size()-1)
        directoryname+="/";
#endif

    std::vector<std::string> list;

    for (int i=0; i<directory->GetNumberOfFiles(); i++)
    {
        if ( (strcmp(directory->GetFile (i), ".") == 0 )
             || (strcmp(directory->GetFile (i), "..") == 0 ))
            continue;

        list.push_back((directoryname+directory->GetFile (i)));
    }

    std::sort(list.begin(), list.end());

    for (unsigned int i = 0; i < list.size(); i++)
    {
        try
        {
            this->ReadFile(list[i].c_str());
        }
        catch (vtkErrorCode::ErrorIds)
        {
            // we don't throw exception as we want to continue scanning the directory
            std::cerr<<"skipping file "<<list[i].c_str()<<std::endl;
        }
    }
    directory->Delete();
}

//----------------------------------------------------------------------------
vtkMetaDataSetSequence*vtkDataManager::ScanDirectoryForSequence (const char *dirname, double duration)
{
    vtkMetaDataSetSequence *Sequence = vtkMetaDataSetSequence::New();
    try
    {
        Sequence->SetSequenceDuration (duration);
        Sequence->Read(dirname);
        std::vector<std::string> components;
        vtksys::SystemTools::SplitPath (dirname, components);
        if (components.size())
        {
            std::string lastname = components[components.size() - 1];
            Sequence->SetName (lastname.c_str());
        }

        this->AddMetaDataSet (Sequence);
        Sequence->Delete();
    }
    catch (vtkErrorCode::ErrorIds error)
    {
        Sequence->Delete();
        throw error;
    }

    return Sequence;
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkDataManager::DuplicateMetaDataSet(vtkMetaDataSet *input)
{
    vtkMetaDataSet *metadataset;
    vtkDataSet *dataset = nullptr;

    switch(input->GetType())
    {
        case vtkMetaDataSet::VTK_META_SURFACE_MESH:
            metadataset = vtkMetaSurfaceMesh::New();
            dataset = vtkPolyData::New();
            break;
        case vtkMetaDataSet::VTK_META_VOLUME_MESH:
            metadataset = vtkMetaVolumeMesh::New();
            dataset = vtkUnstructuredGrid::New();
            break;
        default:
            return nullptr;
    }

    if (input->GetDataSet())
        dataset->DeepCopy (input->GetDataSet());

    metadataset->CopyInformation (input);
    metadataset->SetDataSet (dataset);
    metadataset->SetProperty (input->GetProperty());

    dataset->Delete();

    return metadataset;
}

