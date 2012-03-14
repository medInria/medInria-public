//  Authors:	   Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//  Description:  vtk source of one spherical mesh.
//  Define the mesh and associated value for each direction
//  Deform the mesh after
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#ifndef vtkTesselatedSphereSource_H
#define vtkTesselatedSphereSource_H
#include "vtkINRIA3DConfigure.h"

#include <vtkPolyDataAlgorithm.h>

/** http://davidf.faricy.net/polyhedra/Platonic_Solids.html */

/** \Clas Object
* \brief Perform tesselation of the unit sphere from a given platonic solid
*/

class VTK_VISUMANAGEMENT_EXPORT vtkTesselatedSphereSource : public vtkPolyDataAlgorithm {
public:
    static vtkTesselatedSphereSource *New();
    vtkTypeMacro(vtkTesselatedSphereSource,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Set the Resolution number of times that the selected polyhedron is going to be tesselated.
    vtkSetMacro(Resolution,unsigned int);
    vtkGetMacro(Resolution,unsigned int);

    // Description:
    // Set the type of polyhedron is going to be tesselated.
    vtkSetMacro(PolyhedraType,unsigned int);
    vtkGetMacro(PolyhedraType,unsigned int);


protected:
    vtkTesselatedSphereSource();
    vtkTesselatedSphereSource(const int ip);
    /*virtual*/ ~vtkTesselatedSphereSource(){}


    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    unsigned int Resolution;
    unsigned int PolyhedraType;
    enum {
        cube=0,
        dodecahedron = 1,
        icosahedron = 2,
        octahedron = 3,
        tetrahedron = 4
    };


private:

    vtkTesselatedSphereSource& operator= (const vtkTesselatedSphereSource& st); // Not implemented
    vtkTesselatedSphereSource(const vtkTesselatedSphereSource&);  // Not implemented.

    /** Initialize by using the m_PolyhedraType the polyhedron that it is gong to be tesselated  */
    void initializeTesselation(vtkPoints* vertices, vtkCellArray* triangles);
};

#endif

