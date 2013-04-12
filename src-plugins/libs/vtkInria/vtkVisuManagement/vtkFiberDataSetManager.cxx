#include "vtkFiberDataSetManager.h"

#include "vtkObjectFactory.h"
#include "vtkFiberDataSet.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkFiberMapper.h"
#include "vtkTubeFilter.h"
#include "vtkRibbonFilter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

#include <map>

vtkCxxRevisionMacro(vtkFiberDataSetManager, "$Revision: 1 $");
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

/*
void vtkFiberDataSetManager::SetRenderWindowInteractor (vtkRenderWindowInteractor *iren)
{
  vtkFibersManager::SetRenderWindowInteractor (iren);
  
  if (iren && this->GetRenderer())
  {
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.begin();
    while (it!=d->FiberBundlePipelineList.end())
    {
      this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[(*it).first]->Actor );
      ++it;
    }
  }
}
*/

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

  /*
  vtkFiberDataSet::vtkFiberBundleListType bundles = this->FiberDataSet->GetBundleList();
  vtkFiberDataSet::vtkFiberBundleListType::iterator itb = bundles.begin();
  while (itb!=bundles.end())
  {
    d->FiberBundlePipelineList[(*itb).first] = new vtkFiberDataSetManagerPrivate::BundlePipeline;
    d->FiberBundlePipelineList[(*itb).first]->Actor->GetProperty()
      ->SetColor ( (*itb).second.Red, (*itb).second.Green, (*itb).second.Blue);
    if (this->GetRenderer())
    {
      this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[(*itb).first]->Actor );
    }
    ++itb;
  }
  */

  this->CreateBundleRenderingPipeline();
}

/*
void vtkFiberDataSetManager::RemoveAllActors()
{
  this->RemoveBundleActors();

  vtkFibersManager::RemoveAllActors();
}
*/

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

void vtkFiberDataSetManager::ChangeMapperToUseHardwareShaders()
{
  vtkFibersManager::ChangeMapperToUseHardwareShaders();
  this->CreateBundleRenderingPipeline();
}

void vtkFiberDataSetManager::ChangeMapperToDefault()
{
  vtkFibersManager::ChangeMapperToDefault();
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

void vtkFiberDataSetManager::CreateRenderingPipelineForBundle (const std::string &name)
{
  if (vtkPolyData *bundle = this->FiberDataSet->GetBundleList()[name].Bundle)
  {
    vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find (name);
    if (it==d->FiberBundlePipelineList.end())
    {
      // create rendering pipeline
      d->FiberBundlePipelineList[name] = new vtkFiberDataSetManagerPrivate::BundlePipeline;
      d->FiberBundlePipelineList[name]->Actor->GetProperty()->SetColor (this->FiberDataSet->GetBundleList()[name].Red,
									this->FiberDataSet->GetBundleList()[name].Green,
									this->FiberDataSet->GetBundleList()[name].Blue);
      if (this->GetRenderer())
      {
	this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[name]->Actor );
      }
    }
    
    vtkPolyDataMapper *mapper = 0;
#ifdef vtkINRIA3D_USE_HWSHADING
    if (vtkFibersManager::GetUseHardwareShaders())
    {
      vtkFiberMapper *fiberMapper = vtkFiberMapper::New();
      fiberMapper->SetAmbientContributionShadow(0.0);
      fiberMapper->SetDiffuseContributionShadow(0.6);
      fiberMapper->SetSpecularContributionShadow(0.0);
      fiberMapper->LightingOff();
      fiberMapper->ShadowingOff();
      mapper = fiberMapper;
    }
    else
#endif
    {
      mapper = vtkPolyDataMapper::New();
    }
    mapper->SetScalarModeToUseCellData();
    
    d->FiberBundlePipelineList[name]->Mapper->Delete();
    d->FiberBundlePipelineList[name]->Mapper = mapper;
    d->FiberBundlePipelineList[name]->Actor->SetMapper ( mapper );

    switch (this->GetRenderingMode())
    {    
	case RENDER_IS_TUBES:
#ifdef vtkINRIA3D_USE_HWSHADING
	  if (vtkFiberMapper* fiberMapper = vtkFiberMapper::SafeDownCast (mapper))
	  {
	    fiberMapper->LightingOn();
	    fiberMapper->ShadowingOn();
	    fiberMapper->SetInput (bundle);
	  }
	  else
#endif
	  {   
	    vtkTubeFilter *tubeFilter = d->FiberBundlePipelineList[name]->TubeFilter;
	    tubeFilter->SetRadius ( this->GetRadius() );
	    tubeFilter->SetNumberOfSides (4);
	    tubeFilter->UseDefaultNormalOff();
	    tubeFilter->CappingOn();
	    tubeFilter->SetInput ( bundle );
	    
	    mapper->SetInput ( tubeFilter->GetOutput() );
	  }
	  break;
	  
	case RENDER_IS_RIBBONS:
#ifdef vtkINRIA3D_USE_HWSHADING
	  if (vtkFiberMapper* fiberMapper = vtkFiberMapper::SafeDownCast (mapper))
	  {
	    fiberMapper->LightingOn();
	    fiberMapper->ShadowingOff();
	    fiberMapper->SetInput (bundle);
	  }
	  else
#endif
	  {
	    vtkRibbonFilter *ribbonFilter = d->FiberBundlePipelineList[name]->RibbonFilter;
	    ribbonFilter->SetWidth ( this->GetRadius() );
	    ribbonFilter->SetInput ( bundle );
	    
	    mapper->SetInput ( ribbonFilter->GetOutput() );
	  }
	  break;
	  
	case RENDER_IS_POLYLINES:	  
	default:
	  mapper->SetInput( bundle );
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

  // create rendering pipeline
  /*
  vtkFiberDataSetManagerPrivate::vtkFiberBundlePipelineListType::iterator it = d->FiberBundlePipelineList.find (name);
  if (it!=d->FiberBundlePipelineList.end())
  {
    if (this->GetRenderer())
    {
      this->GetRenderer()->RemoveViewProp ( d->FiberBundlePipelineList[name]->Actor );
    }
    delete d->FiberBundlePipelineList[name];
    d->FiberBundlePipelineList.erase (it);
  }
  
  d->FiberBundlePipelineList[name] = new vtkFiberDataSetManagerPrivate::BundlePipeline;
  
  d->FiberBundlePipelineList[name]->Actor->GetProperty()->SetColor (color[0], color[1], color[2]);
  */
  
  this->CreateRenderingPipelineForBundle (name);
  
  if (this->GetRenderer())
  {
    this->GetRenderer()->AddViewProp ( d->FiberBundlePipelineList[name]->Actor );
  }
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
