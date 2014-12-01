/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkSeedPointHandle.h>

#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkViewport.h>
#include <vtkWindow.h>

#include <cmath>

vtkStandardNewMacro(medVtkSeedPointHandle);

medVtkSeedPointHandle::medVtkSeedPointHandle()
{
    this->PolyData = vtkSmartPointer<vtkPolyData>::New();
    this->Mapper2D = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    this->Mapper2D->SetInput(this->PolyData);

    this->SetHandle( PolyData );
    this->SizeInDisplayUnits = 10;
    this->SetHandleSize(this->SizeInDisplayUnits);

    this->BuildPolyData();

    // Default to red
    this->GetProperty()->SetColor(1,0,0);
}

medVtkSeedPointHandle::~medVtkSeedPointHandle()
{
    // TODO Auto-generated destructor stub
}

//----------------------------------------------------------------------------
void medVtkSeedPointHandle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void medVtkSeedPointHandle::BuildPolyData()
{
  // Check to see whether we have to rebuild everything
  // Initialize and get important info
  this->PolyData->Initialize();

  vtkSmartPointer<vtkPoints>  pts = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  this->PolyData->SetPoints(pts);
  this->PolyData->SetLines(lines);

  double center[3] = {0,0,0};

  this->InitialLength = 0.1;
  double radius = this->SizeHandlesInPixels(1.0,center);
  //radius *= / this->HandleSize;

  // Generate circle
  double len = vtkMath::DoubleTwoPi()*this->SizeInDisplayUnits;
  int num = len / 8;
  num = std::min( num , 64 );
  num = std::max( num , 1 );
  num = num *8;

  pts->SetNumberOfPoints(num);
  lines->InsertNextCell(num + 1);

  for ( int i(0); i<num; ++i ) {
      double theta = i * (double)( vtkMath::DoubleTwoPi() / num );
      pts->SetPoint(i, radius *  std::cos(theta), radius * std::sin(theta), 0. );
      lines->InsertCellPoint(i);
  }
  lines->InsertCellPoint(0);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(num/2);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0     + num/4);
  lines->InsertCellPoint(num/2 + num/4);

  this->BuildTime.Modified();
  this->Modified();
}

void medVtkSeedPointHandle::DeepCopy( vtkProp *prop )
{
    Superclass::DeepCopy(prop);
    if (medVtkSeedPointHandle * h = medVtkSeedPointHandle::SafeDownCast(prop) ) {
        this->SizeInDisplayUnits = h->SizeInDisplayUnits;
    }
}

void medVtkSeedPointHandle::ShallowCopy( vtkProp *prop )
{
    Superclass::ShallowCopy(prop);
    if (medVtkSeedPointHandle * h = medVtkSeedPointHandle::SafeDownCast(prop) ) {
        this->SizeInDisplayUnits = h->SizeInDisplayUnits;
    }
}

void medVtkSeedPointHandle::SetWorldPosition( double pos[3] )
{
    Superclass::SetWorldPosition(pos);

    // Need to call this to update the actor position
    // otherwise the bounds are not returned correctly.
    this->UpdateHandle();
}
