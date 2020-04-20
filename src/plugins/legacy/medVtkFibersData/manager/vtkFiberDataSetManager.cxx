/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2018. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "vtkFiberDataSetManager.h"

#include <vtkObjectFactory.h>
#include <vtkFiberDataSet.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTubeFilter.h>
#include <vtkRibbonFilter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <map>


vtkStandardNewMacro(vtkFiberDataSetManager);


class vtkFiberDataSetManagerPrivate
{
public:
  class BundlePipeline
  {
  public:
    vtkActor          *Actor;
    vtkTubeFilter     *TubeFilter;
    vtkRibbonFilter   *RibbonFilter;
    vtkPolyDataMapper *Mapper;
    BundlePipeline()
    {
      Actor        = vtkActor::New();
      TubeFilter   = vtkTubeFilter::New();
      RibbonFilter = vtkRibbonFilter::New();
      Mapper       = vtkPolyDataMapper::New();
    }
    ~BundlePipeline()
    {
      Actor->Delete();
      TubeFilter->Delete();
      RibbonFilter->Delete();
      Mapper->Delete();
    }
  };
  
  typedef std::map<std::string, BundlePipeline*> vtkFiberBundlePipelineListType;

  vtkFiberBundlePipelineListType FiberBundlePipelineList;

};


vtkFiberDataSetManager::vtkFiberDataSetManager(): d (new vtkFiberDataSetManagerPrivate)
{
  this->FiberDataSet = 0;
}

vtkFiberDataSetManager::~vtkFiberDataSetManager()
{
  if (this->FiberDataSet)
  {
    this->FiberDataSet->Delete();
  }

  this->RemoveBundleActors();
  vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
  while (it!=d->FiberBundlePipelineList.end())
  {
    delete d->FiberBundlePipelineList[ (*it).first ];
    ++it;
  }
  d->FiberBundlePipelineList.clear();

  delete d;
  d = NULL;
}

void vtkFiberDataSetManager::Enable()
{
  vtkFibersManager::Enable();
  this->AddBundleActors();
}

void vtkFiberDataSetManager::Disable()
{
  this->RemoveBundleActors();
  vtkFibersManager::Disable();
}

void vtkFiberDataSetManager::SetInput (vtkFiberDataSet *dataset)
{
  if (dataset!=this->FiberDataSet)
  {
    if (this->FiberDataSet)
    {
      this->FiberDataSet->UnRegister (this);
    }
    this->FiberDataSet = dataset;
    if (this->FiberDataSet)
    {
      this->FiberDataSet->Register(this);
    }
  }

  vtkFibersManager::SetInput ( this->FiberDataSet->GetFibers() );

  this->RemoveBundleActors();

  
  vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
  while (it!=d->FiberBundlePipelineList.end())
  {
    delete d->FiberBundlePipelineList[ (*it).first ];
    ++it;
  }
  d->FiberBundlePipelineList.clear();

  this->CreateBundleRenderingPipeline();
}


void vtkFiberDataSetManager::AddBundleActors()
{
  if (!this->GetRenderer())
    return;

  vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
  while (it!=d->FiberBundlePipelineList.end())
  {
    this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[(*it).first]->Actor );
    ++it;
  }
}

void vtkFiberDataSetManager::RemoveBundleActors()
{
  if (!this->GetRenderer())
    return;
  
  vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
  while (it!=d->FiberBundlePipelineList.end())
  {
    this->GetRenderer()->RemoveViewProp ( (*it).second->Actor );
    ++it;
  }
}

void vtkFiberDataSetManager::SetRenderingModeToPolyLines()
{
  vtkFibersManager::SetRenderingModeToPolyLines();
  this->CreateBundleRenderingPipeline();
}

void vtkFiberDataSetManager::SetRenderingModeToTubes()
{
  vtkFibersManager::SetRenderingModeToTubes();
  this->CreateBundleRenderingPipeline();
}

void vtkFiberDataSetManager::SetRenderingModeToRibbons()
{
  vtkFibersManager::SetRenderingModeToRibbons();
  this->CreateBundleRenderingPipeline();
}

void vtkFiberDataSetManager::CreateBundleRenderingPipeline()
{
  if (!this->FiberDataSet)
    return;

  vtkFiberDataSet::vtkFiberBundleListType bundles = this->FiberDataSet->GetBundleList();
  vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
  while (it!=bundles.end())
  {
    this->CreateRenderingPipelineForBundle ( (*it).first );
    ++it;
  }
}

