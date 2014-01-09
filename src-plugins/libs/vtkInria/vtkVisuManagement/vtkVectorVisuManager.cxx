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
    this->Glyph->ClampingOn();

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
}


vtkVectorVisuManager::~vtkVectorVisuManager()
{
    this->VOI->Delete();
    this->Glyph->Delete();
    this->Mapper->Delete();
    this->Actor->Delete();
    this->Assign->Delete();
    this->Orienter->Delete();
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


void vtkVectorVisuManager::SetScaleMode(ScaleMode mode)
{
    switch( mode )
    {
    case ScaleByScalar:
        this->Glyph->SetScaleModeToScaleByScalar();
        break;
    case ScaleByVector:
        this->Glyph->SetScaleModeToScaleByVector();
        break;
    case ScaleByVectorComponents:
        this->Glyph->SetScaleModeToScaleByVectorComponents();
        break;
    case ScalingOff:
        this->Glyph->SetScaleModeToDataScalingOff();
        break;
    default:
        return;
    }

    this->Glyph->Modified();
}

void vtkVectorVisuManager::SetColorMode(ColorMode mode)
{
    switch( mode )
    {
    case ColorByScalar:
        this->Glyph->SetColorModeToColorByScalar();
        break;
    case ColorByScale:
        this->Glyph->SetColorModeToColorByScale();
        break;
    case ColorByVector:
        this->Glyph->SetColorModeToColorByVector();
        break;
    default:
        return;
    }

    this->Glyph->Modified();

}

void vtkVectorVisuManager::SetProjection(bool enable)
{
    this->Orienter->SetProjection(enable);
    this->Orienter->Modified();
}


