/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkMetaDataSet.cxx 1298 2009-10-22 12:58:07Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-22 14:58:07 +0200 (Thu, 22 Oct 2009) $
Version:   $Revision: 1298 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkMetaDataSet.h>
#include "vtkObjectFactory.h"
#include <fstream>
#include <sstream>

#include <vtkDataSet.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>
#include <vtkMapper.h>

#include <vtkActorCollection.h>
#include <vtkActor.h>
#include <vtkScalarsToColors.h>
#include <vtksys/SystemTools.hxx>
#include <vtkErrorCode.h>
#include <vtkLookupTable.h>
#include <vtkDataArrayCollection.h>
#include <vtkCell.h>

#include <vtkPolyData.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaDataSet );
vtkCxxRevisionMacro( vtkMetaDataSet, "$Revision: 1298 $");

//----------------------------------------------------------------------------
vtkMetaDataSet::vtkMetaDataSet()
{
  this->DataSet          = 0;
  this->WirePolyData = NULL;

  this->ActorList = vtkActorCollection::New();
  this->ArrayCollection = vtkDataArrayCollection::New();
  this->CurrentScalarArray = 0;
  
  this->Time             = -1;  
  this->Property         = 0;
  this->Type             = vtkMetaDataSet::VTK_META_UNKNOWN;
  this->PickedPointId    = -1;
  this->PickedCellId     = -1;
  this->Name             = "";
  this->FilePath         = "";
  this->Initialize();
  
}

