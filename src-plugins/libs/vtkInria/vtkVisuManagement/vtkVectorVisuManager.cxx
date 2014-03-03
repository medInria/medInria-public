/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkVectorVisuManager.h"

#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkArrowSource.h>
#include <vtkProperty.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkInformation.h>
#include <vtkLookupTable.h>

vtkStandardNewMacro(vtkVectorVisuManager)

vtkVectorVisuManager::vtkVectorVisuManager()
{
    this->VOI = vtkExtractVOI::New();
    this->VOI->SetSampleRate(1,1,1);

    this->Assign = vtkAssignAttribute::New();
    Assign->SetInput(this->VOI->GetOutput());

    //tells vtkAssignAttribute to make the active scalars also the active vectors.
    Assign->Assign( vtkDataSetAttributes::SCALARS,
                    vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

    this->Orienter = vtkVectorOrienter::New();
    this->Orienter->SetInput( this->Assign->GetOutput() );

    vtkArrowSource *arrowSource = vtkArrowSource::New();

    this->Glyph = vtkGlyph3D::New();
    this->Glyph->SetInput( this->Orienter->GetOutput() );
    this->Glyph->SetSourceConnection(arrowSource->GetOutputPort());
    this->Glyph->SetVectorModeToUseVector();
    this->Glyph->SetColorModeToColorByVector();
    this->Glyph->SetScaleModeToScaleByVector();
    this->Glyph->OrientOn();
    this->Glyph->ScalingOn();
    this->Glyph->SetScaleFactor(1.0);

    this->Normals = vtkPolyDataNormals::New();
    this->Normals->SetInput( this->Glyph->GetOutput() );

    this->NormalsOrienter = vtkPolyDataNormalsOrienter::New();
    this->NormalsOrienter->SetInput(this->Normals->GetOutput());

    this->Mapper = vtkPolyDataMapper::New();
    this->Mapper->SetColorModeToMapScalars();
    this->Mapper->SetInput( this->NormalsOrienter->GetOutput() );

    this->Actor = vtkActor::New();
    this->Actor->SetMapper( this->Mapper );

    this->Input = 0;
    this->ValueArray = vtkDoubleArray::New();

    this->CurrentColorMode = ColorByVectorMagnitude;
}


vtkVectorVisuManager::~vtkVectorVisuManager()
{
    this->VOI->Delete();
    this->Glyph->Delete();
    this->Mapper->Delete();
    this->Actor->Delete();
    this->Assign->Delete();
    this->Orienter->Delete();
    this->ValueArray->Delete();
}


void vtkVectorVisuManager::SetInput(vtkImageData* data, vtkMatrix4x4 *matrix)
{
    if( !data )
    {
        std::cerr << "Error: null data." << std::endl;
        return;
    }

    this->Input = data;

    if(matrix)
    {
        this->Actor->SetUserMatrix(matrix);
    }

    this->VOI->SetInput ( this->Input );
    this->Orienter->SetOrientationMatrix(matrix);
    this->NormalsOrienter->SetOrientationMatrix(matrix);

}

void vtkVectorVisuManager::SetVOI(const int& imin, const int& imax,
                                  const int& jmin, const int& jmax,
                                  const int& kmin, const int& kmax)
{
    this->VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);
    this->Orienter->SetVOI(imin,imax,jmin,jmax,kmin,kmax);

    this->Orienter->Update();

    SetColorMode(CurrentColorMode);
}


void vtkVectorVisuManager::SetGlyphScale(const float& f)
{
    if (f <= 0.0)
    {
        cerr << "[Glyphs::SetGlyphScale] Invalid input range" << endl;
        return;
    }

    this->Glyph->SetScaleFactor(f);
}

void vtkVectorVisuManager::SetSampleRate(const int& a, const int& b, const int& c)
{
    this->VOI->SetSampleRate(a,b,c);
}


double vtkVectorVisuManager::GetGlyphScale()
{
    return this->Glyph->GetScaleFactor();
}


void vtkVectorVisuManager::SetColorMode(ColorMode mode)
{
    this->Orienter->Modified();
    this->Orienter->Update();

    switch( mode )
    {
    case ColorByVectorMagnitude:
        SetColorByVectorMagnitude();
        break;
    case ColorByVectorDirection:
        SetUpLUTToMapVectorDirection();
        break;
    default:
        return;
    }

    this->CurrentColorMode =  mode;

    this->Glyph->Modified();
}

void vtkVectorVisuManager::SetProjection(bool enable)
{
    this->Orienter->SetProjection(enable);

    SetColorMode(CurrentColorMode);
}

void vtkVectorVisuManager::SetColorByVectorMagnitude()
{
    this->Glyph->SetColorModeToColorByVector();
    this->Mapper->SetLookupTable(0);

    double range[2];
    if(this->Orienter->GetOutput()->GetPointData()->GetScalars())
    {
        this->Orienter->GetOutput()->GetPointData()->GetScalars()->GetRange(range);
        this->Mapper->SetScalarRange(range[0],range[1]);
    }

    this->Mapper->Modified();
}

void vtkVectorVisuManager::SetUpLUTToMapVectorDirection()
{
  vtkDataSet *data = Orienter->GetOutput();

  int numPoints    = data->GetNumberOfPoints();

  this->ValueArray->Initialize();
  this->ValueArray->SetNumberOfComponents(1);
  this->ValueArray->SetNumberOfTuples(numPoints);

  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(numPoints);

  for(int i=0;i<numPoints;i++)
  {
    // Color coding with the eigenvector
    double vect[3];
    data->GetPointData()->GetVectors()->GetTuple(i,vect);

    vtkMath::Normalize(vect);

    double r = fabs(vect[0]);
    double g = fabs(vect[1]);
    double b = fabs(vect[2]);

    r = (r>1.0)?1.0:r;
    g = (g>1.0)?1.0:g;
    b = (b>1.0)?1.0:b;

    lut->SetTableValue(i, r, g, b);

    this->ValueArray->SetTuple1(i, (unsigned int)i);
  }

  data->GetPointData()->SetScalars( this->ValueArray );

  this->Glyph->SetColorModeToColorByScalar();

  this->Glyph->Modified();

  this->Mapper->SetLookupTable(lut);
  this->Mapper->SetScalarRange(0,numPoints-1);

  lut->Delete();
}
