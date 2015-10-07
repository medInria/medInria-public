/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkVectorVisuManager.h>

#include <limits>

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
    this->UserColor[0] = 1.; this->UserColor[1] = 1.; this->UserColor[2] = 1.;

    this->VOI = vtkExtractVOI::New();
    this->VOI->SetSampleRate(1,1,1);

    this->Assign = vtkAssignAttribute::New();
    Assign->SetInputConnection(this->VOI->GetOutputPort());

    //tells vtkAssignAttribute to make the active scalars also the active vectors.
    Assign->Assign( vtkDataSetAttributes::SCALARS,
                    vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

    this->Orienter = vtkVectorOrienter::New();
    this->Orienter->SetInputConnection( this->Assign->GetOutputPort() );

    vtkArrowSource *arrowSource = vtkArrowSource::New();

    this->Glyph = vtkGlyph3D::New();
    this->Glyph->SetInputConnection( this->Orienter->GetOutputPort() );
    this->Glyph->SetSourceConnection(arrowSource->GetOutputPort());
    this->Glyph->SetVectorModeToUseVector();
    this->Glyph->SetColorModeToColorByVector();
    this->Glyph->SetScaleModeToScaleByVector();
    this->Glyph->OrientOn();
    this->Glyph->ScalingOn();
    this->Glyph->SetScaleFactor(1.0);

    this->Normals = vtkPolyDataNormals::New();
    this->Normals->SetInputConnection( this->Glyph->GetOutputPort() );

    this->NormalsOrienter = vtkPolyDataNormalsOrienter::New();
    this->NormalsOrienter->SetInputConnection(this->Normals->GetOutputPort());

    this->Mapper = vtkPolyDataMapper::New();
    this->Mapper->SetColorModeToMapScalars();
    this->Mapper->SetInputConnection( this->NormalsOrienter->GetOutputPort() );

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

    this->VOI->SetInputData ( this->Input );
    this->Orienter->SetOrientationMatrix(matrix);
    this->NormalsOrienter->SetOrientationMatrix(matrix);

}

void vtkVectorVisuManager::SetVOI(const int& imin, const int& imax,
                                  const int& jmin, const int& jmax,
                                  const int& kmin, const int& kmax)
{
    this->VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);
    this->Orienter->SetVOI(imin,imax,jmin,jmax,kmin,kmax);

    this->VOI->Modified();
    this->Orienter->Modified();

    this->VOI->Update();
    this->Mapper->Update();

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

    this->Glyph->Modified();
}

void vtkVectorVisuManager::SetSampleRate(const int& a, const int& b, const int& c)
{
    this->VOI->SetSampleRate(a,b,c);

    this->VOI->Modified();

    this->VOI->Update();
    this->Mapper->Update();

    SetColorMode(CurrentColorMode);
}


double vtkVectorVisuManager::GetGlyphScale()
{
    return this->Glyph->GetScaleFactor();
}


void vtkVectorVisuManager::SetColorMode(ColorMode mode)
{
    switch( mode )
    {
        case ColorByVectorMagnitude:
            SetColorByVectorMagnitude();
            break;
        case ColorByVectorDirection:
            SetUpLUTToMapVectorDirection();
            break;
        case ColorByUserColor:
            SetColorByUserColor();
            break;
        default:
            return;
    }

    this->CurrentColorMode =  mode;

    this->Glyph->Modified();
}

void vtkVectorVisuManager::SetUserColor(double color[3])
{
    UserColor[0] = color[0];
    UserColor[1] = color[1];
    UserColor[2] = color[2];
}

void vtkVectorVisuManager::SetProjection(bool enable)
{
    this->Orienter->SetProjection(enable);

    this->Orienter->Modified();
    this->Orienter->Update();

    SetColorMode(CurrentColorMode);
}

void vtkVectorVisuManager::SetColorByVectorMagnitude()
{
    vtkDataSet *data = Orienter->GetOutput();

    int numPoints    = data->GetNumberOfPoints();

    this->ValueArray->Initialize();
    this->ValueArray->SetNumberOfComponents(1);
    this->ValueArray->SetNumberOfTuples(numPoints);

    vtkLookupTable* lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues(numPoints);
    std::vector<double> norm;
    double min(std::numeric_limits<double>::max()), max(0);

    for(int i=0;i<numPoints;i++)
    {
      // Color coding with the eigenvector
      double vect[3];
      data->GetPointData()->GetVectors()->GetTuple(i,vect);
      norm.push_back(vtkMath::Norm(vect));
      if(norm[i] < min)
          min = norm[i];

      if(norm[i] > max)
          max = norm[i];
    }

    for(int i=0;i<numPoints;i++)
    {


      double hue =   (2.0/3.0)/(min-max) * norm[i] + (2 * max / 3)/(max-min);

      double hsv[3] = {hue, 1, 1};
      double rgb[3];
      vtkMath::HSVToRGB(hsv, rgb);

      lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
      this->ValueArray->SetTuple1(i, (unsigned int)i);
    }

    data->GetPointData()->SetScalars( this->ValueArray );

    this->Glyph->SetColorModeToColorByScalar();

    this->Glyph->Modified();

    this->Mapper->SetLookupTable(lut);
    this->Mapper->SetScalarRange(0, numPoints);

    lut->Delete();
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

void vtkVectorVisuManager::SetColorByUserColor()
{
    // free some memory!
    vtkDataSet *data = Orienter->GetOutput();
    data->GetPointData()->SetScalars(NULL);
    this->ValueArray->Initialize();
    this->Mapper->SetLookupTable(NULL);

    this->Mapper->SetColorModeToDefault();
    this->Actor->GetProperty()->SetColor(UserColor);

    this->Mapper->Modified();
    this->Actor->Modified();
}
