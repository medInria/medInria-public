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
#include <vtkAssignAttribute.h>
#include <vtkMatrix4x4.h>

vtkStandardNewMacro(vtkVectorVisuManager)

vtkVectorVisuManager::vtkVectorVisuManager()
{
    this->VOI = vtkExtractVOI::New();
    this->VOI->SetSampleRate(1,1,1);

    vtkArrowSource *arrowSource = vtkArrowSource::New();

    this->Glyph = vtkGlyph3D::New();
    this->Glyph->SetInput( this->VOI->GetOutput() );
    this->Glyph->SetSourceConnection(arrowSource->GetOutputPort());
    this->Glyph->SetVectorModeToUseVector();
    this->Glyph->SetColorModeToColorByScalar();
    // this->Glyph->SetScaleModeToScaleByVector();
    this->Glyph->SetScaleModeToScaleByScalar();
    this->Glyph->OrientOn();
    this->Glyph->ScalingOn();
    this->Glyph->SetScaleFactor(1.0);
    this->Glyph->ClampingOn();

    this->Mapper = vtkPolyDataMapper::New();
    this->Mapper->SetColorModeToMapScalars();
    this->Mapper->SetInput( this->Glyph->GetOutput() );

    this->Actor = vtkActor::New();
    this->Actor->SetMapper( this->Mapper );

    this->Scalars = 0;

    this->Input = 0;

    Projection = false;
}


vtkVectorVisuManager::~vtkVectorVisuManager()
{
    this->VOI->Delete();
    this->Glyph->Delete();
    this->Normals->Delete();
    this->Mapper->Delete();
    this->Actor->Delete();

    if( this->Scalars )
    {
        this->Scalars->Delete();
    }
}


void vtkVectorVisuManager::SetInput(vtkImageData* data, vtkMatrix4x4 *matrix)
{
    if( !data )
    {
        std::cerr << "Error: null data." << std::endl;
        return;
    }

    this->Input = data;

    if( this->Input->GetPointData()->GetScalars() )
    {
        this->SetScalars (this->Input->GetPointData()->GetScalars());
    }
    else
    {
        this->SetScalars (0);
    }

    if(matrix)
    {
        this->Actor->SetUserMatrix(matrix);
    }

    this->VOI->SetInput ( this->Input );
    Matrix = matrix;
}

void vtkVectorVisuManager::SetVOI(const int& imin, const int& imax,
                                  const int& jmin, const int& jmax,
                                  const int& kmin, const int& kmax)
{
    this->VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);

    if(imin==imax)
        Orientation = sagittal;
    else if(jmin==jmax)
        Orientation = coronal;
    else if(kmin==kmax)
        Orientation = axial;

    this->VOI->SetInput ( this->Input );

    Update();
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
    modifiyPointDataProperty();
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
    Projection = enable;

    Update();
}

void vtkVectorVisuManager::modifiyPointDataProperty()
{
    vtkImageData* VOIOutput = this->VOI->GetOutput();

    vtkAssignAttribute *assign = vtkAssignAttribute::New();
    assign->SetInput(VOIOutput);
    assign->Assign( vtkDataSetAttributes::SCALARS,
                    vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);
    assign->Update();

    int n = VOIOutput->GetPointData()->GetNumberOfArrays();

    if( n == 1)
    {
        VOIOutput->GetPointData()->SetActiveVectors(VOIOutput->GetPointData()->GetArrayName(n-1));
    }

    this->Glyph->SetInputArrayToProcess(3, VOIOutput->GetInformation());
}


void vtkVectorVisuManager::projectVectorsOnSlice()
{
    this->VOI->SetInput ( this->Input );
    this->VOI->Update();
    vtkImageData* VOIOutput = this->VOI->GetOutput();

    if( this->Input && VOIOutput && Matrix)
    {
        vtkMatrix4x4 *matrixT = vtkMatrix4x4::New();
        vtkMatrix4x4::Transpose(Matrix, matrixT);

        vtkMatrix4x4 *matrixI = vtkMatrix4x4::New();
        vtkMatrix4x4::Invert(Matrix, matrixI);

        int t = VOIOutput->GetPointData()->GetScalars()->GetNumberOfTuples();

        double tuple3[3];
        double tuple4[4] = {0,0,0,0};

        for (int i=0; i<t;i++)
        {
            VOIOutput->GetPointData()->GetScalars()->GetTuple(i,tuple3);

            //if(Orientation==axial)
            //std::cout << tuple3[0] << "**" << tuple3[1] <<  "**" <<  tuple3[2]<< "\n";

            tuple4[0] = tuple3[0];
            tuple4[1] = tuple3[1];
            tuple4[2] = tuple3[2];

            Matrix->MultiplyPoint(tuple4, tuple4);
            Matrix->MultiplyPoint(tuple4, tuple4);

            if(Orientation==sagittal)
                tuple4[0] = 0;
            else if(Orientation==coronal)
                tuple4[1] = 0;
            else if(Orientation==axial)
                tuple4[2] = 0;

            matrixT->MultiplyPoint(tuple4, tuple4);
            matrixT->MultiplyPoint(tuple4, tuple4);

            tuple3[0] = tuple4[0];
            tuple3[1] = tuple4[1];
            tuple3[2] = tuple4[2];

            if(Orientation==axial)
            std::cout << "après: " << tuple3[0] << "**"<< tuple3[1] << "**" <<  tuple3[2]<< "\n";

            VOIOutput->GetPointData()->GetScalars()->SetTuple(i,tuple3);

        }
    }
}


void vtkVectorVisuManager::applyMatrix()
{
    this->VOI->Update();
    vtkImageData* VOIOutput = this->VOI->GetOutput();

    if( this->Input && VOIOutput && Matrix)
    {
        vtkMatrix4x4 *matrixT = vtkMatrix4x4::New();
        vtkMatrix4x4::Transpose(Matrix, matrixT);

        int t = VOIOutput->GetPointData()->GetScalars()->GetNumberOfTuples();

        double tuple3[3];
        double tuple4[4] = {0,0,0,0};

        for (int i=0; i<t;i++)
        {
            VOIOutput->GetPointData()->GetScalars()->GetTuple(i,tuple3);

            tuple4[0] = tuple3[0];
            tuple4[1] = tuple3[1];
            tuple4[2] = tuple3[2];

            matrixT->MultiplyPoint(tuple4, tuple4);

            tuple3[0] = tuple4[0];
            tuple3[1] = tuple4[1];
            tuple3[2] = tuple4[2];

            VOIOutput->GetPointData()->GetScalars()->SetTuple(i,tuple3);
        }
    }
}

void vtkVectorVisuManager::Update()
{
    modifiyPointDataProperty();

    applyMatrix();

    if(Projection)
        projectVectorsOnSlice();
}
