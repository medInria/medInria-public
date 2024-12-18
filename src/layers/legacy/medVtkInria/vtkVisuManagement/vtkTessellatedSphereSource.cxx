/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkTessellatedSphereSource.h>

#include <cmath>
#include <map>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>

#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <sstream>

#include <vtkPointLocator.h>
#ifdef WIN32
#pragma warning(disable:4661)
#endif

vtkStandardNewMacro(vtkTessellatedSphereSource);

vtkTessellatedSphereSource::vtkTessellatedSphereSource(): Resolution(2),PolyhedraType(icosahedron) {
    SetNumberOfInputPorts(0);
}

vtkTessellatedSphereSource::vtkTessellatedSphereSource(const int ip): Resolution(2),PolyhedraType(ip) {
    SetNumberOfInputPorts(0);
}

void
vtkTessellatedSphereSource::initializeTesselation(vtkPoints* vertices,vtkCellArray* triangles) {

    typedef double Point[3];

    unsigned        nPoints,nCells;
    const Point*    pts;
    vtkIdTypeArray* faces = vtkIdTypeArray::New();

    switch (PolyhedraType) {

        case tetrahedron: {

            static const double sqrt_3 = 0.5773502692;
            static const Point tetrahedron_pts[4] = {
                {  sqrt_3,  sqrt_3,  sqrt_3 }, { -sqrt_3, -sqrt_3,  sqrt_3 },
                { -sqrt_3,  sqrt_3, -sqrt_3 }, {  sqrt_3, -sqrt_3, -sqrt_3 }
            };

            static vtkIdType tetrahedron_faces[4][4] = {
                { 3, 0 , 1, 2 }, { 3, 0 , 3, 1 }, { 3, 2 , 1, 3 }, { 3, 0 , 2, 3 }
            };

            nCells = nPoints = 4;
            pts = &tetrahedron_pts[0];
            faces->SetArray(&tetrahedron_faces[0][0],nCells*4,1);

            break;
        }

        case octahedron: {

            static const Point octahedron_pts[6] = {
                { 1.0, 0.0,  0.0 }, {-1.0, 0.0,  0.0 }, { 0.0, 1.0,  0.0 },
                { 0.0,-1.0,  0.0 }, { 0.0, 0.0,  1.0 }, { 0.0, 0.0, -1.0 }
            };

            static vtkIdType octahedron_faces[8][4] = {
                { 3, 0, 4, 2 }, { 3, 2, 4, 1 }, { 3, 1, 4, 3 }, { 3, 3, 4, 0 },
                { 3, 0, 2, 5 }, { 3, 2, 1, 5 }, { 3, 1, 3, 5 }, { 3, 3, 0, 5 }
            };

            nPoints = 6;
            nCells  = 8;

            pts = &octahedron_pts[0];
            faces->SetArray(&octahedron_faces[0][0],nCells*4,1);

            break;
        }

        default:
        case icosahedron: {

            static const double tau = 0.850650808352039932;
            static const double one = 0.525731112119133606;

            static const Point icosahedron_pts[12] = {
                {  tau,  one,  0.0 }, { -tau,  one,  0.0 }, { -tau, -one,  0.0 }, {  tau, -one,  0.0 },
                {  one,  0.0,  tau }, {  one,  0.0, -tau }, { -one,  0.0, -tau }, { -one,  0.0,  tau },
                {  0.0,  tau,  one }, {  0.0, -tau,  one }, {  0.0, -tau, -one }, {  0.0,  tau, -one }
            };

            static vtkIdType icosahedron_faces[20][4] = {
                { 3, 4,  8,  7 }, { 3, 4,  7,  9 }, { 3, 5,  6, 11 }, { 3, 5, 10,  6 }, { 3, 0,  4,  3 },
                { 3, 0,  3,  5 }, { 3, 2,  7,  1 }, { 3, 2,  1,  6 }, { 3, 8,  0, 11 }, { 3, 8, 11,  1 },
                { 3, 9, 10,  3 }, { 3, 9,  2, 10 }, { 3, 8,  4,  0 }, { 3, 11, 0,  5 }, { 3, 4,  9,  3 },
                { 3, 5,  3, 10 }, { 3, 7,  8,  1 }, { 3, 6,  1, 11 }, { 3, 7,  2,  9 }, { 3, 6, 10,  2 }
            };

            nPoints = 12;
            nCells  = 20;

            pts = &icosahedron_pts[0];
            faces->SetArray(&icosahedron_faces[0][0],nCells*4,1);

            break;
        }
    }

    vertices->SetNumberOfPoints(nPoints);
    for (vtkIdType i=0;i<nPoints;++i)
        vertices->SetPoint(i,pts[i]);

    triangles->Allocate(nCells);
    triangles->SetCells(nCells,faces);
}

