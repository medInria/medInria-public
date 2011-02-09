#include "v3dFiberBundle.h"

#include <vtkObjectFactory.h>
#include <vector>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkGenericCell.h>

#include "vtkINRIA3DConfigure.h"

vtkCxxRevisionMacro(v3dFiberBundle, "$Revision: 1.4 $");
vtkStandardNewMacro(v3dFiberBundle);


v3dFiberBundle::v3dFiberBundle()
{
  this->Name = "NotSet";
  this->Color[0] = 1.0;
  this->Color[1] = 0.0;
  this->Color[2] = 0.0;
  this->Points = 0;
  this->Cells = 0;
  this->FiberColors = 0;
  this->Bundle = vtkPolyData::New();
  this->Squeezer = vtkSqueezeFibers::New();
  this->TubeFilter = vtkTubeFilter::New();
  this->RibbonFilter = vtkRibbonFilter::New();
  this->Mapper = vtkPolyDataMapper::New();
  this->Actor = vtkActor::New();

  this->PolyData = 0;
  
  this->TubeFilter->SetInput (this->Squeezer->GetOutput());
  this->TubeFilter->SetRadius (0.15);
  this->TubeFilter->SetNumberOfSides (4);
  this->TubeFilter->UseDefaultNormalOff();
  this->TubeFilter->CappingOn();
  this->RibbonFilter->SetWidth (0.15);
  this->RibbonFilter->SetInput (this->Squeezer->GetOutput());
  this->Actor->GetProperty()->SetAmbient (0.01);
  this->Actor->GetProperty()->SetDiffuse (0.7);
  this->Actor->GetProperty()->SetSpecular (0.29);
  this->Actor->GetProperty()->SetSpecularPower(10.0);
  this->Actor->GetProperty()->SetInterpolationToPhong();
  this->Actor->GetProperty()->BackfaceCullingOn();
  this->ColorMethod = COLOR_BY_FIXEDCOLOR;
  this->RenderingMode = vtkFibersManager::RENDER_IS_POLYLINES;
}

v3dFiberBundle::~v3dFiberBundle()
{
  this->Bundle->Delete();
  
  if( Cells )
  {
    this->Cells->Delete();
  }
  
  if( Points )
  {
    this->Points->Delete();
  }
    
  if( FiberColors )
  {
    this->FiberColors->Delete();
  }
    
  this->Squeezer->Delete();
  this->TubeFilter->Delete();
  this->RibbonFilter->Delete();
  this->Mapper->Delete();
  this->Actor->Delete();
}

void v3dFiberBundle::SetColorByDirection()
{
  this->ColorMethod = COLOR_BY_DIRECTION;
  this->Mapper->Modified();
  this->Mapper->SetScalarModeToUsePointData();
}

void v3dFiberBundle::SetColorByFixedColor()
{
  this->ColorMethod = COLOR_BY_FIXEDCOLOR;
  this->Actor->GetProperty()->SetColor ( this->Color[0], this->Color[1], this->Color[2]);
  this->Mapper->Modified();
  this->Mapper->SetScalarModeToUseCellData();
}

bool v3dFiberBundle::isColorFixed()
{
  return (this->ColorMethod == COLOR_BY_FIXEDCOLOR);
}

void v3dFiberBundle::Create()
{
  if( !this->Points || !this->Cells )
  {
    return;
  }
  
  this->Bundle->Initialize();
  this->Bundle->SetPoints ( this->Points );
  this->Bundle->SetLines ( this->Cells );
  this->Bundle->GetPointData()->SetScalars ( this->FiberColors );

  this->Squeezer->SetInput ( this->Bundle );

  this->Mapper->SetInput( this->Bundle );
  
  switch( this->RenderingMode )
  {
      case vtkFibersManager::RENDER_IS_POLYLINES:
	this->SetRenderingModeToPolyLines();
        break;

      case vtkFibersManager::RENDER_IS_TUBES:
	this->SetRenderingModeToTubes();
	break;

      case vtkFibersManager::RENDER_IS_RIBBONS:
	this->SetRenderingModeToRibbons();
        break;
  }
  
  this->PolyData = Bundle;

  this->Actor->SetMapper ( this->Mapper );
  this->SetColorByFixedColor();
  //this->Actor->GetProperty()->SetColor ( this->Color[0], this->Color[1], this->Color[2]);
}



