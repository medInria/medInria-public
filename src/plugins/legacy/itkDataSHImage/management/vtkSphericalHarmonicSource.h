#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkPolyDataAlgorithm.h>
#include <vtkMatrix4x4.h>

#include <itkVariableSizeMatrix.h>
#include <itkVector.h>
#include <vtkTessellatedSphereSource.h>

class vtkMatrix4x4;

class vtkSphericalHarmonicSource: public vtkPolyDataAlgorithm {
public:
    vtkTypeMacro(vtkSphericalHarmonicSource,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os,vtkIndent indent);

    static vtkSphericalHarmonicSource* New();

    vtkSetMacro(Tesselation,int);
    vtkGetMacro(Tesselation,int);
    
    int* GetTesselationRange();
    void GetTesselationRange(int*);
    
    /** Set Type of tesselation : cube, dodecahedron, icosahedron, octahedron, tetrahedron
      * determines the angular sampling scheme*/
    vtkSetMacro(TesselationType,int);

    /** Get Type of tesselation : cube, dodecahedron, icosahedron, octahedron, tetrahedron
      * determines the angular sampling scheme*/
    vtkGetMacro(TesselationType,int);

    /** Set Type of basis : SHMatrix=0, SHMatrixMaxThesis=1, SHMatrixTournier=2, SHMatrixRshBasis=3,
      * determines the Spherical harmonics basis to be used*/
    vtkSetMacro(TesselationBasis,int);

    /** Get Type of basis : SHMatrix=0, SHMatrixMaxThesis=1, SHMatrixTournier=2, SHMatrixRshBasis=3,
      * determines the Spherical harmonics basis to be used*/
    vtkGetMacro(TesselationBasis,int);

    //BTX
    enum {
        Icosahedron=0,
        Dodecahedron,
        Octahedron ,
        Cube,
        Tetrahedron
    };
    //ETX

    enum {
        SHMatrix = 0,
        SHMatrixMaxThesis = 1,
        SHMatrixTournier = 2,
        SHMatrixRshBasis = 3
    };

    void    SetSphericalHarmonics(double* coeff);
    double* GetSphericalHaarmonics() { return SphericalHarmonics; }

    void SetSphericalHarmonicComponent(int i,double v);
    void SetSphericalHarmonicComponent(size_t i,double v[]);
    double GetSphericalHarmonicComponent(int i) { return SphericalHarmonics[i]; }

    /** Get Order of the spherical harmonic basis*/

    vtkGetMacro(Order,int);

    /** Set Order of the spherical harmonic basis*/

    vtkSetMacro(Order,int);

    vtkGetMacro(NumberOfSphericalHarmonics,int);
    void SetNumberOfSphericalHarmonics(const int);
    
    vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
    vtkGetMacro(Radius,double);

    vtkSetVector3Macro(Center,double);
    vtkGetVectorMacro(Center,double,3);

    /** Set Deform On/Off
      * If On, the radius of the sphere is deformed according to the spherical value
      * Else, the data is visualized on a sphere of radius 1*/

    vtkSetMacro(Deform,bool);

    /** Deform On/Off
      * If On, the radius of the sphere is deformed according to the spherical value
      * Else, the data is visualized on a sphere of radius 1*/

    vtkBooleanMacro(Deform,bool);

    /** Get Deform On/Off
      * If On, the radius of the sphere is deformed according to the spherical value
      * Else, the data is visualized on a sphere of radius 1*/

    vtkGetMacro(Deform,bool);

    /** Set Normalize On/Off
      *  If On, spherical values are (0,1) normalized*/

    vtkSetMacro(Normalize,bool);

    /** Set Normalize On/Off
      *  If On, spherical values are (0,1) normalized*/

    vtkBooleanMacro(Normalize,bool);

    /** Get Normalize On/Off
      *  If On, spherical values are (0,1) normalized*/

    vtkGetMacro(Normalize,bool);
    
    /** Set FlipVector On/Off If On, x direction is flipped in the
      * reconstruction directions defining the basis function*/

    vtkSetVector3Macro(FlipVector, bool);

    /** Get FlipVector On/Off If On, x direction is flipped in the reconstruction
      * directions defining the basis function*/

    vtkGetVector3Macro(FlipVector,bool);

    vtkSetMacro(MaxThesisFunc,bool);
    vtkBooleanMacro(MaxThesisFunc,bool);
    vtkGetMacro(MaxThesisFunc,bool);

    /** Set the rotation matrix that defines the individual tesselated deformed
      *  sphere pose. virtual*/

    vtkSetObjectMacro(RotationMatrix,vtkMatrix4x4);

    /** Get the rotation matrix that defines the individual tesselated deformed
      *  sphere pose. virtual*/

    vtkGetObjectMacro(RotationMatrix,vtkMatrix4x4);

    void UpdateSphericalHarmonicSource();

    //     /** Function constructing the Spherical Harmonic function with the given
    //      *  directions text file.  At this point, the number of directions needs
    //      *  to match the Tesselation. i.e. Tesselation = 3 -> 81 directions
    //      *  Tesselation = 4 -> 321 directions, Tesselation = 5 -> 1281 directions.
    //      *  @param dirs a string to a text file containing the number of directions
    //      *  and the directions in cartesian coordinates
    //      *  To do: implement a PolyData with arbitrary dirs */

    //      void SetReconstructionDirections(const std::string& dirs);
    
protected:

     vtkSphericalHarmonicSource(const int tess=2);
    ~vtkSphericalHarmonicSource();
    
    int RequestData(vtkInformation*,vtkInformationVector**,vtkInformationVector*);
    int RequestInformation(vtkInformation*,vtkInformationVector**,vtkInformationVector*);

    double Radius;
    double Center[3];
    
    /** Spherical function */
    //    vtkPolyData* shell;

    vtkTessellatedSphereSource *sphereT;

    /** Spherical harmonic basis function*/

    itk::VariableSizeMatrix<double> BasisFunction;

    /** Order of the spherical harmonic basis*/

    int Order;

    /** Spherical harmonic representation of the spherical function*/

    double* SphericalHarmonics;
    int     NumberOfSphericalHarmonics;

    /** Type of tesselation : cube, dodecahedron, icosahedron, octahedron, tetrahedron
      * determines the angular sampling scheme*/

    int TesselationType;

    /** Type of basis : SHMatrix=0, SHMatrixMaxThesis=1, SHMatrixTournier=2, SHMatrixRshBasis=3,
      * determines the Spherical harmonics basis to be used*/

    int TesselationBasis;

    /** Tesselation order, determines the angular sampling*/

    int Tesselation;

    /** Deform On/Off
      * If On, the radius of the sphere is deformed according to the spherical value
      * Else, the data is visualized on a sphere of radius 1*/

    bool Deform;
    
    /** Normalize On/Off
      *  If On, spherical values are (0,1) normalized*/

    bool Normalize;
    
    /** FlipVector {X,Y,Z} On/Off
      * If On, the correponding direction is flipped in the reconstruction directions defining
      * the basis function By default we need to flip Z to have glyphs correctly aligned.      */

    bool FlipVector[3];

    /** MaximeDescoteaux Thesis On/Off
      * If On, spherical values are (0,1) normalized*/

    bool MaxThesisFunc;

    itk::VariableSizeMatrix<double> PhiThetaShellDirections;
    vtkMatrix4x4*                   RotationMatrix;

private:

    vtkSphericalHarmonicSource(const vtkSphericalHarmonicSource&);  // Not implemented.
    void operator=(const vtkSphericalHarmonicSource&);  // Not implemented.
};
