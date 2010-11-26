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
}

void vtkFiberDataSet::AddBundle (const std::string &name, vtkPolyData *bundle, double color[3])
{
  if (!bundle)
  {
    return;
  }

  vtkFiberDataSet::vtkFiberBundleType p_bundle;
  p_bundle.Bundle = bundle;
  p_bundle.Red    = color[0];
  p_bundle.Green  = color[1];
  p_bundle.Blue   = color[2];

  this->AddBundle (name, p_bundle);
}

void vtkFiberDataSet::AddBundle (const std::string &name, const vtkFiberBundleType &bundle)
{
  if (name=="")
  {
    return;
  }

  this->Bundles[name] = bundle;
}

void vtkFiberDataSet::RemoveBundle (const std::string &name)
{
  this->Bundles.erase (name);
}

void vtkFiberDataSet::Clear (void)
{
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
  this->Bundles[name].Red   = color[0];
  this->Bundles[name].Green = color[1];
  this->Bundles[name].Blue  = color[2];
}

void vtkFiberDataSet::GetBundleColor (const std::string &name, double color[3])
{
  color[0] = this->Bundles[name].Red;
  color[1] = this->Bundles[name].Green;
  color[2] = this->Bundles[name].Blue;
}

 
