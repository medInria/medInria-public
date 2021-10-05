/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaDataSetSequence.h>
#include "vtkObjectFactory.h"

#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkActorCollection.h>
#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkCellData.h>
#include <vtksys/SystemTools.hxx>
#include <vtkDirectory.h>
#include <vtkErrorCode.h>
#include <vtkLookupTable.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkDataArrayCollection.h>

#include <sstream>
#include <algorithm> // for sort algorithm

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaDataSetSequence )

//----------------------------------------------------------------------------
vtkMetaDataSetSequence::vtkMetaDataSetSequence()
  : vtkMetaDataSet()
{
    this->SequenceDuration = 2.0;
    this->CurrentId = -1;
    this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;
    this->SameGeometryFlag = true;
    this->ParseAttributes = true;
}

vtkMetaDataSetSequence::vtkMetaDataSetSequence(const vtkMetaDataSetSequence& other)
  : vtkMetaDataSet(other)
{
    this->SequenceDuration = other.SequenceDuration;
    this->CurrentId = other.CurrentId;
    this->SameGeometryFlag = other.SameGeometryFlag;
    this->ParseAttributes = other.ParseAttributes;

    for (unsigned int i = 0; i < other.MetaDataSetList.size(); ++i)
    {
        this->MetaDataSetList.push_back(other.MetaDataSetList[i]->Clone());
    }
}

//----------------------------------------------------------------------------
vtkMetaDataSetSequence::~vtkMetaDataSetSequence()
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        this->MetaDataSetList[i]->Delete();
    }
    this->MetaDataSetList.clear();
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::Initialize()
{
    this->Superclass::Initialize();
}

vtkMetaDataSetSequence* vtkMetaDataSetSequence::Clone()
{
    return new vtkMetaDataSetSequence(*this);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::AddMetaDataSet (vtkMetaDataSet *metadataset)
{
    if (!metadataset)
    {
        vtkErrorMacro(<<"nullptr object !"<<endl);
        throw vtkErrorCode::UserError;
    }

    if (this->Type == vtkMetaDataSet::VTK_META_UNKNOWN)
        this->Type = metadataset->GetType();

    if ( metadataset->GetType() != this->Type)
    {
        vtkErrorMacro(<<"Cannot add heterogeneous type datasets to sequence !"<<endl);
        throw vtkErrorCode::UserError;
    }

    std::vector<vtkMetaDataSet*>::iterator it;
    bool inserted = false;

    for (it = this->MetaDataSetList.begin(); it != this->MetaDataSetList.end(); it++)
    {
        if ((*it)->GetTime() > metadataset->GetTime())
        {
            this->MetaDataSetList.insert(it, metadataset);
            inserted = true;
        }
    }
    if (!inserted)
    {
        this->MetaDataSetList.push_back (metadataset);
    }
    metadataset->Register(this);

    try
    {
        if (!this->GetDataSet())
        {
            this->Time = metadataset->GetTime();
            this->CurrentId = this->MetaDataSetList.size() - 1;
            this->BuildMetaDataSetFromMetaDataSet (metadataset);
        }
        this->ComputeSequenceDuration();
    }
    catch (vtkErrorCode::ErrorIds error)
    {
        throw error;
    }
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveMetaDataSet (unsigned int id)
{
    if (id >= this->MetaDataSetList.size())
        return;

    std::vector<vtkMetaDataSet*> templist = this->MetaDataSetList;
    this->MetaDataSetList.clear();

    for (unsigned int i=0; i<templist.size(); i++)
    {
        if (i != id)
            this->MetaDataSetList.push_back (templist[i]);
        else
        {
            templist[i]->UnRegister(this);
        }
    }

    this->ComputeSequenceDuration();

    if (this->MetaDataSetList.size() == 0)
    {
        this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;
        if (this->DataSet)
        {
            this->DataSet->Delete();
        }
    }
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveMetaDataSet (vtkMetaDataSet *metadataset)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if (this->MetaDataSetList[i] == metadataset)
        {
            this->RemoveMetaDataSet(i);
            return;
        }
    }
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveAllMetaDataSets()
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        this->MetaDataSetList[i]->UnRegister(this);
    }
    this->MetaDataSetList.clear();
    this->ComputeSequenceDuration();
    this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;

    if (this->DataSet)
    {
        this->DataSet->Delete();
    }
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkMetaDataSetSequence::GetMetaDataSet (unsigned int i)
{
    if (i>=this->MetaDataSetList.size())
        return nullptr;
    return this->MetaDataSetList[i];
}

//----------------------------------------------------------------------------
bool vtkMetaDataSetSequence::HasMetaDataSet (vtkMetaDataSet *metadataset)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if (this->MetaDataSetList[i] == metadataset)
            return true;
    }
    return false;
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkMetaDataSetSequence::FindMetaDataSet (const char *name)
{
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if (strcmp (this->MetaDataSetList[i]->GetName(), name) == 0)
            return this->MetaDataSetList[i];
    }
    return nullptr;
}

