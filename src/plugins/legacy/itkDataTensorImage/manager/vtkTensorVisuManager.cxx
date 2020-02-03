/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkTensorVisuManager.h"

#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkFieldData.h>
#include <vtkArrowSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkProperty.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>

#include "vtkLookupTableManager.h"

vtkStandardNewMacro(vtkTensorVisuManager)

vtkTensorVisuManager::vtkTensorVisuManager()
{

    this->Fliper = vtkFlipTensorImageFilter::New();

    this->VOI = vtkExtractVOI::New();
    this->VOI->SetSampleRate(1,1,1);
    this->VOI->SetInputConnection ( this->Fliper->GetOutputPort() );

    this->Glyph = vtkTensorGlyph::New();
    this->Glyph->SetInputConnection( this->VOI->GetOutputPort() );
    this->Glyph->SetScaleFactor(1000.0);
    this->Glyph->ClampScalingOn();
    this->Glyph->ColorGlyphsOn();

    this->SetGlyphShapeToSphere();

    this->Normals = vtkPolyDataNormals::New();
    this->Normals->SetInputConnection( this->Glyph->GetOutputPort() );

    this->Mapper = vtkPolyDataMapper::New();
    this->Mapper->SetColorModeToMapScalars();
    this->Mapper->SetInputConnection( this->Normals->GetOutputPort() );

    this->Actor = vtkActor::New();
    this->Actor->SetMapper( this->Mapper );

    this->Scalars = nullptr;

    this->EigenvectorArray = vtkUnsignedIntArray::New();
    this->EigenvalueArray = vtkDoubleArray::New();
    this->TraceArray = vtkDoubleArray::New();
    this->VolumeArray = vtkDoubleArray::New();
    this->DistanceArray = vtkDoubleArray::New();

    this->Input = nullptr;

    this->ColorMode = COLOR_BY_EIGENVECTOR;
    this->EigenNumber = 2; // biggest eigen element

    this->Min = 0.0;
    this->Max = 1.0;

    this->LUT = nullptr;

    this->GlyphResolution = 6;
}

vtkTensorVisuManager::~vtkTensorVisuManager()
{
    this->Fliper->Delete();
    this->VOI->Delete();
    this->Glyph->Delete();
    this->Normals->Delete();
    this->Mapper->Delete();
    this->Actor->Delete();

    this->EigenvectorArray->Delete();
    this->EigenvalueArray->Delete();
    this->TraceArray->Delete();
    this->VolumeArray->Delete();
    this->DistanceArray->Delete();

    if( this->Scalars )
    {
        this->Scalars->Delete();
    }
}

void vtkTensorVisuManager::SetGlyphShape (int i)
{
    switch(i)
    {
        case 0:

            this->SetGlyphShapeToLine();
            break;

        case 1:

            this->SetGlyphShapeToArrow();
            break;

        case 2:

            this->SetGlyphShapeToDisk();
            break;

        case 3:

            this->SetGlyphShapeToCylinder();
            break;

        case 4:
            this->SetGlyphShapeToCube();
            break;

        case 5:
            this->SetGlyphShapeToSphere();
            break;

        case 6:
            this->SetGlyphShapeToSuperquadric();
            break;

        default:
            std::cerr << "Error: shape is not recognized." << std::endl;
            return;
    }

    this->SetGlyphResolution( this->GlyphResolution );
}

