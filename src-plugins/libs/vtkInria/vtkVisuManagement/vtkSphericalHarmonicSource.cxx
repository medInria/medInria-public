/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vector>
#include <complex>
#include <cmath>

#include <vtkSphericalHarmonicSource.h>

#include <vtkPolyDataNormals.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkMatrix4x4.h>
#include <vtkStreamingDemandDrivenPipeline.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compute spherical associated Legendre function
#if (defined __APPLE__ || defined WIN32 || defined CLANG)
#include <boost/math/special_functions/legendre.hpp>
#else
#include <tr1/cmath>
#endif //WIN32

#if (defined __APPLE__ || defined WIN32 || defined CLANG)
double sphLegendre(int _l, int _m, double theta) {
  double factor = sqrt(((double)(2*_l+1) / (4.0*vtkMath::DoublePi()))*(boost::math::factorial<double>((unsigned int)(_l - _m))
                                                        / boost::math::factorial<double>((unsigned int)(_l + _m))))*boost::math::legendre_p (_l, _m, cos(theta));
    return factor;
}
#else
double sphLegendre(int _l, int _m, double theta) {
    double factor =std::tr1::sph_legendre(_l,_m,theta);
    return factor;
}
#endif //WIN32
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static itk::Vector<double, 3> Cartesian2Spherical(const itk::Vector<double, 3> vITK );

