//  Author(s):           Maxime Descoteaux
//  
//  
//  Description:     SH == SphericalHarmonic
//  Spherical function viewer when parameterised with a SH basis
//  Tesselation decides the resolution of the sphere
//  
//  assumption: icosahedron polydata
//  
//  In charge of drawing a single spherical function
//  
//  Copyright (c) 2007 by Maxime Descoteaux, Odyssee Group, INRIA
//  Sophia Antipolis, France.  Please see the copyright notice
//  included in this distribution for full details.

#ifndef VTKSPHERICALHARMONICSOURCE_HPP
#define VTKSPHERICALHARMONICSOURCE_HPP

#include <vtkPolyDataAlgorithm.h>
#include <vtkSphereTesselator.h>

#include <vtkMatrix4x4.h>

#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

class vtkMatrix4x4;

class VTK_VISUMANAGEMENT_EXPORT vtkSphericalHarmonicSource: public vtkPolyDataAlgorithm {
public:

    vtkTypeRevisionMacro(vtkSphericalHarmonicSource,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os,vtkIndent indent);

    static vtkSphericalHarmonicSource* New();

    vtkSetMacro(Tesselation,int);
    vtkGetMacro(Tesselation,int);
    
    int* GetTesselationRange();
    void GetTesselationRange(int*);
    
    vtkSetMacro(TesselationType,int);
    vtkGetMacro(TesselationType,int);     

    vtkSetMacro(TesselationBasis,int);
    vtkGetMacro(TesselationBasis,int);


    //BTX
    enum {
        Cube=0,    
        Dodecahedron = 1,
        Icosahedron = 2,
        Octahedron = 3,
        Tetrahedron = 4
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
    
    vtkGetMacro(Order,int);
    vtkSetMacro(Order,int);

    vtkGetMacro(NumberOfSphericalHarmonics,int);
    void SetNumberOfSphericalHarmonics(const int);
    
    vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
    vtkGetMacro(Radius,double);

    vtkSetVector3Macro(Center,double);
    vtkGetVectorMacro(Center,double,3);

    vtkSetMacro(Deform,bool);
    vtkBooleanMacro(Deform,bool);
    vtkGetMacro(Deform,bool);

    vtkSetMacro(Normalize,bool);
    vtkBooleanMacro(Normalize,bool);
    vtkGetMacro(Normalize,bool);
    
    vtkSetMacro(FlipX,bool);
    vtkBooleanMacro(FlipX,bool);
    vtkGetMacro(FlipX,bool);

    vtkSetMacro(FlipY,bool);
    vtkBooleanMacro(FlipY,bool);
    vtkGetMacro(FlipY,bool);
    
    vtkSetMacro(FlipZ,bool);
    vtkBooleanMacro(FlipZ,bool);
    vtkGetMacro(FlipZ,bool);

    vtkSetMacro(MaxThesisFunc,bool);
    vtkBooleanMacro(MaxThesisFunc,bool);
    vtkGetMacro(MaxThesisFunc,bool);


    virtual void SetRotationMatrix(vtkMatrix4x4*);
    vtkGetObjectMacro(RotationMatrix,vtkMatrix4x4);
    
    void UpdateSphericalHarmonicSource();

    // Function constructing the Spherical Harmonic function with the given
    // directions text file.  At this point, the number of directions needs 
    // to match the Tesselation. i.e. Tesselation = 3 -> 81 directions
    // Tesselation = 4 -> 321 directions, Tesselation = 5 -> 1281 directions.
    //
    // @param dirs a string to a text file containing the number of directions
    // and the directions in cartesian coordinates
    //
    // To do: implement a PolyData with arbitrary dirs

    //		void SetReconstructionDirections(const std::string& dirs);
    
protected:

    vtkSphericalHarmonicSource(int tess=3);
    ~vtkSphericalHarmonicSource();
    
    int RequestData(vtkInformation*,vtkInformationVector**,vtkInformationVector*);
    int RequestInformation(vtkInformation*,vtkInformationVector**,vtkInformationVector*);

    double Radius;
    double Center[3];
    
    // Spherical function

    vtkPolyData* shell;

    // Spherical harmonic basis function

    matrix<double> BasisFunction;
    
    // Order of the spherical harmonic basis

    int Order;

    // Spherical harmonic representation of the spherical function

    double* SphericalHarmonics;
    int     NumberOfSphericalHarmonics;

    // Type of tesselation : cube, dodecahedron, icosahedron, octahedron, tetrahedron
    // determines the angular sampling scheme

    int TesselationType;

    // Type of basis : SHMatrix=0, SHMatrixMaxThesis=1, SHMatrixTournier=2, SHMatrixRshBasis=3,
    // determines the Spherical harmonics basis to be used

    int TesselationBasis;

    // Tesselation order, determines the angular sampling

    int Tesselation;

    // Deform On/Off
    // If On, the radius of the sphere is deformed according to the spherical value
    // Else, the data is visualized on a sphere of radius 1

    bool Deform;
    
    // Normalize On/Off
    // If On, spherical values are (0,1) normalized

    bool Normalize;
    
    // Flip{X,Y,Z} On/Off
    // If On, x direction is flipped in the reconstruction directions defining the basis function
    //
    // By default we need to flip Z to have glyphs correctly aligned

    bool FlipX;
    bool FlipY;
    bool FlipZ;
    
    // MaximeDescoteaux Thesis On/Off
    // If On, spherical values are (0,1) normalized

    bool MaxThesisFunc;

    matrix<double> PhiThetaShellDirections;
    vtkMatrix4x4*  RotationMatrix;

private:

    vtkSphericalHarmonicSource(const vtkSphericalHarmonicSource&);  // Not implemented.
    void operator=(const vtkSphericalHarmonicSource&);  // Not implemented.
};

#endif  //  ! VTKSPHERICALHARMONICSOURCE_HPP