void v3dFiberBundle::UseHardwareShadersOn()
{
  
#ifdef vtkINRIA3D_USE_HWSHADING

  vtkDebugMacro(<<"Hardware shading is activated.");
  
  if( vtkFiberMapper::SafeDownCast( this->Mapper ) ) // already using hardware shaders
  {
    return;
  }
  
  vtkFiberMapper* mapper = vtkFiberMapper::New();
  
  if( !mapper->IsRenderSupported() )
  {
    vtkErrorMacro(<<"Error: Hardware acceleration is not supported.");
    mapper->Delete();
    return;
  }
  
  mapper->SetAmbientContributionShadow(0.0);
  mapper->SetDiffuseContributionShadow(0.6);
  mapper->SetSpecularContributionShadow(0.0);
  mapper->LightingOff();
  mapper->ShadowingOff();

  mapper->SetScalarMode ( this->Mapper->GetScalarMode() );
  mapper->SelectColorArray ( this->Mapper->GetArrayId() );
  mapper->SetLookupTable ( this->Mapper->GetLookupTable() );
  mapper->SetInput( this->Bundle );
  this->Actor->SetMapper( mapper );
  
  this->Mapper->Delete(); this->Mapper = 0;
  this->Mapper = mapper;
  
  this->SetRenderingMode( this->GetRenderingMode() );
  
#endif
}

void v3dFiberBundle::UseHardwareShadersOff()
{

#ifdef vtkINRIA3D_USE_HWSHADING
  if( !vtkFiberMapper::SafeDownCast( this->Mapper ) )
  {
    return;
  }

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetScalarMode ( this->Mapper->GetScalarMode() );
  mapper->SelectColorArray ( this->Mapper->GetArrayId() );
  mapper->SetLookupTable ( this->Mapper->GetLookupTable() );
  mapper->SetInput( this->Bundle );
  this->Actor->SetMapper( mapper );
  
  this->Mapper->Delete(); this->Mapper = 0;
  this->Mapper = mapper;

  this->SetRenderingMode( this->GetRenderingMode() );
#endif
}


void v3dFiberBundle::SetUseHardwareShaders(int i)
{
  if( i==0 )
  {
    this->UseHardwareShadersOff();
  }
  else
  {
    this->UseHardwareShadersOn();
  }
}


void v3dFiberBundle::SetRenderingModeToPolyLines()
{
    this->RenderingMode = vtkFibersManager::RENDER_IS_POLYLINES;
#ifdef vtkINRIA3D_USE_HWSHADING
    vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
    if( mapper )
    {
      mapper->LightingOff();
      mapper->ShadowingOff();
    }
    else
    {
      this->Mapper->SetInput( this->Bundle );
    }
#else
    this->Mapper->SetInput( this->Bundle );
#endif
  }


void v3dFiberBundle::SetRenderingModeToTubes()
{
    this->RenderingMode = vtkFibersManager::RENDER_IS_TUBES;
#ifdef vtkINRIA3D_USE_HWSHADING
    vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
    if( mapper )
    {
      mapper->LightingOn();
      mapper->ShadowingOn();
    }
    else
    {
      //this->TubeFilter->Update();
      this->Mapper->SetInput ( this->TubeFilter->GetOutput() );
    }
#else
    //this->TubeFilter->Update();
    this->Mapper->SetInput ( this->TubeFilter->GetOutput() );
#endif
}


void v3dFiberBundle::SetRenderingModeToRibbons()
{
  this->RenderingMode = vtkFibersManager::RENDER_IS_RIBBONS;
#ifdef vtkINRIA3D_USE_HWSHADING
  vtkFiberMapper* mapper = vtkFiberMapper::SafeDownCast (this->Mapper);
  if( mapper )
  {
    mapper->LightingOn();
    mapper->ShadowingOff();
  }
  else
  {
    //this->RibbonFilter->Update();
    this->Mapper->SetInput ( this->RibbonFilter->GetOutput() );
    
    }
#else
  //this->RibbonFilter->Update();
  this->Mapper->SetInput ( this->RibbonFilter->GetOutput() );
#endif
}



void v3dFiberBundle::SetRenderingMode (int i)
{

  switch(i)
  {
      case vtkFibersManager::RENDER_IS_POLYLINES:
        this->SetRenderingModeToPolyLines();
        break;
        
      case vtkFibersManager::RENDER_IS_RIBBONS:
        this->SetRenderingModeToRibbons();
        break;
        
      case vtkFibersManager::RENDER_IS_TUBES:
        this->SetRenderingModeToTubes();
        break;
        
        
      default:
        vtkErrorMacro (<<"The rendering type is not recognized.");
        break;
        
  }
  
}
