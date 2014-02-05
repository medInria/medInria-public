/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkFiberDataSet.h"

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkInformation.h>

vtkCxxRevisionMacro(vtkFiberDataSet, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkFiberDataSet);

vtkFiberDataSet::vtkFiberDataSet()
{
}

vtkFiberDataSet::~vtkFiberDataSet()
{
  /*
  vtkFiberBundleListType::iterator it = this->Bundles.begin();
  while (it!=this->Bundles.end())
  {
    (*it).second.Bundle->Delete();
  }
  */
}

void vtkFiberDataSet::SetFibers (vtkPolyData *fibers)
{
  this->SetBlock (0, fibers);
}

vtkPolyData *vtkFiberDataSet::GetFibers()
{
  return vtkPolyData::SafeDownCast ( this->GetBlock (0) );
}

void vtkFiberDataSet::AddBundle (const std::string &name, vtkPolyData *bundle, double color[3])
{
  if (name=="" || !bundle)
  {
    return;
  }

  int id = this->Bundles[name].Id;

  if (id==-1)
  {
    int childCount = this->GetNumberOfChildren();
    id = childCount==0?1:childCount;
    this->Bundles[name].Id = id;
  }

  this->SetBlock (id, bundle);

  this->Bundles[name].Bundle = bundle;
  this->Bundles[name].Red    = color[0];
  this->Bundles[name].Green  = color[1];
  this->Bundles[name].Blue   = color[2];

  /*
  if (vtkInformation *info = this->GetMetaData (id))
  {
    info->AppendUnique ("Name", name.c_str());
  }
  */
}

void vtkFiberDataSet::ChangeBundleName (const std::string &oldName, const std::string &newName)
{
    if (oldName == newName)
        return;
    
    vtkFiberBundleListType::iterator it = this->Bundles.find (newName);
    if (it!=this->Bundles.end())
        return;
    
    this->Bundles[newName] = this->Bundles[oldName];
    it = this->Bundles.find (oldName);
    
    this->Bundles.erase(it);
}

void vtkFiberDataSet::RemoveBundle (const std::string &name)
{
  vtkFiberBundleListType::iterator it = this->Bundles.find (name);
  if (it!=this->Bundles.end())
  {
    int id = (*it).second.Id;
    this->Bundles.erase (it);
    this->RemoveBlock (id);
  }
}

void vtkFiberDataSet::Clear()
{
  vtkFiberBundleListType::iterator it = this->Bundles.begin();
  while (it!=this->Bundles.end())
  {
    this->RemoveBlock ( (*it).second.Id );
    ++it;
  }
  this->Bundles.clear();
}

int vtkFiberDataSet::GetNumberOfBundles() const
{
  return (int)this->Bundles.size();
}

vtkFiberDataSet::vtkFiberBundleType vtkFiberDataSet::GetBundle (const std::string &name)
{
  return this->Bundles[name];
}

vtkFiberDataSet::vtkFiberBundleListType vtkFiberDataSet::GetBundleList() const
{
  return this->Bundles;
}
    
void vtkFiberDataSet::SetBundleColor (const std::string &name, double color[3])
{
  vtkFiberBundleListType::iterator it = this->Bundles.find (name);
  if (it==this->Bundles.end())
      return;
  
  this->Bundles[name].Red   = color[0];
  this->Bundles[name].Green = color[1];
  this->Bundles[name].Blue  = color[2];
}

void vtkFiberDataSet::GetBundleColor (const std::string &name, double color[3])
{
  vtkFiberBundleListType::iterator it = this->Bundles.find (name);
  if (it==this->Bundles.end())
      return;
  
  color[0] = this->Bundles[name].Red;
  color[1] = this->Bundles[name].Green;
  color[2] = this->Bundles[name].Blue;
}

 