//----------------------------------------------------------------------------
vtkMetaDataSet*vtkMetaDataSetSequence::FindMetaDataSet (double time, unsigned int &id)
{
    double distance = VTK_DOUBLE_MAX;
    double framedistance;
    vtkMetaDataSet *ret = 0;
    unsigned int i;


    for (i=0; i<this->MetaDataSetList.size(); i++)
    {
        framedistance = fabs (time - this->MetaDataSetList[i]->GetTime());
        if (framedistance < distance)
        {
            ret = this->MetaDataSetList[i];
            distance = framedistance;
            id = i;
        }
    }


    return ret;
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetRelativeTime (double time)
{
    if (this->MetaDataSetList.size() == 0)
        return 0.0;

    if (this->SequenceDuration <= 0 )
        return 0;
    double t = time;
    while (t > (this->SequenceDuration + 0.000001) )
    {
        t -= this->SequenceDuration;
    }
    return (t);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::BuildMetaDataSetFromMetaDataSet (vtkMetaDataSet *metadataset)
{
    this->CopyInformation(metadataset);
    this->SetProperty (metadataset->GetProperty());

    vtkDataSet *dataset = nullptr;

    switch (metadataset->GetDataSet()->GetDataObjectType())
    {
        case VTK_IMAGE_DATA:
            dataset = vtkImageData::New();
            break;
        case VTK_POLY_DATA:
            dataset = vtkPolyData::New();
            break;
        case VTK_UNSTRUCTURED_GRID:
            dataset = vtkUnstructuredGrid::New();
            break;
        default:
            vtkErrorMacro(<<"Unknown type !"<<endl);
            throw vtkErrorCode::UnrecognizedFileTypeError;
    }


    dataset->DeepCopy (metadataset->GetDataSet());
    try
    {
        this->SetDataSet(dataset);
    }
    catch (vtkErrorCode::ErrorIds &e)
    {
        throw e;
    }

    dataset->Delete();
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetMinTime() const
{
    double ret = 0;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if ( ret > this->MetaDataSetList[i]->GetTime())
            ret = this->MetaDataSetList[i]->GetTime();
    }
    return ret;
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetMaxTime() const
{
    double ret = 0;
    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        if ( ret < this->MetaDataSetList[i]->GetTime())
            ret = this->MetaDataSetList[i]->GetTime();
    }
    return ret;
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetTimeResolution()
{
    if (!this->MetaDataSetList.size())
        return 0;

    this->ComputeSequenceDuration();

    double ret = this->SequenceDuration / (double)this->GetNumberOfMetaDataSets();

    return ret;
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ComputeSequenceDuration()
{
    if (this->MetaDataSetList.size() < 2)
    {
        this->SequenceDuration = this->GetMaxTime();
        return;
    }

    this->SequenceDuration = this->GetMaxTime() - this->GetMinTime();
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::UpdateToTime (double time)
{
    unsigned int id = 0;
    if (this->FindMetaDataSet (time, id))
    {
        this->UpdateToIndex (id);
    }

    this->SetTime (time);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::UpdateToIndex (unsigned int id)
{
    if ((int)id == this->CurrentId)
        return;

    if (id >= this->MetaDataSetList.size())
        return;

    vtkDataSet *datasettoshow  = this->MetaDataSetList[id]->GetDataSet();

    vtkPolyData *polydatatoshow   = vtkPolyData::SafeDownCast (datasettoshow);
    vtkPolyData *polydatatochange = vtkPolyData::SafeDownCast (this->GetDataSet());

    if (polydatatoshow && polydatatochange) {
        polydatatochange->ShallowCopy (polydatatoshow);
    } else {
        vtkUnstructuredGrid *unstructuredgridtoshow   = vtkUnstructuredGrid::SafeDownCast (datasettoshow);
        vtkUnstructuredGrid *unstructuredgridtochange = vtkUnstructuredGrid::SafeDownCast (this->GetDataSet());
        if (unstructuredgridtoshow && unstructuredgridtochange) {
            unstructuredgridtochange->ShallowCopy (unstructuredgridtoshow);
        }
    }

    if (this->ParseAttributes)
    {
        vtkDataArray *pdscalars = this->GetDataSet()->GetPointData()->GetScalars();
        vtkDataArray *cdscalars = this->GetDataSet()->GetCellData()->GetScalars();
        vtkDataArray *pdtensors = this->GetDataSet()->GetPointData()->GetTensors();
        vtkDataArray *cdtensors = this->GetDataSet()->GetCellData()->GetTensors();

        if (pdscalars)
        {
            const char *c_name = pdscalars->GetName();
            if ( c_name && (*c_name) )
            {
                std::string name = c_name;
                vtkDataArray *array = datasettoshow->GetPointData()->GetArray(name.c_str());
                if (array)
                {
                    pdscalars->DeepCopy (array);
                }
            }
            else
            {
                this->GetDataSet()->GetPointData()->SetScalars(datasettoshow->GetPointData()->GetScalars());
            }

            this->GetDataSet()->GetPointData()->Modified();
            this->GetDataSet()->Modified();
        }

        if (cdscalars)
        {
            const char *c_name = cdscalars->GetName();
            if ( c_name && (*c_name) )
            {
                std::string name = c_name;
                vtkDataArray *array = datasettoshow->GetCellData()->GetArray(name.c_str());

                if (array)
                {
                    cdscalars->DeepCopy (array);
                }
            }
            else
            {
                this->GetDataSet()->GetCellData()->SetScalars(datasettoshow->GetCellData()->GetScalars());
            }
            this->GetDataSet()->GetCellData()->Modified();
        }
        if (pdtensors)
        {
            const char *c_name = pdtensors->GetName();
            if ( c_name && (*c_name) )
            {
                std::string name = c_name;
                vtkDataArray *array = datasettoshow->GetPointData()->GetArray(name.c_str());
                if (array)
                {
                    pdtensors->DeepCopy (array);
                }
            }
            else
            {
                this->GetDataSet()->GetPointData()->SetTensors(datasettoshow->GetPointData()->GetTensors());
            }

            this->GetDataSet()->GetPointData()->Modified();
        }

        if (cdtensors)
        {
            const char *c_name = cdtensors->GetName();
            if ( c_name && (*c_name) )
            {
                std::string name = c_name;
                vtkDataArray *array = datasettoshow->GetCellData()->GetArray(name.c_str());

                if (array)
                {
                    cdtensors->DeepCopy (array);
                }
            }
            else
            {
                this->GetDataSet()->GetCellData()->SetTensors(datasettoshow->GetCellData()->GetTensors());
            }
            this->GetDataSet()->GetCellData()->Modified();
        }
    }

    // updating the current id
    this->CurrentId   = id;
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::GetCurrentScalarRange(double range[2])
{
    GetScalarRange(range);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ColorByArray(vtkDataArray *array)
{
    this->CurrentScalarArray = array;

    if (!array)
        return;

    if (!this->MetaDataSetList.size())
        return;

    if (!this->DataSet)
        return;

    bool array_is_in_points = false;

    if (this->DataSet->GetPointData()->HasArray (array->GetName()))
        array_is_in_points = true;

    double min = 0, max = 0;

    vtkLookupTable *lut = array->GetLookupTable();

    for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    {
        vtkDataArray *junk;
        vtkDataSetAttributes *attributes;

        if (array_is_in_points)
        {
            junk = this->MetaDataSetList[i]->GetDataSet()->GetPointData()->GetArray (array->GetName());
            attributes = this->MetaDataSetList[i]->GetDataSet()->GetPointData();
        }
        else
        {
            junk = this->MetaDataSetList[i]->GetDataSet()->GetCellData()->GetArray (array->GetName());
            attributes = this->MetaDataSetList[i]->GetDataSet()->GetCellData();
        }

        if (!junk)
            continue;

        if (min > junk->GetRange()[0])
            min = junk->GetRange()[0];
        if (max < junk->GetRange()[1])
            max = junk->GetRange()[1];


        if (lut)
            junk->SetLookupTable (lut);

        attributes->SetActiveScalars(array->GetName());

        this->MetaDataSetList[i]->SetCurrentActiveArray (junk);
    }

    if (lut)
        lut->SetRange (min, max);


    if (array_is_in_points)
        this->DataSet->GetPointData()->SetActiveScalars (array->GetName());
    else
        this->DataSet->GetCellData()->SetActiveScalars (array->GetName());


    for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
    {
        vtkActor *actor = this->GetActor (i);
        if (!actor)
            continue;
        vtkMapper *mapper = actor->GetMapper();

        if (!array_is_in_points)
            mapper->SetScalarModeToUseCellFieldData();
        else
            mapper->SetScalarModeToUsePointFieldData();

        if (lut)
        {
            mapper->UseLookupTableScalarRangeOn();
        }

        mapper->SelectColorArray (array->GetName());
    }
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::SetScalarVisibility(bool val)
{
    this->Superclass::SetScalarVisibility (val);
    this->SetParseAttributes (val);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "name \t: " << this->GetName() << endl;
    os << indent << "delay \t: " << this->GetTimeResolution() << endl;
    os << indent << "duration \t: " << this->SequenceDuration << endl;
    os << indent << "type \t: " << this->Type << endl;
    os << indent << "number of items \t: " << this->GetNumberOfMetaDataSets() << endl;
}

//----------------------------------------------------------------------------
vtkDoubleArray*vtkMetaDataSetSequence::GenerateFollowerTimeTable(const char *arrayname, unsigned int idtofollow)
{
    vtkDoubleArray *ret = vtkDoubleArray::New();
    ret->Allocate(this->GetNumberOfMetaDataSets());

    ret->SetName (arrayname);

    unsigned int canfollow = this->GetNumberOfMetaDataSets();

    for (int i=0; i<this->GetNumberOfMetaDataSets(); i++)
    {
        double val;

        vtkDataArray *array = this->GetMetaDataSet (i)->GetArray (arrayname);

        if (!array)
        {
            ret->InsertNextValue (0);
            continue;
        }

        if ((int)idtofollow > array->GetNumberOfTuples())
        {
            canfollow--;
            ret->InsertNextValue (0);
            continue;
        }


        double *temp = array->GetTuple (idtofollow);
        if (temp)
            val = temp[0];
        else
            val = -1;

        ret->InsertNextValue (val);
    }

    if ((double)(ret->GetNumberOfTuples()) < (double)(this->GetNumberOfMetaDataSets())/2.0 )
    {
        vtkWarningMacro(<<"array "<<arrayname<<" not found in all sequence instances"<<endl);
        ret->Delete();
        return nullptr;
    }


    return ret;
}

//----------------------------------------------------------------------------
vtkDoubleArray*vtkMetaDataSetSequence::GenerateMetaDataTimeTable(const char *metadatakey)
{
    vtkDoubleArray *ret = vtkDoubleArray::New();
    ret->Allocate(this->GetNumberOfMetaDataSets());

    ret->SetName (metadatakey);

    for (int i=0; i<this->GetNumberOfMetaDataSets(); i++)
    {
        double val = 0.0;

        bool isvalid = this->GetMetaDataSet (i)->GetMetaData<double>(metadatakey, val);
        if (!isvalid)
        {
            vtkWarningMacro(<<"metadata "<<metadatakey<<" not found in all sequence frames"<<endl);
            ret->Delete();
            return nullptr;
        }
        ret->InsertNextValue (val);
    }

    return ret;
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::CopyInformation (vtkMetaDataSet *metadataset)
{
    this->Superclass::CopyInformation(metadataset);

    vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast (metadataset);

    if (!sequence)
        return;

    this->SameGeometryFlag = sequence->GetSameGeometryFlag();
    this->ParseAttributes = sequence->GetParseAttributes();
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ComputeTimesFromDuration()
{
    int nbDataSets = this->GetNumberOfMetaDataSets();
    // compute the time step between each data set
    double dt = this->GetSequenceDuration() / (nbDataSets - 1);
    for (int i = 0; i < nbDataSets; i++)
    {
        this->GetMetaDataSet(i)->SetTime(dt * i);
    }
}

void vtkMetaDataSetSequence::GetScalarRange(double range[2], QString attributeName)
{
    double localRange[2];
    range[0] = VTK_DOUBLE_MAX;
    range[1] = VTK_DOUBLE_MIN;

    for (auto* metaData : this->MetaDataSetList)
    {
        metaData->GetScalarRange(localRange, attributeName);

        if (range[0] > localRange[0])
        {
            range[0] = localRange[0];
        }
        if (range[1] < localRange[1])
        {
            range[1] = localRange[1];
        }
    }
}
