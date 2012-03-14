//  Authors:	   Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//  Description:  vtk source of one spherical mesh.
//  Define the mesh and associated value for each direction
//  Deform the mesh after
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#ifndef vtkSphereTesselator_H
#define vtkSphereTesselator_H
#include "vtkINRIA3DConfigure.h"



#include <vtkPolyDataAlgorithm.h>


/** http://davidf.faricy.net/polyhedra/Platonic_Solids.html */


/** \Clas Object
* \brief Perform tesselation of the unit sphere from a given platonic solid
*/

class VTK_VISUMANAGEMENT_EXPORT vtkSphereTesselator : public vtkPolyDataAlgorithm {
public:
    static vtkSphereTesselator *New();
    vtkTypeMacro(vtkSphereTesselator,vtkPolyDataAlgorithm);
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
    vtkSphereTesselator();
    vtkSphereTesselator(const int ip);
    /*virtual*/ ~vtkSphereTesselator(){}


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

    vtkSphereTesselator& operator= (const vtkSphereTesselator& st); // Not implemented
    vtkSphereTesselator(const vtkSphereTesselator&);  // Not implemented.

    /** Initialize by using the m_PolyhedraType the polyhedron that it is gong to be tesselated  */
    void initializeTesselation(vtkPoints* vertices, vtkCellArray* triangles);
};

#endif

