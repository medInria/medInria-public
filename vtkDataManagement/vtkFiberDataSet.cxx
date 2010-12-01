#include "vtkFiberDataSet.h"

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkFiberDataSet, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkFiberDataSet);

vtkFiberDataSet::vtkFiberDataSet()
{
  this->Fibers = 0;
}

vtkFiberDataSet::~vtkFiberDataSet()
{
  if (this->Fibers)
  {
    this->Fibers->Delete();
  }

  vtkFiberBundleListType::iterator it = this->Bundles.begin();
  while (it!=this->Bundles.end())
  {
    (*it).second.Bundle->Delete();
  }
}

void vtkFiberDataSet::AddBundle (const std::string &name, vtkPolyData *bundle, double color[3])
{
  if (name=="" || !bundle)
  {
    return;
  }
  
  if (bundle!=this->Bundles[name].Bundle)
  {
    if (this->Bundles[name].Bundle)
    {
      this->Bundles[name].Bundle->UnRegister (this);
    }
    this->Bundles[name].Bundle = bundle;
    if (this->Bundles[name].Bundle)
    {
      this->Bundles[name].Bundle->Register (this);
    }
  }
  
  this->Bundles[name].Red   = color[0];
  this->Bundles[name].Green = color[1];
  this->Bundles[name].Blue  = color[2];
}

void vtkFiberDataSet::RemoveBundle (const std::string &name)
{
  vtkFiberBundleListType::iterator it = this->Bundles.find (name);
  if (it!=this->Bundles.end())
  {
    (*it).second.Bundle->Delete();
    this->Bundles.erase (it);
  }
}

void vtkFiberDataSet::Clear (void)
{
  vtkFiberBundleListType::iterator it = this->Bundles.begin();
  while (it!=this->Bundles.end())
  {
    (*it).second.Bundle->Delete();
  }
  this->Bundles.clear();
}

int vtkFiberDataSet::GetNumberOfBundles (void) const
{
  return (int)this->Bundles.size();
}

vtkFiberDataSet::vtkFiberBundleType vtkFiberDataSet::GetBundle (const std::string &name)
{
  return this->Bundles[name];
}

vtkFiberDataSet::vtkFiberBundleListType vtkFiberDataSet::GetBundleList (void) const
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

 