void vtkFiberDataSetManager::CreateRenderingPipelineForBundle(const std::string &name)
{
    if (vtkPolyData *bundle = this->FiberDataSet->GetBundleList()[name].Bundle)
    {
        vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find(name);
        if (it == d->FiberBundlePipelineList.end())
        {
            // create rendering pipeline
            d->FiberBundlePipelineList[name] = new vtkFiberDataSetManagerPrivate::BundlePipeline;
            d->FiberBundlePipelineList[name]->Actor->GetProperty()->SetColor(this->FiberDataSet->GetBundleList()[name].Red, this->FiberDataSet->GetBundleList()[name].Green, this->FiberDataSet->GetBundleList()[name].Blue);
            if (this->GetRenderer())
            {
                this->GetRenderer()->AddViewProp(d->FiberBundlePipelineList[name]->Actor);
            }
        }

        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();

        mapper->SetScalarModeToUseCellData();

        d->FiberBundlePipelineList[name]->Mapper->Delete();
        d->FiberBundlePipelineList[name]->Mapper = mapper;
        d->FiberBundlePipelineList[name]->Actor->SetMapper(mapper);

        switch (this->GetRenderingMode())
        {
        case RENDER_IS_TUBES:
            {
                vtkTubeFilter *tubeFilter = d->FiberBundlePipelineList[name]->TubeFilter;
                tubeFilter->SetRadius(this->GetRadius());
                tubeFilter->SetNumberOfSides(4);
                tubeFilter->UseDefaultNormalOff();
                tubeFilter->CappingOn();
                tubeFilter->SetInputData(bundle);
            
                mapper->SetInputConnection(tubeFilter->GetOutputPort());
                break;
            }

        case RENDER_IS_RIBBONS:
            {
                vtkRibbonFilter *ribbonFilter = d->FiberBundlePipelineList[name]->RibbonFilter;
                ribbonFilter->SetWidth(this->GetRadius());
                ribbonFilter->SetInputData(bundle);
            
                mapper->SetInputConnection(ribbonFilter->GetOutputPort());
                break;
            }

        case RENDER_IS_POLYLINES:
        default:
            mapper->SetInputData(bundle);
            break;
        }
    }
}

void vtkFiberDataSetManager::Validate (const std::string &name, double color[3])
{
  if (!this->FiberDataSet || !this->FiberDataSet->GetFibers())
  {
    vtkErrorMacro ( <<"Cannot validate bundle if input is not set");
    return;
  }

  vtkPolyData *bundle = vtkPolyData::New();
  bundle->Initialize();
  bundle->SetPoints ( this->FiberDataSet->GetFibers()->GetPoints() );
  bundle->GetPointData()->SetScalars ( this->FiberDataSet->GetFibers()->GetPointData()->GetScalars() );
  for( int i=0; i<this->FiberDataSet->GetFibers()->GetPointData()->GetNumberOfArrays(); i++)
  {
    bundle->GetPointData()->AddArray ( this->FiberDataSet->GetFibers()->GetPointData()->GetArray (i) );
  }
  bundle->SetLines ( this->GetSelectedCells() );

  this->FiberDataSet->AddBundle (name, bundle, color);

  bundle->Delete();
    
  this->CreateRenderingPipelineForBundle (name);
  
  if (this->GetRenderer())
  {
    this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[name]->Actor );
  }
}

void vtkFiberDataSetManager::RemoveBundle(const std::string &name)
{
    if (this->GetRenderer())
        this->GetRenderer()->RemoveViewProp ( d->FiberBundlePipelineList[name]->Actor );
    
    if (this->FiberDataSet)
        this->FiberDataSet->RemoveBundle(name);
    
    d->FiberBundlePipelineList.erase(name);
}

void vtkFiberDataSetManager::ChangeBundleName(const std::string &oldName, const std::string &newName)
{
    if (this->FiberDataSet)
        this->FiberDataSet->ChangeBundleName(oldName,newName);
    
    d->FiberBundlePipelineList[newName] = d->FiberBundlePipelineList[oldName];
    d->FiberBundlePipelineList.erase(oldName);
}

void vtkFiberDataSetManager::SetBundleVisibility(const std::string &name, int visibility)
{
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find (name);
    if (it!=d->FiberBundlePipelineList.end())
    {
        (it->second)->Actor->SetVisibility(visibility);
    }
}

int vtkFiberDataSetManager::GetBundleVisibility(const std::string &name)
{
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find (name);
    if (it!=d->FiberBundlePipelineList.end())
    {
        return (it->second)->Actor->GetVisibility();
    }
    return 0;
}

vtkActor *vtkFiberDataSetManager::GetBundleActor(const std::string &name)
{
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find (name);
    if (it!=d->FiberBundlePipelineList.end())
    {
        return (it->second)->Actor;
    }
    return NULL;
}

void vtkFiberDataSetManager::ShowAllBundles()
{
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
    while (it!=d->FiberBundlePipelineList.end())
    {
        it->second->Actor->SetVisibility(1);
        ++it;
    }
}

void vtkFiberDataSetManager::HideAllBundles()
{
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
    while (it!=d->FiberBundlePipelineList.end())
    {
        it->second->Actor->SetVisibility(0);
        ++it;
    }
}