void vtkTensorVisuManager::SetGlyphShapeToLine()
{
    this->ShapeMode = GLYPH_LINE;
    this->Shape = vtkLineSource::New();
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToDisk()
{
    this->ShapeMode = GLYPH_DISK;
    vtkDiskSource* source = vtkDiskSource::New();
    source->SetInnerRadius (0.0);
    this->Shape = source;
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToArrow()
{
    this->ShapeMode = GLYPH_ARROW;
    this->Shape = vtkArrowSource::New();
    vtkArrowSource* arrow = vtkArrowSource::SafeDownCast (this->Shape);
    arrow->SetTipLength (0.0);
    arrow->SetTipRadius (0.0);
    arrow->SetShaftRadius (0.18);

    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToCube()
{
    this->ShapeMode = GLYPH_CUBE;
    this->Shape = vtkCubeSource::New();
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToCylinder()
{
    this->ShapeMode = GLYPH_CYLINDER;
    this->Shape = vtkCylinderSource::New();
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToSphere()
{
    this->ShapeMode = GLYPH_SPHERE;
    this->Shape = vtkSphereSource::New();
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphShapeToSuperquadric()
{
    this->ShapeMode = GLYPH_SUPERQUADRIC;
    this->Shape = vtkSuperquadricSource::New();
    vtkSuperquadricSource::SafeDownCast (this->Shape)->SetPhiRoundness (0.25);
    vtkSuperquadricSource::SafeDownCast (this->Shape)->SetThetaRoundness (0.25);
    this->Glyph->SetSourceConnection(this->Shape->GetOutputPort());
    this->Shape->Delete();
}

void vtkTensorVisuManager::SetGlyphResolution (int res)
{
    this->GlyphResolution = res;

    vtkArrowSource* arrowSource = 0;
    vtkDiskSource* diskSource = 0;
    vtkCylinderSource* cylinderSource = 0;
    vtkSphereSource* sphereSource = 0;
    vtkSuperquadricSource* quadricSource = 0;

    switch(this->ShapeMode)
    {
        case GLYPH_LINE:
            break;

        case GLYPH_ARROW:
            arrowSource = vtkArrowSource::SafeDownCast (this->Shape);
            if( arrowSource )
                arrowSource->SetShaftResolution (res);
            break;

        case GLYPH_DISK:
            diskSource = vtkDiskSource::SafeDownCast (this->Shape);
            if( diskSource )
                diskSource->SetCircumferentialResolution(res);
            break;

        case GLYPH_CYLINDER:
            cylinderSource = vtkCylinderSource::SafeDownCast (this->Shape);
            if( cylinderSource )
                cylinderSource->SetResolution (res);
            break;

        case GLYPH_CUBE:
            break;

        case GLYPH_SPHERE:
            sphereSource = vtkSphereSource::SafeDownCast (this->Shape);
            if( sphereSource )
            {
                sphereSource->SetThetaResolution (res);
                sphereSource->SetPhiResolution (res);
            }
            break;

        case GLYPH_SUPERQUADRIC:
            quadricSource = vtkSuperquadricSource::SafeDownCast (this->Shape);
            if( quadricSource )
            {
                quadricSource->SetThetaResolution (res);
                quadricSource->SetPhiResolution (res);
            }
            break;

        default:
            break;

    }

    this->Glyph->Modified();
}

void vtkTensorVisuManager::SetGlyphScale(const float& f)
{
    if (f <= 0.0)
    {
        cerr << "[Glyphs::SetGlyphScale] Invalid input range" << endl;
        return;
    }

    this->Glyph->SetScaleFactor(f);
}

double vtkTensorVisuManager::GetGlyphScale()
{
    return this->Glyph->GetScaleFactor();
}

void vtkTensorVisuManager::SetMaxGlyphSize(const float& f)
{
    if (f < 0.0)
    {
        cerr << "[Glyphs::SetMaxGlyphSize] Invalid input range" << endl;
        return;
    }

    this->Glyph->SetMaxScaleFactor(f);
}

void vtkTensorVisuManager::SetInput(vtkStructuredPoints* data, vtkMatrix4x4 *matrix)
{
    if( !data ) // OTHERS THING TO CHECK!
    {
        std::cerr << "Error: null data." << std::endl;
        return;
    }
    if( !(data->GetPointData()->GetTensors()) )
    {
        std::cerr << "Error: null tensors data." << std::endl;
        return;
    }

    this->Input = data;

    if( this->Input->GetPointData()->GetScalars() )
    {
        this->SetScalars (this->Input->GetPointData()->GetScalars() );
    }
    else
    {
        this->SetScalars (0);
    }

    if (matrix)
    {
        this->Actor->SetUserMatrix(matrix);
    }

    this->Fliper->SetInputData ( this->Input );
    this->Fliper->Update();

    this->UpdateLUT();
}

void vtkTensorVisuManager::SetInput(vtkUnstructuredGrid* data, vtkMatrix4x4 *matrix)
{
    if( !data )
    {
        std::cerr << "Error: null data." << std::endl;
        return;
    }
    if( !(data->GetPointData()->GetTensors()) )
    {
        std::cerr << "Error: null tensors data." << std::endl;
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

    if (matrix)
    {
        this->Actor->SetUserMatrix(matrix);
    }

    this->Glyph->SetInputData( data );
    this->Glyph->Update();
    this->UpdateLUT();
}

void vtkTensorVisuManager::SetVOI(const int& imin, const int& imax,
                                  const int& jmin, const int& jmax,
                                  const int& kmin, const int& kmax)
{
    this->VOI->SetVOI(imin,imax,jmin,jmax,kmin,kmax);
}

void vtkTensorVisuManager::SetSampleRate(const int& a, const int& b, const int& c)
{
    this->VOI->SetSampleRate(a,b,c);
}

void vtkTensorVisuManager::SetColorModeToEigenvector( const int& i )
{

    if(i<0 || i>2)
    {
        std::cerr << "Error: wrong eigen vector index." << std::endl;
        return;
    }

    this->EigenNumber = i;
    this->ColorMode = COLOR_BY_EIGENVECTOR;
    this->UpdateLUT();

}

void vtkTensorVisuManager::SetColorModeToEigenvalue( const int& i )
{
    if(i<0 || i>2)
    {
        std::cerr << "Error: wrong eigen value index." << std::endl;
        return;
    }

    this->EigenNumber = i;
    this->ColorMode = COLOR_BY_EIGENVALUE;
    this->UpdateLUT();

}

void vtkTensorVisuManager::SetColorModeToVolume()
{
    this->ColorMode = COLOR_BY_VOLUME;
    this->UpdateLUT();
}

void vtkTensorVisuManager::SetColorModeToTrace()
{
    this->ColorMode = COLOR_BY_TRACE;
    this->UpdateLUT();
}

void vtkTensorVisuManager::SetColorModeToDistanceToIdentity()
{
    this->ColorMode = COLOR_BY_DISTANCE_TO_IDENTITY;
    this->UpdateLUT();
}

void vtkTensorVisuManager::UpdateLUT()
{
    if(this->GetInput() == 0)
    {
        return;
    }

    switch(this->ColorMode)
    {
        case COLOR_BY_EIGENVECTOR:
            this->SetUpLUTToMapEigenVector();
            break;

        case COLOR_BY_EIGENVALUE:
            this->SetUpLUTToMapEigenValue();
            break;

        case COLOR_BY_VOLUME:
            this->SetUpLUTToMapVolume();
            break;

        case COLOR_BY_TRACE:
            this->SetUpLUTToMapTrace();
            break;

        case COLOR_BY_DISTANCE_TO_IDENTITY:
            this->SetUpLUTToMapDistanceToIdentity();
            break;

        case COLOR_BY_SCALAR:
            this->SetUpLUTToMapScalars();
            break;

        default:
            this->SetUpLUTToMapEigenVector();
            break;
    }
}

void vtkTensorVisuManager::SetUpLUTToMapEigenVector()
{
    vtkDataSet* myData = this->GetInput();

    int numPoints = myData->GetPointData()->GetTensors()->GetNumberOfTuples();
    this->EigenvectorArray->Initialize();
    this->EigenvectorArray->SetNumberOfComponents(1);
    this->EigenvectorArray->SetNumberOfTuples(numPoints);

    vtkLookupTable* lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues(numPoints);

    vtkMatrix4x4 *userMatrix = this->Actor->GetUserMatrix();
    double coefs[9];
    double vRotated[3];

    double **a;
    a = new double*[3];
    double *w = new double[3];
    double **v;
    v = new double*[3];
    
    for( unsigned int j=0; j<3; j++)
    {
        a[j] = new double[3];
        v[j] = new double[3];
    }

    for(int i=0;i<numPoints;i++)
    {
        // Color coding with the eigenvector
        myData->GetPointData()->GetTensors()->GetTuple(i,coefs);

        for( int nl=0; nl<3; nl++)
        {
            for( int nc=0; nc<3; nc++)
            {
                a[nl][nc] = coefs[nl*3+nc];
            }
        }

        vtkMath::Jacobi (a, w, v);

        for (unsigned int j = 0;j < 3;++j)
        {
            vRotated[j] = 0;
            for (unsigned int k = 0;k < 3;++k)
                vRotated[j] += v[k][2-this->EigenNumber] * userMatrix->GetElement(j,k);
        }

        double r = fabs(vRotated[0]);
        double g = fabs(vRotated[1]);
        double b = fabs(vRotated[2]);

        r = (r>1.0)?1.0:r;
        g = (g>1.0)?1.0:g;
        b = (b>1.0)?1.0:b;

        lut->SetTableValue(i, r, g, b);

        this->EigenvectorArray->SetTuple1(i, (unsigned int)i);
    }

    for( int j=0; j<3; j++)
    {
        delete [] a[j];
        delete [] v[j];
    }

    delete [] a;
    delete [] w;
    delete [] v;

    myData->GetPointData()->SetScalars( this->EigenvectorArray );
    this->Glyph->Modified();

    this->Mapper->SetColorModeToMapScalars();

    this->Mapper->SetLookupTable(lut);
    this->Mapper->SetScalarRange(0,numPoints-1);

    lut->Delete();
}

void vtkTensorVisuManager::SetUpLUTToMapEigenValue()
{
    vtkDataSet* myData = this->GetInput();

    int numPoints = myData->GetPointData()->GetTensors()->GetNumberOfTuples();

    this->EigenvalueArray->Initialize();
    this->EigenvalueArray->SetNumberOfComponents(1);
    this->EigenvalueArray->SetNumberOfTuples(numPoints);

    double coefs[9];
    double **a;
    a = new double*[3];
    double *w = new double[3];
    double **v;
    v = new double*[3];
    
    for( unsigned int j=0; j<3; j++)
    {
        a[j] = new double[3];
        v[j] = new double[3];
    }

    for(int i=0;i<numPoints;i++)
    {
        // Color coding with the eigenvalue
        myData->GetPointData()->GetTensors()->GetTuple(i,coefs);

        for( int nl=0; nl<3; nl++)
        {
            for( int nc=0; nc<3; nc++)
            {
                a[nl][nc] = coefs[nl*3+nc];
            }
        }

        vtkMath::Jacobi (a, w, v);

        double val = w[2-this->EigenNumber];

        this->EigenvalueArray->SetTuple1(i,val);
    }

    for( int j=0; j<3; j++)
    {
        delete [] a[j];
        delete [] v[j];
    }

    delete [] a;
    delete [] w;
    delete [] v;

    double range[2];
    this->EigenvalueArray->GetRange (range);
    std::cout << "Eigenvalue range is: " << range[0] << " " << range[1] << std::endl;

    myData->GetPointData()->SetScalars(this->EigenvalueArray);
    this->Glyph->Modified();

    this->Mapper->SetColorModeToMapScalars();

    if( !this->LUT )
    {
        vtkLookupTable* lut = vtkLookupTable::New();
        lut->SetHueRange(0.667,0.0);
        lut->SetSaturationRange(1.0,1.0);
        lut->SetValueRange(1.0,1.0);
        lut->Build();

        this->SetLookupTable(lut);
        lut->Delete();
    }
    else
    {
        this->Mapper->SetLookupTable (this->LUT);
    }

    this->Mapper->SetScalarRange(this->Min,this->Max);
}

void vtkTensorVisuManager::SetUpLUTToMapVolume()
{
    vtkDataSet* myData = this->GetInput();

    int numPoints = myData->GetPointData()->GetTensors()->GetNumberOfTuples();
    this->VolumeArray->Initialize();
    this->VolumeArray->SetNumberOfComponents(1);
    this->VolumeArray->SetNumberOfTuples(numPoints);

    double coefs[9];

    double **a;
    a = new double*[3];
    double *w = new double[3];
    double **v;
    v = new double*[3];
    
    for( unsigned int j=0; j<3; j++)
    {
        a[j] = new double[3];
        v[j] = new double[3];
    }

    for(int i=0;i<numPoints;i++)
    {
        // Color coding with the eigenvalue
        myData->GetPointData()->GetTensors()->GetTuple(i,coefs);

        for( int nl=0; nl<3; nl++)
        {
            for( int nc=0; nc<3; nc++)
            {
                a[nl][nc] = coefs[nl*3+nc];
            }
        }

        vtkMath::Jacobi (a, w, v);

        double prod = 1.0;

        for(int j=0;j<3;j++)
        {
            prod *= w[j];
        }

        this->VolumeArray->SetTuple1(i, prod);
    }

    for( int j=0; j<3; j++)
    {
        delete [] a[j];
        delete [] v[j];
    }

    delete [] a;
    delete [] w;
    delete [] v;

    double range[2];
    this->VolumeArray->GetRange (range);
    std::cout << "Volume range is: " << range[0] << " " << range[1] << std::endl;

    myData->GetPointData()->SetScalars(this->VolumeArray);
    this->Glyph->Modified();
    this->LUT->SetRange (range);
    this->Mapper->SetColorModeToMapScalars();
    this->Mapper->UseLookupTableScalarRangeOn();

    if( !this->LUT )
    {
        vtkLookupTable* lut = vtkLookupTable::New();
        lut->SetHueRange(0.0,1.0);
        lut->SetSaturationRange(1.0,1.0);
        lut->SetValueRange(1.0,1.0);
        lut->Build();

        this->SetLookupTable(lut);
        lut->Delete();
    }
    else
    {
        this->Mapper->SetLookupTable (this->LUT);
    }

    this->Mapper->SetScalarRange(this->Min,this->Max);

}

void vtkTensorVisuManager::SetUpLUTToMapTrace()
{
    vtkDataSet* myData = this->GetInput();

    int numPoints = myData->GetPointData()->GetTensors()->GetNumberOfTuples();

    this->TraceArray->Initialize();
    this->TraceArray->SetNumberOfComponents(1);
    this->TraceArray->SetNumberOfTuples(numPoints);

    double coefs[9];

    for(int i=0;i<numPoints;i++)
    {
        // Color coding with the eigenvalue
        myData->GetPointData()->GetTensors()->GetTuple(i,coefs);


        double trace2 = coefs[0]*coefs[0] + coefs[1]*coefs[1] + coefs[2]*coefs[2] +
                coefs[3]*coefs[3] + coefs[4]*coefs[4] + coefs[5]*coefs[5] +
                coefs[6]*coefs[6] + coefs[7]*coefs[7] + coefs[8]*coefs[8];

        double sum = sqrt ( trace2 );

        this->TraceArray->SetTuple1(i, sum);

    }

    double range[2];
    this->TraceArray->GetRange (range);
    std::cout << "RMS range is: " << range[0] << " " << range[1] << std::endl;

    myData->GetPointData()->SetScalars (this->TraceArray);
    this->Glyph->Modified();

    this->Mapper->SetColorModeToMapScalars();

    if( !this->LUT )
    {
        vtkLookupTable* lut = vtkLookupTableManager::GetLONILookupTable();
        this->SetLookupTable(lut);
        lut->Delete();
    }
    else
    {
        this->Mapper->SetLookupTable (this->LUT);
    }

    this->Mapper->SetScalarRange(this->Min,this->Max);
}

void vtkTensorVisuManager::SetUpLUTToMapDistanceToIdentity()
{
    vtkDataSet* myData = this->GetInput();

    int numPoints = myData->GetPointData()->GetTensors()->GetNumberOfTuples();

    this->DistanceArray->Initialize();
    this->DistanceArray->SetNumberOfComponents(1);
    this->DistanceArray->SetNumberOfTuples(numPoints);

    double coefs[9];

    double **a;
    a = new double*[3];
    double *w = new double[3];
    double **v;
    v = new double*[3];
    
    for( unsigned int j=0; j<3; j++)
    {
        a[j] = new double[3];
        v[j] = new double[3];
    }

    for(int i=0;i<numPoints;i++)
    {
        // Color coding with the eigenvalue
        myData->GetPointData()->GetTensors()->GetTuple(i,coefs);

        for( int nl=0; nl<3; nl++)
        {
            for( int nc=0; nc<3; nc++)
            {
                a[nl][nc] = coefs[nl*3+nc];
            }
        }

        vtkMath::Jacobi (a, w, v);

        double norm = 1.0;
        for( int j=0; j<3; j++)
        {
            norm += log (w[j])*log (w[j]);
        }
        norm = sqrt (norm);

        this->DistanceArray->SetTuple1(i,norm);
    }

    for( int j=0; j<3; j++)
    {
        delete [] a[j];
        delete [] v[j];
    }

    delete [] a;
    delete [] w;
    delete [] v;

    double range[2];
    this->DistanceArray->GetRange (range);
    std::cout << "Norm range is: " << range[0] << " " << range[1] << std::endl;

    myData->GetPointData()->SetScalars(this->DistanceArray);
    this->Glyph->Modified();

    this->Mapper->SetColorModeToMapScalars();

    if( !this->LUT )
    {
        vtkLookupTable* lut = vtkLookupTable::New();
        lut->SetHueRange(0.334,1.0);
        lut->SetSaturationRange(1.0,1.0);
        lut->SetValueRange(1.0,1.0);
        lut->Build();

        this->SetLookupTable(lut);
        lut->Delete();
    }
    else
    {
        this->Mapper->SetLookupTable (this->LUT);
    }

    this->Mapper->SetScalarRange(this->Min,this->Max);
}

void vtkTensorVisuManager::SetUpLUTToMapScalars()
{
    vtkDataSet* myData = this->GetInput();

    if( !this->Scalars )
    {
        return;
    }

    myData->GetPointData()->SetScalars (this->Scalars);
    this->Glyph->Modified();

    double range[2];
    this->Scalars->GetRange(range);
    std::cout << "Scalar range is: " << range[0] << " " << range[1] << std::endl;

    this->Mapper->SetColorModeToMapScalars();

    if( !this->LUT )
    {
        vtkLookupTable* lut = vtkLookupTable::New();
        lut->SetTableRange(range[0],0.2);
        lut->SetHueRange(0.667,0.0);
        lut->SetSaturationRange(1.0,1.0);
        lut->SetValueRange(1.0,1.0);
        lut->SetNumberOfTableValues(512);
        lut->Build();

        this->SetLookupTable(lut);
        lut->Delete();
    }

    this->Mapper->SetScalarRange(this->Min,this->Max);
}

void vtkTensorVisuManager::SetColorMode(const int& n)
{
    switch(n)
    {
        case 0:
            std::cout << "Color Mode set to eigenvector" << std::endl;
            this->ColorMode = COLOR_BY_EIGENVECTOR;
            break;

        case 1:
            std::cout << "Color Mode set to eigenvalue" << std::endl;
            this->ColorMode = COLOR_BY_EIGENVALUE ;
            break;

        case 2:
            std::cout << "Color Mode set to volume" << std::endl;
            this->ColorMode = COLOR_BY_VOLUME;
            break;

        case 3:
            std::cout << "Color Mode set to trace" << std::endl;
            this->ColorMode = COLOR_BY_TRACE;
            break;

        case 4:
            std::cout << "Color Mode set to distance to identity" << std::endl;
            this->ColorMode = COLOR_BY_DISTANCE_TO_IDENTITY;
            break;

        case 5:
            std::cout << "Color Mode set to scalars" << std::endl;
            this->ColorMode = COLOR_BY_SCALAR;
            break;

        default:
            std::cerr << "Error: Color mode not recognized!" << std::endl;
    }

    this->UpdateLUT();
}

void vtkTensorVisuManager::SetLookupTable (vtkLookupTable* lut)
{
    if( !lut )
    {
        return;
    }

    this->LUT=lut;

    if(this->ColorMode != COLOR_BY_EIGENVECTOR)
    {
        this->Mapper->SetLookupTable (lut);
    }
}

void vtkTensorVisuManager::SetScalarRange(const float& min, const float &max)
{
    this->Min = min;
    this->Max = max;

    if(this->ColorMode != COLOR_BY_EIGENVECTOR)
    {
        this->Mapper->SetScalarRange(min, max);
    }
}
