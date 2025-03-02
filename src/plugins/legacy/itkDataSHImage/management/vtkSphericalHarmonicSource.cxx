/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <qmath.h>

#include <vtkSphericalHarmonicSource.h>

#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkMatrix4x4.h>

/**
 * @brief Computes the associated Legendre function P_l^m(cos(theta)).
 * 
 * The associated Legendre functions are solutions to Legendre's differential
 * equation and are used in applications such as spherical harmonics.
 * This implementation uses a recurrence relation for efficient computation.
 * 
 * @param l The degree (l >= 0).
 * @param m The order (|m| <= l).
 * @param theta The polar angle in radians (0 <= theta <= π).
 * @return The value of the associated Legendre function P_l^m(cos(theta)).
 * 
 * @throws std::invalid_argument if l < 0, |m| > l, or theta is out of range.
 * 
 * @note This function assumes double precision for floating-point computations.
 */
double sphLegendre(const int l, const int m, const double theta)
{
    //when we will used macOs that support std::sph_legendre, we can use std::sph_legendre
    if (l < 0 || std::abs(m) > l || theta < 0.0 || theta > M_PI)
    {
        throw std::invalid_argument("Invalid arguments for Legendre function");
    }

    // Compute cos(theta) and sin(theta)
    const double x = std::cos(theta);
    const double sinTheta = std::sin(theta);

    // Base case: P_0^0(x) = 1
    if (l == 0)
    {
        return 1.0;
    }

    // Base case: P_1^0(x) = x
    if (l == 1 && m == 0)
    {
        return x;
    }

    // Compute P_m^m(x) (diagonal terms)
    double pmm = 1.0;
    for (int i = 1; i <= std::abs(m); ++i)
    {
        pmm *= -1.0 * (2.0 * i - 1.0) * sinTheta;
    }

    if (l == m)
    {
        return pmm;
    }

    // Compute P_{m+1}^m(x)
    double pmmp1 = x * (2.0 * m + 1.0) * pmm;
    if (l == m + 1)
    {
        return pmmp1;
    }

    // Compute P_l^m(x) using recurrence relation
    double pll = 0.0;
    for (int ll = m + 2; ll <= l; ++ll)
    {
        pll = ((2.0 * ll - 1.0) * x * pmmp1 - (ll + m - 1.0) * pmm) / (ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }

    return pll;
}

typedef itk::Vector<double,3> itkVector3;

static itkVector3 Cartesian2Spherical(const itkVector3 vITK) {

    const double r = vITK.GetNorm();
    if (r==0) {
        std::cerr << "Warning: 0 radius in spherical coordinates!\n" << std::endl;
        const double spherical [3] = { 0.0, 0.0, 0.0};
        return itkVector3(spherical);
    }

    // r, phi [0, PI), theta [0, PI) computation

    const double spherical [3] = { vITK.GetNorm(), atan2(vITK[1],vITK[0]), acos(vITK[2]/r)};
    return itkVector3(spherical);
}

static itkVector3 get_flipped_point_spherical_coords(vtkPoints* vertices,const int i,const bool* Flip) {
    double p[3];
    vertices->GetPoint(i,p);
    for (int i=0;i<3;++i)
        p[i] = (Flip[i]) ? -p[i] : p[i];
    return Cartesian2Spherical(itkVector3(p));
}

static itk::VariableSizeMatrix<double>
ComputeSHMatrixMaxThesis(const int rank,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrixTournier(const int rank,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrixRshBasis(const int rank,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrix (const int rank,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static void
TranslateAndDeformShell(vtkPolyData* shell,vtkPoints* outPts,double center[3],
                        bool deform,vtkMatrix4x4* transform=0);


vtkStandardNewMacro(vtkSphericalHarmonicSource);

vtkSphericalHarmonicSource::vtkSphericalHarmonicSource(const int tess) {
    Radius = 0.5;
    Center[0] = 0.0;
    Center[1] = 0.0;
    Center[2] = 0.0;
    RotationMatrix = 0;

    SetNumberOfInputPorts(0);

    DeformOn();
    NormalizeOff();

    SetFlipVector(false,false,false);
    MaxThesisFuncOff();

    TesselationType = Icosahedron;
    TesselationBasis = SHMatrix;
    Tesselation = tess;

    sphereT = vtkTessellatedSphereSource::New();
    sphereT->SetResolution(Tesselation);

    Order = 4;

    SphericalHarmonics = 0;
    SetNumberOfSphericalHarmonics (15);
}

vtkSphericalHarmonicSource::~vtkSphericalHarmonicSource() {

    if (sphereT)
        sphereT->Delete();

    if(RotationMatrix)
        RotationMatrix->Delete();

    if (SphericalHarmonics)
        delete[] SphericalHarmonics;

    SphericalHarmonics = 0;
}

void vtkSphericalHarmonicSource::SetNumberOfSphericalHarmonics(const int number) {

    NumberOfSphericalHarmonics = number;
    if (SphericalHarmonics)
        delete[] SphericalHarmonics;
    SphericalHarmonics = new double[NumberOfSphericalHarmonics];
    SphericalHarmonics[0] = 1.0;
    for (int i=1;i<NumberOfSphericalHarmonics;++i)
        SphericalHarmonics[i] = 0.0;

    UpdateSphericalHarmonicSource();
    Modified();
}

int* vtkSphericalHarmonicSource::GetTesselationRange() {
    int* range = new int[2];
    GetTesselationRange(range);
    return range;
}

void vtkSphericalHarmonicSource::GetTesselationRange(int *range) {
    range[0] = 2;
    range[1] = 5;
}

void vtkSphericalHarmonicSource::SetSphericalHarmonicComponent(int i,double v) {
    if (SphericalHarmonics[i]!=v) {
        vtkDebugMacro(<< GetClassName() << " (" << this
                      <<"): setting Spherical Harmonics coefficient "<< i <<" to "<< v);
        Modified();
        SphericalHarmonics[i] = v;
    }
}

int
vtkSphericalHarmonicSource::
RequestData(vtkInformation *vtkNotUsed(request),vtkInformationVector **vtkNotUsed(inputVector),vtkInformationVector *outputVector) {

    // Get the info and output objects.

    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkPolyData*    output  = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    const int       N       = sphereT->GetOutput()->GetNumberOfPoints();

    // SH Source stuff.

    vtkFloatArray* sValues = vtkFloatArray::New();
    sValues->SetNumberOfTuples(N);
    sValues->SetNumberOfComponents(1);

    /* Project data on the sphere:
    In matrix form:
    S := spherical function (discrete data on the sphere)
    C := Spherical harmonic coefficients
    B := Basis function
    S = CB
    */

    itk::VariableSizeMatrix<double> C(1,NumberOfSphericalHarmonics);

    for (int i=0;i<NumberOfSphericalHarmonics;++i)
        C(0,i) = SphericalHarmonics[i];

    itk::VariableSizeMatrix<double> S = C*BasisFunction;

    if (Normalize) {

        const double min = S.GetVnlMatrix().min_value();
        const double max = S.GetVnlMatrix().max_value();

        if (max!=min) {
            for (unsigned i=0; i<S.Rows(); ++i)
                for (unsigned j=0;j<S.Cols(); ++j)
                    S(i,j) -= min;
            S /= (max-min);
        } else
            for (unsigned i=0; i<S.Rows(); ++i)
                for (unsigned j=0;j<S.Cols(); ++j)
                    S(i,j) = 1.0;
    }
    S *= Radius;

    for (int i=0; i<N; ++i) {
        if (i%10000==0) {
            UpdateProgress ((double)i/N);
            if (GetAbortExecute())
                break;
        }
        sValues->SetTuple1(i,S(0,i));
    }

    sphereT->GetOutput()->GetPointData()->SetScalars(sValues);
    sValues->Delete();

    // Don't know how to copy everything but the points.
    output->DeepCopy(sphereT->GetOutput());
    output->GetPoints()->Reset();

    TranslateAndDeformShell(sphereT->GetOutput(),output->GetPoints(),Center,Deform,RotationMatrix);

    return 1;
}

void vtkSphericalHarmonicSource::PrintSelf(ostream& os,vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);

    os << indent << "Tessellation Order: " << Tesselation << std::endl;
    os << indent << "Tessellation Type: " << TesselationType << std::endl;
    os << indent << "Tessellation Basis: " << TesselationBasis << std::endl;

    os << indent << "SH Basis Order: " << Order << std::endl;
    os << indent << "Length of SH coefficient vector: "
       << NumberOfSphericalHarmonics << std::endl;
    os << indent << "SH Basis: " << NumberOfSphericalHarmonics << "x"
       << sphereT->GetOutput()->GetNumberOfPoints() << std::endl;

    os << indent << "SH Coefficients:" << std::endl << '[';
    for (int i=0;i<NumberOfSphericalHarmonics;++i)
    {
        os << indent << SphericalHarmonics[i] << " ";
    }
    os << indent << ']' << std::endl;
}

int vtkSphericalHarmonicSource::RequestInformation(vtkInformation*,vtkInformationVector**,vtkInformationVector *outputVector) {
    // Get the info object
    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    outInfo->Set(CAN_HANDLE_PIECE_REQUEST(),
                 1);

    return 1;
}

/*
  Compute SH matrix for discrete samplings on the sphere
  rank := number of spherical harmonics (rank of HOT in Reseach report 5681)
  n_s  := number of spherical values on the sphere (Research report 5681)
*/
itk::VariableSizeMatrix<double>
ComputeSHMatrix(const int order,vtkPolyData* shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections)
{
    const int n_s   = shell->GetNumberOfPoints();
    const int rank =  (order+1)*(order+2)/2;
    itk::VariableSizeMatrix<double> B(rank,n_s);
    vtkPoints* vertices = shell->GetPoints();

    for (int i=0;i<n_s;++i) {

        // Get spherical component of the point direction and transform them in spherical coordinates.

        const itkVector3 v = get_flipped_point_spherical_coords(vertices,i,FlipVector);

        const double phi   = PhiThetaDirections(i,0) = v[1];
        const double theta = PhiThetaDirections(i,1) = v[2];

        for (int l=0,j=0;l<=order;l+=2) {

            //  Handle the case m=0
            B(j++,i) = sphLegendre(l,0,theta);

            for (int m=1,s=-1;m<=l;++m,++j,s=-s) {
                const double temp = sphLegendre(l, m,theta)*std::sqrt(2.0);

                //-m Real like t3 at hardi.cpp but math simplified and with tr1
                B(j,i) = s*temp*(cos(m*phi));

                //+m Imag like t3 at hardi.cpp but math simplified and with tr1
                B(++j,i) = temp*(sin(s*m*phi));
            }
        }
    }

    return B;
}

itk::VariableSizeMatrix<double>
ComputeSHMatrixMaxThesis(const int order,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections) {

    const int n_s  = shell->GetNumberOfPoints();
    const int rank = (order+1)*(order+2)/2;
    itk::VariableSizeMatrix<double> B(rank,n_s);
    vtkPoints* vertices = shell->GetPoints();
    for (int i=0;i<n_s;++i) {

        // Get spherical component of the point direction and transform them in spherical coordinates.

        const itkVector3 v = get_flipped_point_spherical_coords(vertices,i,FlipVector);

        const double phi   = PhiThetaDirections(i,0) = v[1];
        const double theta = PhiThetaDirections(i,1) = v[2];

        //  It is even nicer to compute the SH once (for m>0 and for m<0).
        //  The central term is given by the suite u_n
        //  TO

        for (int l=0,j=0;l<=order;l+=2,j+=2*l-1) {
            B(j,i) = sphLegendre(l,0,theta);

            for (int m=1,j1=j-1,j2=j+1;m<=l;++m,--j1,++j2) {
                const double temp = std::sqrt(2.0)*sphLegendre(l,m,theta);
                if (m % 2 != 0)
                    B(j1,i) = - temp*cos(m*phi);
                else
                    B(j1,i) = temp*cos(m*phi);
                B(j2,i) = temp*sin(m*phi);
            }
        }
    }

    return B;
}

itk::VariableSizeMatrix<double>
ComputeSHMatrixTournier(const int order,vtkPolyData *shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections) {

    const int n_s  = shell->GetNumberOfPoints();
    const int rank = (order+1)*(order+2)/2;
    /*
      We declare the Bmatrix of size n_s x n_b.
    */
    itk::VariableSizeMatrix<double> B(rank,n_s);
    vtkPoints* vertices = shell->GetPoints();

    for (int i = 0; i < n_s;++i) {

        // Get spherical component of the point direction and transform them in spherical coordinates.

        const itkVector3 v = get_flipped_point_spherical_coords(vertices,i,FlipVector);

        const double phi   = PhiThetaDirections(i,0) = v[1];
        const double theta = PhiThetaDirections(i,1) = v[2];

        for (int l=0,j=0;l<=order;l+=2)
            for (int m=-l,s=1;m<=l;++m,s=-s,++j) {
                const double temp = sphLegendre(l,std::abs(m),theta);
                B(j,i) = temp*((m>=0) ? cos(m*phi) : s*sin(m*phi));
            }
    }

    return B;
}

itk::VariableSizeMatrix<double>
ComputeSHMatrixRshBasis(const int order,vtkPolyData* shell,const bool* FlipVector,itk::VariableSizeMatrix<double>& PhiThetaDirections) {

    const int n_s   = shell->GetNumberOfPoints();
    const int rank =  (order+1)*(order+2)/2;

    itk::VariableSizeMatrix<double> B(rank,n_s);
    vtkPoints* vertices = shell->GetPoints();

    for (int i=0;i<n_s;++i) {

        // Get spherical component of the point direction and transform them in spherical coordinates.

        const itkVector3 v = get_flipped_point_spherical_coords(vertices,i,FlipVector);

        const double phi   = PhiThetaDirections(i,0) = v[1];
        const double theta = PhiThetaDirections(i,1) = v[2];

        for (int l=0,j=0;l<=order;l+=2) {
            B(j++,i) = sphLegendre(l,0,theta);
            for (int m=1,s=-1;m<=l;++m,++j,s=-s) {
                const double temp = sphLegendre(l, m,theta)*std::sqrt(2.0);
                //-m Real like RshBasis.pdf Luke Bloy eq 1.2 but math simplified and with tr1
                B(j,i)   = temp*(cos(m*phi));
                //+m Imag
                B(++j,i) = temp*(sin(m*phi));
            }
        }
    }
    return B;
}

void vtkSphericalHarmonicSource::SetSphericalHarmonics(double* _arg) {
    vtkDebugMacro(<< GetClassName() << " (" << this << "): setting Spherical Harmonics to " << _arg);
    SphericalHarmonics = _arg;
    Modified();
}

void vtkSphericalHarmonicSource::UpdateSphericalHarmonicSource() {

    sphereT->SetPolyhedraType(TesselationType);
    sphereT->SetResolution(Tesselation);
    sphereT->Update();

    itk::VariableSizeMatrix<double>  PhiThetaDirection(sphereT->GetOutput()->GetNumberOfPoints(),2);

    switch (TesselationBasis) {
        case SHMatrix:
            BasisFunction = ComputeSHMatrix(Order,sphereT->GetOutput(),FlipVector,PhiThetaDirection);
            break;
        case SHMatrixMaxThesis:
            BasisFunction = ComputeSHMatrixMaxThesis(Order,sphereT->GetOutput(),FlipVector,PhiThetaDirection);
            break;
        case SHMatrixTournier:
            BasisFunction = ComputeSHMatrixTournier(Order,sphereT->GetOutput(),FlipVector,PhiThetaDirection);
            break;
        case SHMatrixRshBasis:
            BasisFunction = ComputeSHMatrixRshBasis(Order,sphereT->GetOutput(),FlipVector,PhiThetaDirection);
            break;
    }
    PhiThetaShellDirections = PhiThetaDirection;

    Modified();
}

void
TranslateAndDeformShell(vtkPolyData *shell,vtkPoints* outPts,double center[3],bool deform,vtkMatrix4x4* transform) {
    vtkPoints* inPts = shell->GetPoints();
    const int  n     = inPts->GetNumberOfPoints();

    vtkDataArray* sValues  = shell->GetPointData()->GetScalars();

    for (int i=0;i<n;++i) {
        double point[4];
        inPts->GetPoint(i,point);

        if (deform) {
            const double val = sValues->GetTuple1(i);
            point[0] = val*point[0]; // ((val-range[0])/rangeDiff)*point[0];
            point[1] = val*point[1]; // ((val-range[0])/rangeDiff)*point[1];
            point[2] = val*point[2]; // ((val-range[0])/rangeDiff)*point[2];
        }
        point[3] = 1.0;
        const double* pointOut = (transform!=0) ? transform->MultiplyDoublePoint(point) : &point[0];
        outPts->InsertNextPoint(pointOut[0]+center[0],pointOut[1]+center[1],pointOut[2]+center[2]);
    }
}