//----------------------------------------------------------------------------
vtkMetaDataSet::~vtkMetaDataSet()
{  
  vtkDebugMacro(<<"deleting vtkMetaDataSet "<<this->GetName());
  
  if (this->DataSet)
  {
    this->DataSet->Delete();
  }
  if (this->WirePolyData)
  {
    this->WirePolyData->Delete();
  }

  if (this->Property)
    this->Property->Delete();
  
  this->ActorList->Delete();
  this->ArrayCollection->Delete();
  
  
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::Initialize()
{  
  this->ActorList->RemoveAllItems();
//   if (this->DataSet)
//     this->DataSet->GetPointData()->CopyScalarsOn();
}
			    
//----------------------------------------------------------------------------
const char* vtkMetaDataSet::GetName() const
{ return this->Name.c_str(); }
//----------------------------------------------------------------------------
void vtkMetaDataSet::SetName (const char* name)
{ this->Name = name; }
//----------------------------------------------------------------------------
const char* vtkMetaDataSet::GetTag() const
{ return this->Tag.c_str(); }
//----------------------------------------------------------------------------
void vtkMetaDataSet::SetTag (const char* tag)
{ this->Tag = tag; }
//----------------------------------------------------------------------------
const char* vtkMetaDataSet::GetFilePath() const
{ return this->FilePath.c_str(); }
//----------------------------------------------------------------------------
void vtkMetaDataSet::SetFilePath (const char* path)
{ this->FilePath = path; }


//----------------------------------------------------------------------------
void vtkMetaDataSet::SetDataSet (vtkDataSet* dataset)
{
  if (this->DataSet == dataset)
  {
    return;
  }
  if (this->DataSet)
  {
    this->DataSet->UnRegister(this);
  }
  vtkDataSet* newdataset = dataset->NewInstance();
  newdataset->DeepCopy (dataset);
  
  this->DataSet = newdataset;
  
  if (this->DataSet)
  {
    this->DataSet->Register(this);
  }

  if (this->DataSet)
  {
    //this->DataSet->GetPointData()->CopyAllOn();
    
    this->Initialize();
  }

  newdataset->Delete();
  
  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::SetWirePolyData (vtkPolyData* dataset)
{
  if (this->WirePolyData == dataset)
  {
    return;
  }
  if (this->WirePolyData)
  {
    this->WirePolyData->UnRegister(this);
  }
  this->WirePolyData = dataset;
  
  if (this->WirePolyData)
  {
    this->WirePolyData->Register(this);
  }

  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::AddActor(vtkActor* actor)
{
  
  if (actor && !this->HasActor (actor))
    this->ActorList->AddItem(actor);
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::RemoveActor(vtkActor* actor)
{
  this->ActorList->RemoveItem(actor);
}

//----------------------------------------------------------------------------
bool vtkMetaDataSet::HasActor (vtkActor* actor)
{
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    if (this->GetActor (i) == actor)
      return true;
  }
  return false;
  
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::RemoveAllActors()
{
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  { this->ActorList->RemoveItem (i); }
}

//----------------------------------------------------------------------------
vtkActor* vtkMetaDataSet::GetActor(unsigned int i)
{
  return vtkActor::SafeDownCast (this->ActorList->GetItemAsObject(i));
}

//----------------------------------------------------------------------------
unsigned int vtkMetaDataSet::GetNumberOfActors() const
{
  return this->ActorList->GetNumberOfItems();
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::SetVisibility (bool visible)
{
  for (unsigned int i=0; i<this->GetNumberOfActors(); i++)
    this->SetActorVisibility (i, visible);
}

//----------------------------------------------------------------------------
int vtkMetaDataSet::GetVisibility()
{
  bool ret = false;
  for (unsigned int i=0; i<this->GetNumberOfActors(); i++)
  {
    if (this->GetActorVisibility(i))
    {
      ret = true;
      break;
    }
  }
  return ret;
}
//----------------------------------------------------------------------------
void vtkMetaDataSet::SetActorVisibility (unsigned int it, bool visible)
{
  vtkActor* actor = this->GetActor (it);
  if (actor)
    actor->SetVisibility (visible);
}
//----------------------------------------------------------------------------
int vtkMetaDataSet::GetActorVisibility (unsigned int it)
{
  vtkActor* actor = this->GetActor (it);
  if (actor)
    return actor->GetVisibility ();
  return false;
}


void vtkMetaDataSet::ReadData (const char* filename)
{
  if (!this->GetDataSet())
    return;

  std::ifstream file (filename );
  
  if(file.fail())
  {
    vtkWarningMacro(<<"File not found"<<endl);
    throw vtkErrorCode::FileNotFoundError;
  }

  
  char t_keyword[256];
  file >> t_keyword;
  file.close();
  std::string  keyword = vtksys::SystemTools::LowerCase (t_keyword);
//   std::cout<<"reading data : "<<keyword.c_str()<<std::endl;

  try
  {
    
    if (strcmp (keyword.c_str(), "position")==0)
    {
      this->ReadPosition (filename);
    }
    else
    {
      this->ReadDataInternal (filename);
    }
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }

  
}

void vtkMetaDataSet::WriteData (const char* filename, const char* dataname)
{
  if (!this->GetDataSet())
    return;

  try
  {
    std::ofstream file (filename );
  
    if(file.fail())
    {
      vtkWarningMacro(<<"File not found"<<endl);
      throw vtkErrorCode::FileNotFoundError;
    }
    
    vtkDataArray* array = this->GetArray (dataname);
    if (!array)
    {
      vtkWarningMacro(<<"Data not found"<<endl);
      throw vtkErrorCode::FileNotFoundError;
    }


    vtkWarningMacro (<<"Not available yet..."<<endl);
    throw vtkErrorCode::UserError;
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }
}



void vtkMetaDataSet::ReadPosition(const char* filename)
{
    
  std::ifstream file (filename );
  
  if(file.fail())
  {
    vtkWarningMacro(<<"File not found"<<endl);
    file.close();
    throw vtkErrorCode::FileNotFoundError;
  }

  vtkPointSet* pointset = vtkPointSet::SafeDownCast (this->GetDataSet());

  if (!pointset)
  {
    vtkWarningMacro(<<"this DataSet dows not handle this type of attributes"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }

  char t_keyword[256];
  file >> t_keyword;
  std::string keyword = t_keyword;

  unsigned int type = 0;
  file >> type;

//   std::cout<<"type "<<type<<std::endl;
  
  unsigned long Nb = 0;
  unsigned int Dim = 0;
  // should get line twice
  // one for returning to the next line (end of line)
  // second to actually get the line
  std::string line = "";
  vtksys::SystemTools::GetLineFromStream(file, line);
  vtksys::SystemTools::GetLineFromStream(file, line);
  std::istringstream is;
  is.str (line);
  is >> Nb >> Dim;
//   std::cout<<"Nb "<<Nb<<", Dim "<<Dim<<std::endl;
  
  if (Dim != 3)
  {
    vtkWarningMacro(<<"Wrong dimension for this type of data"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }
  
  unsigned long NPoints = pointset->GetPoints()->GetNumberOfPoints();
  if (Nb < NPoints)
  {
    vtkWarningMacro(<<"Cannot assign these data : does not match the number of points in dataset"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }  
  
  vtkPoints* points = pointset->GetPoints();
  unsigned long i=0;  
  double position[3] = {0,0,0};
  double t = 0;

  while(!file.fail() && (long)i<points->GetNumberOfPoints())
  {
    
    file >> t;
    position[0] = t;
    file >> t;
    position[1] = t;
    file >> t;
    position[2] = t;
    points->SetPoint (i, position[0], position[1], position[2]);    
    i++;
  }
  
  file.close();  
}



void vtkMetaDataSet::ReadDataInternal(const char* filename)
{
  
  std::ifstream file (filename );
  
  if(file.fail())
  {
    vtkWarningMacro(<<"File not found"<<endl);
    file.close();
    throw vtkErrorCode::FileNotFoundError;
  }

  vtkPointSet* pointset = vtkPointSet::SafeDownCast (this->GetDataSet());

  if (!pointset)
  {
    vtkWarningMacro(<<"this DataSet dows not handle this type of attributes"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }
  

  char t_keyword[256];
  file >> t_keyword;
  std::string keyword = t_keyword;

  
  unsigned int type = 0;
  file >> type;

//   std::cout<<"type "<<type<<std::endl;
  
  unsigned long Nb = 0;
  unsigned int Dim = 0;
  // should get line twice
  // one for returning to the next line
  // second to actually ge the line
  std::string line = "";
  vtksys::SystemTools::GetLineFromStream(file, line);
  vtksys::SystemTools::GetLineFromStream(file, line);
  std::istringstream is;
  is.str (line);
  is >> Nb >> Dim;
//   std::cout<<"Nb "<<Nb<<", Dim "<<Dim<<std::endl;

  if ((Dim < 1) || Nb < 1)
  {
    vtkWarningMacro(<<"Wrong dimension for this type of data"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }

  if ((type == 1) && (long)Nb <  pointset->GetNumberOfPoints())
  {
    vtkWarningMacro(<<"Number of points ("<<pointset->GetNumberOfPoints()<<") don't match the data ("<<Nb<<")"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }

  if ((type != 1) && (long)Nb <  pointset->GetNumberOfCells())
  {
    vtkWarningMacro(<<"Number of cells ("<<pointset->GetNumberOfCells()<<") don't match the data ("<<Nb<<")"<<endl);
    file.close();
    throw vtkErrorCode::UserError;
  }

  
  vtkFloatArray* array = vtkFloatArray::New();
  array->SetName (keyword.c_str());
  array->Allocate(Nb);
  array->SetNumberOfComponents(Dim);
  
  unsigned long i=0;
  
  float* tuple = new float[Dim];
  
  while(!file.fail() && i<Nb)
  {
    
    for (unsigned int t=0; t<Dim; t++)
      file >> tuple[t];
    
    array->InsertNextTupleValue (tuple);
    i++;
    
  }
  delete [] tuple;
  
  file.close();

  vtkDataSetAttributes* attributes = NULL;
  
  if (type == 1) // assign array to points
  {
    attributes = this->GetDataSet()->GetPointData();
  }
  else // assign array to cells
  {
    attributes = this->GetDataSet()->GetCellData();
  }

  
  attributes->AddArray (array);

  if (Dim == 3)
    attributes->SetActiveVectors (array->GetName());
  if (Dim == 6)
    attributes->SetActiveTensors (array->GetName());

  array->Delete();
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::Read (const char* filename)
{
  vtkErrorMacro("not implemented here");
  throw vtkErrorCode::UserError;
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::Write (const char* filename)
{
  vtkErrorMacro("not implemented here");
  throw vtkErrorCode::UserError;
}


void vtkMetaDataSet::LinkFilters()
{
  
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::CopyInformation (vtkMetaDataSet* metadataset)
{
  if (!metadataset)
    return;

  this->SetName (metadataset->GetName());
  this->SetTime (metadataset->GetTime());
  this->SetFilePath (metadataset->GetFilePath());

  //this->SetProperty (metadataset->GetProperty());

  //add actors ??
  this->SetTag(metadataset->GetTag());

  
  for (int i=0; i<metadataset->GetArrayCollection()->GetNumberOfItems(); i++)
  {
    this->ArrayCollection->AddItem (vtkDataArray::SafeDownCast (metadataset->GetArrayCollection()->GetItemAsObject (i)));
  }
  
}


//----------------------------------------------------------------------------
double* vtkMetaDataSet::GetCurrentScalarRange()
{
  double* val = new double[2];
  val[0] = VTK_DOUBLE_MAX;
  val[1] = VTK_DOUBLE_MIN;
  
  if (this->GetCurrentScalarArray())
  {
    double* range2 = this->GetCurrentScalarArray()->GetRange ();
    val[0] = range2[0];
    val[1] = range2[1];
  }

  else if (this->GetDataSet() && ( val[0] == VTK_DOUBLE_MAX ) )
    val = this->GetDataSet()->GetScalarRange();
    

  return val;
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::ColorByArray(vtkDataArray* array)
{
  this->CurrentScalarArray = array;
  
  if (!array)
    return;
  
  bool array_is_in_points = false;

  if (this->DataSet->GetPointData()->HasArray (array->GetName()))
      array_is_in_points = true;
  
  double min = 0, max = 0;
  
  vtkDataArray* junk;
  vtkDataSetAttributes* attributes;
  
  if (array_is_in_points)
  {
    junk = this->GetDataSet()->GetPointData()->GetArray (array->GetName());
    attributes = this->GetDataSet()->GetPointData();
  }
  else
  {  
    junk = this->GetDataSet()->GetCellData()->GetArray (array->GetName());
    attributes = this->GetDataSet()->GetCellData();
  }
  
  if (!junk)
    return;
  
  if (min > junk->GetRange()[0])
    min = junk->GetRange()[0];
  if (max < junk->GetRange()[1])
    max = junk->GetRange()[1];
  
  vtkLookupTable* lut = array->GetLookupTable();
  if (lut)
  {
    lut->SetRange (min, max);
    junk->SetLookupTable (lut);

    double values[4];
    for(int i = 0; i < lut->GetNumberOfTableValues(); i++)
    {
      lut->GetTableValue(i, values);
      values[3] = 1.0;
      lut->SetTableValue(i, values);
    }
  }
  
  attributes->SetActiveScalars(array->GetName());
  
  
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = this->GetActor (i);
    if (!actor)
      continue;
    vtkMapper* mapper = actor->GetMapper();
    
    if (!array_is_in_points)
      mapper->SetScalarModeToUseCellFieldData();
    else
      mapper->SetScalarModeToUsePointFieldData();    
    
    if (lut)
    {
      // mapper->SetLookupTable (lut);
      mapper->UseLookupTableScalarRangeOn();
    }
    
    // mapper->SetScalarRange (min, max);
    mapper->SelectColorArray (array->GetName());
  }


}


//----------------------------------------------------------------------------
bool vtkMetaDataSet::GetScalarVisibility()
{
  if (this->GetNumberOfActors() && this->GetActor (0))
    return this->GetActor (0)->GetMapper()->GetScalarVisibility();
  return false;
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::SetScalarVisibility(bool val)
{
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = this->GetActor (i);
    if (!actor)
      continue;
    vtkMapper* mapper = actor->GetMapper();
    mapper->SetScalarVisibility(val);
  }
}

//----------------------------------------------------------------------------
void vtkMetaDataSet::ScalarVisibilityOn()
{
  this->SetScalarVisibility (true);
}
//----------------------------------------------------------------------------
void vtkMetaDataSet::ScalarVisibilityOff()
{
  this->SetScalarVisibility (false);
}




//----------------------------------------------------------------------------
void vtkMetaDataSet::GetColorArrayCollection(vtkDataArrayCollection* collection)
{
  if (!this->GetDataSet())
    return;

  std::cout<<"dataset has "<<this->GetDataSet()->GetPointData()->GetNumberOfArrays()<<" pointdata arrays..."<<std::endl;
  if (this->GetDataSet()->GetPointData()->GetScalars())
    std::cout<<"dataset has pointdata scalars..."<<std::endl;
  
  for (int i=0; i<this->GetDataSet()->GetPointData()->GetNumberOfArrays(); i++)
  {
    collection->AddItem(this->GetDataSet()->GetPointData()->GetArray (i));
  }
  for (int i=0; i<this->GetDataSet()->GetCellData()->GetNumberOfArrays(); i++)
  {
    collection->AddItem(this->GetDataSet()->GetCellData()->GetArray (i));    
  }
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::AddArray(vtkDataArray* array)
{
  this->ArrayCollection->AddItem(array);
}


//----------------------------------------------------------------------------
vtkDataArray* vtkMetaDataSet::GetArray (const char* name)
{

  // first try in the MetaDataSet array collection
  for (int i=0; i<this->ArrayCollection->GetNumberOfItems(); i++)
  {
    if (this->ArrayCollection->GetItem (i)->GetName() && (strcmp(this->ArrayCollection->GetItem (i)->GetName(), name) == 0))
    {
      return this->ArrayCollection->GetItem (i);
    }
  }

  

  // then try in the pointdata and celldata array collections
  vtkDataArray* ret = NULL;

  vtkDataArrayCollection* arrays = vtkDataArrayCollection::New();
  this->GetColorArrayCollection (arrays);
  
  for (int i=0; i<arrays->GetNumberOfItems(); i++)
  {
    std::cout<<"comparing name : "<<name<<" VS "<<arrays->GetItem (i)->GetName()<<std::endl;
    
    if (arrays->GetItem (i)->GetName() && (strcmp(arrays->GetItem (i)->GetName(), name) == 0))
    {
      ret = arrays->GetItem (i);
      break;
    }
  }

  arrays->Delete();
  return ret;
}


//----------------------------------------------------------------------------
void vtkMetaDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Name \t: " << this->Name << endl;
  os << indent << "DataSet \t: " << (*this->DataSet) << endl;  
}



// //----------------------------------------------------------------------------
// template <class type>
// inline void vtkMetaDataSet::SetMetaData (std::string key, type value)
// {
//   if (this->MetaDataDictionary.HasKey (key))
//     itk::EncapsulateMetaData<type>(this->MetaDataDictionary, key, (type)value);
//   else
//     itk::EncapsulateMetaData<type>(this->MetaDataDictionary, key, (type)value);
// }

// //----------------------------------------------------------------------------
// template <class type>
// inline bool vtkMetaDataSet::GetMetaData (std::string key, type &ret)
// {

//   type toret;
//   bool valid = itk::ExposeMetaData<type>(this->MetaDataDictionary, key, toret);
//   if(!valid)
//     return false;
//   ret = toret;
//   return true;
// }


//template <unsigned int> bool vtkMetaDataSet::GetMetaData (std::string key, unsigned int &ret);