static itk::VariableSizeMatrix<double>
ComputeSHMatrixMaxThesis(const int rank,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                         const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrixTournier(const int rank,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                        const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrixRshBasis(const int rank,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                        const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections);

static itk::VariableSizeMatrix<double>
ComputeSHMatrix (const int rank,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                 const bool FlipZ, itk::VariableSizeMatrix<double>& PhiThetaDirections);


static void
TranslateAndDeformShell(vtkPolyData* shell,vtkPoints* outPts,double center[3],
                        bool deform,vtkMatrix4x4* transform=0);

vtkCxxRevisionMacro(vtkSphericalHarmonicSource,"$Revision: 0 $");
vtkStandardNewMacro(vtkSphericalHarmonicSource);

vtkSphericalHarmonicSource::vtkSphericalHarmonicSource(int tess)
{
  this->Radius = 0.5;
  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;
  this->RotationMatrix = 0;

  this->SetNumberOfInputPorts(0);

  this->DeformOn();
  this->NormalizeOff();
  this->FlipXOff();
  this->FlipYOff();
  // By Default we flip the z-axis, the internal x,y,z have z flipped with respect to visu
  this->FlipZOn();
  this->MaxThesisFuncOff();

  this->TesselationType = Icosahedron;
  this->TesselationBasis = SHMatrix;
  this->Tesselation = tess;
  this->sphereT = vtkTessellatedSphereSource::New();
 this-> SphericalHarmonics = 0;
 this-> Order = 4;
  this->SetNumberOfSphericalHarmonics (15);
}

vtkSphericalHarmonicSource::~vtkSphericalHarmonicSource()
{
  if (this->sphereT)
    this->sphereT->Delete();

  if(this->RotationMatrix)
    this->RotationMatrix->Delete();

  SphericalHarmonics = 0;
}

void vtkSphericalHarmonicSource::SetNumberOfSphericalHarmonics(const int number)
{
  NumberOfSphericalHarmonics = number;
  if (SphericalHarmonics)
    delete[] SphericalHarmonics;
  SphericalHarmonics = new double[NumberOfSphericalHarmonics];
  SphericalHarmonics[0] = 1.0;
  for(int i=1;i<NumberOfSphericalHarmonics;++i)
    SphericalHarmonics[i] = 0.0;

  this->UpdateSphericalHarmonicSource();
  this->Modified();
}

int* vtkSphericalHarmonicSource::GetTesselationRange()
{
  int* range = new int[2];
  GetTesselationRange(range);
  return range;
}

void vtkSphericalHarmonicSource::GetTesselationRange(int *range)
{
  range[0] = 2;
  range[1] = 5;
}

void vtkSphericalHarmonicSource::SetSphericalHarmonicComponent(int i,double v)
{
  if (SphericalHarmonics[i]!=v)
  {
    vtkDebugMacro(<< this->GetClassName() << " (" << this
                  <<"): setting Spherical Harmonics coefficient "<< i <<" to "<< v);
    this->Modified();
    SphericalHarmonics[i] = v;
  }
}

int
vtkSphericalHarmonicSource::
RequestData(vtkInformation *vtkNotUsed(request),vtkInformationVector **vtkNotUsed(inputVector),
            vtkInformationVector *outputVector)
{
  // Get the info and output objects.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPolyData*    output  = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  int N = this->sphereT->GetOutput()->GetNumberOfPoints();
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

  for (int i=0;i<NumberOfSphericalHarmonics;++i){
    C(0,i) = SphericalHarmonics[i];
  }

  itk::VariableSizeMatrix<double> S/*(1,BasisFunction.GetVnlMatrix().size())*/;
  S = C*BasisFunction;

  if (Normalize) {

    double min = S.GetVnlMatrix().min_value();
    double max = S.GetVnlMatrix().max_value();

//    std::cout << "Normalizing by min/max" << min << "/" << max << "\n";

    if (max!=min) {
      for (unsigned i=0; i<S.Rows(); ++i)
        for (unsigned j=0; j<S.Cols(); ++j)
          S(i,j) -= min;
      S /= (max-min);
    } else {
      for (unsigned i=0; i<S.Rows(); ++i)
        for (unsigned j=0; j<S.Cols(); ++j)
          S(i,j) = 1.0;
    }
  }
  S *= this->Radius;

  for(int i=0; i<N; ++i) {
    if  (i%10000==0) {
      this->UpdateProgress ((vtkFloatingPointType)i/N);
      if (this->GetAbortExecute())
        break;
    }
    sValues->SetTuple1(i,S(0,i));
  }

  this->sphereT->GetOutput()->GetPointData()->SetScalars(sValues);
  sValues->Delete();

  // Don't know how to copy everything but the points.
  output->DeepCopy(this->sphereT->GetOutput());
  output->GetPoints()->Reset();

  TranslateAndDeformShell(this->sphereT->GetOutput(),output->GetPoints(),this->Center,Deform,RotationMatrix);

  return 1;
}

void vtkSphericalHarmonicSource::PrintSelf(ostream& os,vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Tessellation Order: " << Tesselation << std::endl;
  os << indent << "Tessellation Type: " << TesselationType << std::endl;
  os << indent << "Tessellation Basis: " << TesselationBasis << std::endl;

  os << indent << "SH Basis Order: " << Order << std::endl;
  os << indent << "Length of SH coefficient vector: "
     << NumberOfSphericalHarmonics << std::endl;
  os << indent << "SH Basis: " << NumberOfSphericalHarmonics << "x"
     << this->sphereT->GetOutput()->GetNumberOfPoints() << std::endl;
  os << indent << "SH Coefficients:" << std::endl << '[';
  for (int i=0;i<NumberOfSphericalHarmonics;++i)
    os << indent << SphericalHarmonics[i] << " ";
  os << indent << ']' << std::endl;
}

int vtkSphericalHarmonicSource::RequestInformation(vtkInformation*,vtkInformationVector**,
                                                   vtkInformationVector *outputVector)
{
  // Get the info object
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_BOUNDING_BOX(),
               this->Center[0]-this->Radius,this->Center[0]+this->Radius,
               this->Center[1]-this->Radius,this->Center[1]+this->Radius,
               this->Center[2]-this->Radius,this->Center[2]+this->Radius);

  return 1;
}

itk::Vector<double, 3> Cartesian2Spherical(const itk::Vector<double, 3> vITK)
{
  const double r =vITK.GetNorm();
  if (r==0) {
    std::cerr << "Cannot have a 0 radius in spherical coordinates!\n" << std::endl;
    exit(1);
  }
  // r, phi [0, PI), theta [0, PI) computation
  const double spherical [3] = { vITK.GetNorm(), atan2(vITK[1],vITK[0]), acos(vITK[2]/r)};
  const itk::Vector<double, 3> s (spherical);

  return s;
}


/*
  Compute SH matrix for discrete samplings on the sphere
  rank := number of spherical harmonics (rank of HOT in Reseach report 5681)
  n_s  := number of spherical values on the sphere (Research report 5681)
*/
itk::VariableSizeMatrix<double>
ComputeSHMatrix(const int order,vtkPolyData* shell,const bool FlipX,const bool FlipY,
                const bool FlipZ, itk::VariableSizeMatrix<double>& PhiThetaDirections)
{
  const int n_s   = shell->GetNumberOfPoints();
  const int rank =  (order+1)*(order+2)/2;
  itk::VariableSizeMatrix<double> B(rank,n_s);
  vtkPoints* vertices = shell->GetPoints();

  for (int i=0;i<n_s;++i) {
    // Get spherical component of the point direction and transform them in spherical coordinates.
    double p[3];
    vertices->GetPoint(i,p);

    itk::Vector<double, 3> dITK;
    dITK[0] = (FlipX) ? -p[0] : p[0];
    dITK[1] = (FlipY) ? -p[1] : p[1];
    dITK[2] = (FlipZ) ? -p[2] : p[2];

    const itk::Vector<double, 3> v = Cartesian2Spherical(dITK);

    const double phi   = v[1];
    const double theta = v[2];
    double temp=0;
    PhiThetaDirections(i,0) = phi;
    PhiThetaDirections(i,1) = theta;

    for (int l=0,j=0;l<=order;l+=2) {

      //  Handle the case m=0
      B(j,i) = sphLegendre(l,0,theta);
      j = j+1;

      for(int m=1,s=-1;m<=l;++m,++j,s=-s) {
        temp = sphLegendre(l, m,theta)*std::sqrt(2.0);

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
ComputeSHMatrixMaxThesis(const int order,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                         const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections)
{
  const int n_s   = shell->GetNumberOfPoints();
  const int rank =  (order+1)*(order+2)/2;
  itk::VariableSizeMatrix<double> B(rank,n_s);
  vtkPoints* vertices = shell->GetPoints();
  for (int i=0;i<n_s;++i) {

    // Get spherical component of the point direction and transform them in spherical coordinates.
    double p[3];
    vertices->GetPoint(i,p);

    itk::Vector<double, 3> dITK;
    dITK[0] = (FlipX) ? -p[0] : p[0];
    dITK[1] = (FlipY) ? -p[1] : p[1];
    dITK[2] = (FlipZ) ? -p[2] : p[2];

    const itk::Vector<double, 3> v = Cartesian2Spherical(dITK);
    const double phi   = v[1];// v.y;
    const double theta = v[2];//v.z;

    PhiThetaDirections(i,0) = phi;
    PhiThetaDirections(i,1) = theta;

    //  It is even nicer to compute the SH once (for m>0 and for m<0).
    //  The central term is given by the suite u_n
    //  TO
    double temp=0;
    for (int l=0,j=0;l<=order;l+=2,j+=2*l-1) {
      B(j,i) = sphLegendre(l,0,theta);

      for(int m=1,j1=j-1,j2=j+1;m<=l;++m,--j1,++j2) {
        temp = std::sqrt(2.0)*sphLegendre(l,m,theta);
        B(j1,i) = temp*cos(m*phi);
        B(j2,i) = temp*sin(m*phi);
      }
    }
  }

  return B;
}

itk::VariableSizeMatrix<double>
ComputeSHMatrixTournier(const int order,vtkPolyData *shell,const bool FlipX,const bool FlipY,
                        const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections)
{
  const int n_s   = shell->GetNumberOfPoints();
  const int rank =  (order+1)*(order+2)/2;
  /*
      We declare the Bmatrix of size n_s x n_b.
    */
  itk::VariableSizeMatrix<double> B(rank,n_s);

  vtkPoints* vertices = shell->GetPoints();
  std::complex<float>  cplx_1;

  for(int i = 0; i < n_s; i++) {
    // Get spherical component of the point direction and transform them in spherical coordinates.
    double p[3];
    vertices->GetPoint(i,p);

    itk::Vector<double, 3> dITK;
    dITK[0] = (FlipX) ? -p[0] : p[0];
    dITK[1] = (FlipY) ? -p[1] : p[1];
    dITK[2] = (FlipZ) ? -p[2] : p[2];

    const itk::Vector<double, 3> v = Cartesian2Spherical(dITK);
    const double phi   = v[1]; //v.y;
    const double theta = v[2]; //v.z;

    PhiThetaDirections(i,0) = phi;
    PhiThetaDirections(i,1) = theta;
    int j = 0;
    //counter for the j dimension of B
    //get spherical component of the direction vector
    double temp=0;

    for(int l = 0; l <= order; l+=2)
      for(int m = -l,s=1; m <= l; m++,s=-s) {
        temp = sphLegendre(l,std::abs(m),theta);
        if(m >= 0) { /* positive "m" SH */
          B(j,i) =  temp*cos(m*phi);;
        }
        else { /* negative "m" SH  */
          B(j,i) = s*temp*sin(m*phi);
        }
        j++;
      }
  }

  return B;
}

itk::VariableSizeMatrix<double>
ComputeSHMatrixRshBasis(const int order,vtkPolyData* shell,const bool FlipX,const bool FlipY,
                        const bool FlipZ,itk::VariableSizeMatrix<double>& PhiThetaDirections)
{
  const int n_s   = shell->GetNumberOfPoints();
  const int rank =  (order+1)*(order+2)/2;

  itk::VariableSizeMatrix<double> B(rank,n_s);
  vtkPoints* vertices = shell->GetPoints();

  for (int i=0;i<n_s;++i) {

    // Get spherical component of the point direction and transform them in spherical coordinates.
    double p[3];
    vertices->GetPoint(i,p);

    itk::Vector<double, 3> dITK;
    dITK[0] = (FlipX) ? -p[0] : p[0];
    dITK[1] = (FlipY) ? -p[1] : p[1];
    dITK[2] = (FlipZ) ? -p[2] : p[2];

    const itk::Vector<double, 3> v = Cartesian2Spherical(dITK);
    const double phi   = v[1]; //v.y;
    const double theta = v[2]; // v.z;
    PhiThetaDirections(i,0) = phi;
    PhiThetaDirections(i,1) = theta;
    double temp=0;

    for (int l=0,j=0;l<=order;l+=2) {
      B(j,i)=sphLegendre(l,0,theta);
      j=j+1;
      for(int m=1,s=-1;m<=l;++m,++j,s=-s) {
        temp = sphLegendre(l, m,theta)*std::sqrt(2.0);
        //-m Real like RshBasis.pdf Luke Bloy eq 1.2 but math simplified and with tr1
        B(j,i)   = temp*(cos(m*phi));
        //+m Imag
        B(++j,i) = temp*(sin(m*phi));
      }
    }
  }
  return B;
}

void vtkSphericalHarmonicSource::SetSphericalHarmonics(double* _arg)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this
                << "): setting Spherical Harmonics to " << _arg);
  this->SphericalHarmonics = _arg;
  this->Modified();
}

void vtkSphericalHarmonicSource::UpdateSphericalHarmonicSource()
{
  this->sphereT->SetPolyhedraType(TesselationType);
  this->sphereT->SetResolution(Tesselation);
  this->sphereT->Update();

  itk::VariableSizeMatrix<double>  PhiThetaDirection(this->sphereT->GetOutput()->GetNumberOfPoints(),2);

  switch (TesselationBasis) {
  case SHMatrix:
  {
    BasisFunction = ComputeSHMatrix(Order,this->sphereT->GetOutput(),FlipX,FlipY,FlipZ,PhiThetaDirection);
    break;
  }
  case SHMatrixMaxThesis:
  {
    BasisFunction = ComputeSHMatrixMaxThesis(Order,this->sphereT->GetOutput(),FlipX,FlipY,FlipZ,PhiThetaDirection);
    break;
  }
  case SHMatrixTournier:
  {
    BasisFunction = ComputeSHMatrixTournier(Order,this->sphereT->GetOutput(),FlipX,FlipY,FlipZ,PhiThetaDirection);
    break;
  }
  case SHMatrixRshBasis:
  {
    BasisFunction = ComputeSHMatrixRshBasis(Order,this->sphereT->GetOutput(),FlipX,FlipY,FlipZ,PhiThetaDirection);
    break;
  }
  }
  PhiThetaShellDirections = PhiThetaDirection;


}

void TranslateAndDeformShell(vtkPolyData *shell,vtkPoints* outPts,double center[3],
                             bool deform,vtkMatrix4x4* transform)
{
  vtkPoints* inPts = shell->GetPoints();
  const int  n     = inPts->GetNumberOfPoints();

//  double range[2];
//  shell->GetPointData()->GetScalars()->GetRange(range);
//  const double rangeDiff = (range[0]!=range[1]) ? range[1]-range[0] : 1.;

  vtkDataArray* sValues  = shell->GetPointData()->GetScalars();

  for (int i=0;i<n;++i) {
    double point[4];
    inPts->GetPoint(i,point);

    if (deform) {
      const double val = sValues->GetTuple1(i);
      point[0] = (val)*point[0];// ((val-range[0])/rangeDiff)*point[0];
      point[1] = (val)*point[1];//((val-range[0])/rangeDiff)*point[1];
      point[2] =(val)*point[2];// ((val-range[0])/rangeDiff)*point[2];
    }
    point[3] = 1.0;
    const double* pointOut = (transform!=0) ? transform->MultiplyDoublePoint(point) : &point[0];
    outPts->InsertNextPoint(pointOut[0]+center[0],pointOut[1]+center[1],pointOut[2]+center[2]);
  }
}
