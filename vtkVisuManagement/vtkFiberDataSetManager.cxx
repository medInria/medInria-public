#include "vtkFiberDataSetManager.h"

#include "vtkObjectFactory.h"
#include "vtkFiberDataSet.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkFiberMapper.h"
#include "vtkTubeFilter.h"
#include "vtkRibbonFilter.h"

vtkCxxRevisionMacro(vtkFiberDataSetManager, "$Revision: 1 $");
vtkStandardNewMacro(vtkFiberDataSetManager);


vtkFiberDataSetManager::vtkFiberDataSetManager()
{
  this->FiberDataSet = 0;
}

vtkFiberDataSetManager::~vtkFiberDataSetManager()
{
  if (this->FiberDataSet)
  {
    this->FiberDataSet->Delete();
  }
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

  this->FiberBundleActorList.clear();

  vtkFiberDataSet::vtkFiberBundleListType bundles = this->FiberDataSet->GetBundleList();
  vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
  while (it!=bundles.end())
  {
    std::cout << "Name: " << (*it).first << std::endl;
    this->FiberBundleActorList[(*it).first] = vtkActor::New();
    this->FiberBundleActorList[(*it).first]->GetProperty()
      ->SetColor ( (*it).second.Red, (*it).second.Green, (*it).second.Blue);
    if (this->GetRenderer())
    {
      this->GetRenderer()->AddViewProp ( this->FiberBundleActorList[(*it).first] );
    }
    ++it;
  }  
}

void vtkFiberDataSetManager::RemoveAllActors()
{
  vtkFiberDataSetManager::vtkFiberBundleActorListType::iterator it = this->FiberBundleActorList.begin();
  while (it!=this->FiberBundleActorList.end())
  {
    this->GetRenderer()->RemoveViewProp ( (*it).second );
    ++it;
  }

  vtkFibersManager::RemoveAllActors();
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
  
  vtkFiberDataSet::vtkFiberBundleListType::iterator it = this->FiberDataSet->GetBundleList().begin();
  while (it!=this->FiberDataSet->GetBundleList().end())
  {
    vtkPolyData *bundle = (*it).second.Bundle;
    if (bundle)
    {
#ifdef vtkINRIA3D_USE_HWSHADING
      if (vtkFibersManager::GetUseHardwareShaders())
      {
	vtkFiberMapper* mapper = vtkFiberMapper::New();
	mapper->SetAmbientContributionShadow(0.0);
	mapper->SetDiffuseContributionShadow(0.6);
	mapper->SetSpecularContributionShadow(0.0);
	mapper->LightingOff();
	mapper->ShadowingOff();
	mapper->SetInput( bundle );

	if (this->FiberBundleActorList[(*it).first])
	{
	  this->FiberBundleActorList[(*it).first]->SetMapper( mapper );
	}
      }
      else
#endif
	switch (vtkFibersManager::GetRenderingMode())
	{    
	    case RENDER_IS_TUBES:
	      {
		vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
		tubeFilter->SetRadius ( this->GetRadius() );
		tubeFilter->SetNumberOfSides (4);
		tubeFilter->UseDefaultNormalOff();
		tubeFilter->CappingOn();
		tubeFilter->SetInput ( bundle );
		bundle = tubeFilter->GetOutput();
	      }
	      break;
	      
	    case RENDER_IS_RIBBONS:
	      {
		vtkRibbonFilter *ribbonFilter = vtkRibbonFilter::New();
		ribbonFilter->SetWidth ( this->GetRadius() );
		ribbonFilter->SetInput ( bundle );
		bundle = ribbonFilter->GetOutput();
	      }
	      break;
	      
	    case RENDER_IS_POLYLINES:
	    default:
	      break;
	}

      vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
      mapper->SetInput ( bundle );
      if (this->FiberBundleActorList[(*it).first])
      {
	this->FiberBundleActorList[(*it).first]->SetMapper( mapper );
      }
    }
    ++it;
  }
}