void GetMiddleNormalizedPoint(const double A[3],const double B[3],double x[3])
{
    x[0] = A[0]+B[0];
    x[1] = A[1]+B[1];
    x[2] = A[2]+B[2];

    const double inv_norm= 1.0/(sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));

    x[0] = inv_norm*x[0];
    x[1] = inv_norm*x[1];
    x[2] = inv_norm*x[2];
}

void TesselateTriangles(vtkPoints* oldVertices,vtkCellArray* oldTriangles,
                        vtkPoints* vertices,vtkCellArray* triangles)
{
    const int oldPoints = oldVertices->GetNumberOfPoints();
    const int oldFaces  = oldTriangles->GetNumberOfCells();
    const int oldEdges  = oldPoints+oldFaces-2;

    vertices->SetNumberOfPoints(oldEdges+oldPoints);
    triangles->Allocate(4*oldFaces);

    for (int i=0;i<oldPoints;++i)
        vertices->SetPoint(i,oldVertices->GetPoint(i));

    // Subdivide each triangle in the old approximation and normalize
    // the new points thus generated to lie on the surface of the unit
    // sphere.
    // Each input triangle with vertices labelled [0,1,2] as shown
    // below will be turned into four new triangles:
    //              * Make new points
    //              a = (1+2)/2
    //              b = (0+2)/2
    //              c = (0+1)/2
    //      1
    //      /\         Normalize a, b, c
    //     /  \        Construct new triangles: 
    //   c/____\a
    //   /\    /\       [a,b,c]
    //  /  \  /  \      [1,a,c]
    // /____\/____\     [b,0,c]
    // 0     b     2    [b,a,2]
    //

    //  Data structure to insert middle points only once.

    typedef std::pair<vtkIdType,vtkIdType> BiPoint;
    typedef std::map<BiPoint,vtkIdType>    MidPointIds;

    MidPointIds ids;

    vtkIdType numCellPts = 0;
    vtkIdType const *pts = nullptr;
    oldTriangles->InitTraversal();
    for (int current2P=oldPoints;oldTriangles->GetNextCell(numCellPts,pts);) {

        //  Get point coordinates of the triangle vertices.

        double points[3][3];
        for (unsigned i=0;i<3;++i)
            oldVertices->GetPoint(pts[i],points[i]);

        //  Compute the mid points for the current triangle.

        vtkIdType triPointsIds[3] = {-1,-1,-1};
        for (unsigned i=0,l=2;i<3;++i)
            for (unsigned j=i+1;j<3;++j,--l) {
                const BiPoint key = (pts[i]<pts[j]) ? std::make_pair(pts[i],pts[j]) : std::make_pair(pts[j],pts[i]);
                const MidPointIds::const_iterator iter = ids.find(key);

                if (iter!=ids.end()) {

                    //  The point has already been built, get its id and continue.

                    triPointsIds[l] = iter->second;
                    continue;
                }
                    
                //  Create the new mid point, assign an id and register it.

                double midPoint[3];
                GetMiddleNormalizedPoint(points[i],points[j],midPoint);
                triPointsIds[l] = current2P;
                ids.insert(std::make_pair(key,current2P));
                vertices->SetPoint(current2P++,midPoint);
            }

        //  Add the triangles
        vtkIdType tmpPts[3];
        std::copy(pts, pts + 3, tmpPts);

        const vtkIdType c3 = 3;
        triangles->InsertNextCell(c3,triPointsIds);
        std::swap(triPointsIds[0],triPointsIds[1]);
        std::swap(tmpPts[0], tmpPts[1]);

        for (int j=0;j<3;++j)
        {
            std::swap(triPointsIds[j], tmpPts[j]);
            triangles->InsertNextCell(c3, triPointsIds);
            std::swap(triPointsIds[j], tmpPts[j]);
        }
    }
}

int
vtkTessellatedSphereSource::RequestData(vtkInformation *vtkNotUsed(request),vtkInformationVector **vtkNotUsed(inputVector),vtkInformationVector *outputVector) {

    // Get the info object

    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // Get the ouptut

    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPoints* m_vertices = vtkPoints::New();
    vtkCellArray* m_triangles = vtkCellArray::New();
    initializeTesselation(m_vertices, m_triangles);

    vtkCellArray *tempTriangles = nullptr;
    vtkPoints *tempVertices = nullptr;

    for (unsigned level=0;level<Resolution;++level) {

        tempVertices = vtkPoints::New();
        tempTriangles  = vtkCellArray::New();

        TesselateTriangles(m_vertices , m_triangles, tempVertices, tempTriangles);

        m_vertices->SetData(tempVertices->GetData());
        m_triangles->SetCells(tempTriangles->GetNumberOfCells(),tempTriangles->GetData());

        tempVertices->Delete();
        tempTriangles->Delete();
    }

    output->SetPoints(m_vertices);
    m_vertices->Delete();

    m_triangles->Squeeze();
    output->SetPolys(m_triangles);
    m_triangles->Delete();
    return 1;
}

//----------------------------------------------------------------------------
void vtkTessellatedSphereSource::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Polyhedratype: " << PolyhedraType << "\n";

}
