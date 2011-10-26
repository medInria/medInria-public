//  Authors:	   Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//  Description:  source of one spherical mesh.
//  Define the mesh and associated value for each direction
//  Deform the mesh after
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#include <vtkSphereTesselator.hpp>

#ifdef WIN32
#pragma warning(disable:4661)
#endif

namespace Visualization {

template<typename T>
vtkSphereTesselator<T>::vtkSphereTesselator() {
    m_initPolyhedra = icosahedron;
    m_cclockwise = false;
    m_verbose = false;
    m_vertices = vtkPoints::New();
    m_triangles  = vtkCellArray::New();

    m_initializeTesselation();
}

template<typename T>
vtkSphereTesselator<T>::vtkSphereTesselator(const p_solid& ip) {
    m_initPolyhedra = ip;
    m_cclockwise = false;
    m_verbose = false;
    m_vertices = vtkPoints::New();
    m_triangles  = vtkCellArray::New();

    m_initializeTesselation();
}

//  template<typename T>
//  vtkSphereTesselator<T>::vtkSphereTesselator(const vtkSphereTesselator<T>& st) {
//      m_initPolyhedra = st.m_initPolyhedra;
//      m_cclockwise    = st.m_cclockwise;
//      m_verbose = st.m_verbose;
//      m_tesselation   = st.m_tesselation;
//      m_pDataCleaner = vtkCleanPolyData::New();
//      m_pDataCleaner->ShallowCopy(st.m_pDataCleaner);
//  }

template<typename T>
vtkSphereTesselator<T>::~vtkSphereTesselator() {
    if( m_vertices )
        m_vertices->Delete();
    if( m_triangles )
        m_triangles->Delete();
}

template<typename T>
void vtkSphereTesselator<T>::enableCClockWise(const bool& flag) {
    m_cclockwise = flag;
}

template<typename T>
void vtkSphereTesselator<T>::enableVerbosity(const bool& flag) {
    m_verbose = flag;
}

template<typename T>
void vtkSphereTesselator<T>:: m_initializeTesselation() {
    vtkIdType* vi = new vtkIdType[3];

    switch( m_initPolyhedra ) {

    case tetrahedron: {
        T sqrt_3 = 0.5773502692;
        m_vertices->SetNumberOfPoints(4);
        //m_vertices->Allocate(4);

        /* 4 vertices */
        m_vertices->SetPoint(0,  sqrt_3,  sqrt_3,  sqrt_3);
        m_vertices->SetPoint(1, -sqrt_3, -sqrt_3,  sqrt_3);
        m_vertices->SetPoint(2, -sqrt_3,  sqrt_3, -sqrt_3);
        m_vertices->SetPoint(3,  sqrt_3, -sqrt_3, -sqrt_3);

        m_triangles->Allocate(4);

        /* Create Tetrahedron */
        vi[0] = 0; vi[1] = 1; vi[2] = 2;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 3; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 1; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 2; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);

        break;
    }

    case icosahedron: {

        T tau = 0.850650808352039932;
        T one = 0.525731112119133606;

        m_vertices->SetNumberOfPoints(12);

        /* 12 vertices */
        m_vertices->SetPoint(0,  tau,  one,  0.0);
        m_vertices->SetPoint(1, -tau,  one,  0.0);
        m_vertices->SetPoint(2, -tau, -one,  0.0);
        m_vertices->SetPoint(3,  tau, -one,  0.0);
        m_vertices->SetPoint(4,  one,  0.0,  tau);
        m_vertices->SetPoint(5,  one,  0.0, -tau);
        m_vertices->SetPoint(6, -one,  0.0, -tau);
        m_vertices->SetPoint(7, -one,  0.0,  tau);
        m_vertices->SetPoint(8,  0.0,  tau,  one);
        m_vertices->SetPoint(9,  0.0, -tau,  one);
        m_vertices->SetPoint(10, 0.0, -tau, -one);
        m_vertices->SetPoint(11, 0.0,  tau, -one);

        /* Create Icosahedron */
        m_triangles->Allocate(20);

        vi[0] = 4; vi[1] = 8; vi[2] = 7;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 4; vi[1] = 7; vi[2] = 9;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 6; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 10; vi[2] = 6;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 4; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 3; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 7; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 1; vi[2] = 6;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 0; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 11; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 9; vi[1] = 10; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 9; vi[1] = 2; vi[2] = 10;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 4; vi[2] = 0;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 11; vi[1] = 0; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 4; vi[1] = 9; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 3; vi[2] = 10;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 7; vi[1] = 8; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 6; vi[1] = 1; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 7; vi[1] = 2; vi[2] = 9;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 6; vi[1] = 10; vi[2] = 2;
        m_triangles->InsertNextCell((vtkIdType)3,vi);

        break;

    }

    case octahedron: {
        /* 6 vertices */
        m_vertices->SetNumberOfPoints(6);
        m_vertices->SetPoint(0, 1.0, 0.0,  0.0);
        m_vertices->SetPoint(1,-1.0, 0.0,  0.0);
        m_vertices->SetPoint(2, 0.0, 1.0,  0.0);
        m_vertices->SetPoint(3, 0.0,-1.0,  0.0);
        m_vertices->SetPoint(4, 0.0, 0.0,  1.0);
        m_vertices->SetPoint(5, 0.0, 0.0, -1.0);

        m_triangles->Allocate(8);

        vi[0] = 0; vi[1] = 4; vi[2] = 2;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 4; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 1; vi[1] = 4; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 3; vi[1] = 4; vi[2] = 0;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 2; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 1; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 1; vi[1] = 3; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 3; vi[1] = 0; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        break;
    }

    default: {

        T tau = 0.850650808352039932;
        T one = 0.525731112119133606;

        m_vertices->SetNumberOfPoints(12);
        //        m_vertices->Allocate(12);

        /* 12 vertices */
        m_vertices->SetPoint(0, tau,  one,  0.0);
        m_vertices->SetPoint(1,-tau,  one,  0.0);
        m_vertices->SetPoint(2,-tau, -one,  0.0);
        m_vertices->SetPoint(3, tau, -one,  0.0);
        m_vertices->SetPoint(4, one,  0.0,  tau);
        m_vertices->SetPoint(5, one,  0.0, -tau);
        m_vertices->SetPoint(6,-one,  0.0, -tau);
        m_vertices->SetPoint(7,-one,  0.0,  tau);
        m_vertices->SetPoint(8, 0.0,  tau,  one);
        m_vertices->SetPoint(9, 0.0, -tau,  one);
        m_vertices->SetPoint(10, 0.0, -tau, -one);
        m_vertices->SetPoint(11, 0.0,  tau, -one);

        m_triangles->Allocate(20);

        /* Create Icosahedron */
        vi[0] = 4; vi[1] = 8; vi[2] = 7;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 4; vi[1] = 7; vi[2] = 9;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 6; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 10; vi[2] = 6;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 4; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 0; vi[1] = 3; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 7; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 2; vi[1] = 1; vi[2] = 6;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 0; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 11; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 9; vi[1] = 10; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 9; vi[1] = 2; vi[2] = 10;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 8; vi[1] = 4; vi[2] = 0;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 11; vi[1] = 0; vi[2] = 5;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 4; vi[1] = 9; vi[2] = 3;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 5; vi[1] = 3; vi[2] = 10;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 7; vi[1] = 8; vi[2] = 1;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 6; vi[1] = 1; vi[2] = 11;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 7; vi[1] = 2; vi[2] = 9;
        m_triangles->InsertNextCell((vtkIdType)3,vi);
        vi[0] = 6; vi[1] = 10; vi[2] = 2;
        m_triangles->InsertNextCell((vtkIdType)3,vi);

        break;
    }

    }
    delete [] vi;
}

void GetMiddleNormalizedPoint( double A[3],double B[3], double x[3])
{
    double norm;
    x[0]=A[0]+B[0]; x[1]=A[1]+B[1]; x[2]=A[2]+B[2];

    norm= 1/(sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
    x[0]=norm*x[0];    x[1]=norm*x[1];    x[2]=norm*x[2];
}


void TesselateTriangles( vtkPoints* oldVertices, vtkCellArray* oldTriangles, vtkPoints* vertices,vtkCellArray* triangles)
{
    double pointA[3], pointB[3], pointC[3], tempPoint[3];
    double AB[3][3];

    vtkIdType* vi = new vtkIdType[3];

    int n1Points = oldVertices->GetNumberOfPoints();
    int n1Faces = oldTriangles->GetNumberOfCells();
    int n1Edges = n1Points+n1Faces-2;

    int n2Points=n1Edges+n1Points;

    vertices->SetNumberOfPoints(n2Points);

    triangles->Allocate(4*n1Faces);

    for (int i = 0; i < n1Points; i++)
    {
        vertices->SetPoint(i,oldVertices->GetPoint(i));
    }
    vertices->GetPoint(0, pointA);
    vertices->GetPoint(1, pointB);
    vertices->GetPoint(2, pointC);

    oldTriangles->InitTraversal();

    vtkIdType numCellPts = 0;
    vtkIdType triPointsIds[3] = {-1,-1,-1};
    vtkIdType * pts;

    int current2P = n1Points-1;

    int face =0;
    float a,b,c;
    oldVertices->GetPoint(0, pointA);
    float minDist=( pointA[1]*pointA[1]+ pointA[0]*pointA[0]+ pointA[2]*pointA[2])/1000;

    /* Subdivide each triangle in the old approximation and normalize
     * the new points thus generated to lie on the surface of the unit
     * sphere.
     * Each input triangle with vertices labelled [0,1,2] as shown
     * below will be turned into four new triangles:
     *              * Make new points
     *              a = (0+2)/2
     *              b = (0+1)/2
     *              c = (1+2)/2
     *      1
     *      /\         Normalize a, b, c
     *     /  \
     *   b/____\c    Construct new triangles
     *   /\    /\       [0,b,a]
     *  /  \  /  \      [b,1,c]
     * /____\/____\     [a,b,c]
     * 0     a     2    [a,c,2]
    */

    while(oldTriangles->GetNextCell(numCellPts, pts))
    {
        triPointsIds[0] = -1; triPointsIds[1] = -1; triPointsIds[2] = -1;
        //if (numCellPts == 3)
        //{
        oldVertices->GetPoint(pts[0], pointA);
        oldVertices->GetPoint(pts[1], pointB);
        oldVertices->GetPoint(pts[2], pointC);

        GetMiddleNormalizedPoint( pointA, pointB, AB[0]);
        GetMiddleNormalizedPoint( pointA, pointC, AB[1]);
        GetMiddleNormalizedPoint( pointB, pointC, AB[2]);

        for (int n = current2P; n >= n1Points; n--)
        {
            if(triPointsIds[0] != -1 && triPointsIds[1] != -1 && triPointsIds[2] != -1)
            {
                break;
            }
            else
            {
                vertices->GetPoint(n,tempPoint);

                a=std::abs(tempPoint[0]-AB[0][0]); b = std::abs(tempPoint[1]-AB[0][1]); c = std::abs(tempPoint[2]-AB[0][2]);
                //if(tempPoint[0]==AB[0][0] && tempPoint[1]==AB[0][1] && tempPoint[2]==AB[0][2])
                if(a<=minDist && b<=minDist && c<=minDist)
                {
                    triPointsIds[0]= n;
                    continue;
                }

                a=std::abs(tempPoint[0]-AB[1][0]);b= std::abs(tempPoint[1]-AB[1][1]); c=std::abs(tempPoint[2]-AB[1][2]);
                //if(tempPoint[0]==AB[1][0] && tempPoint[1]==AB[1][1] && tempPoint[2]==AB[1][2])
                if(a<=minDist && b<=minDist && c<=minDist)
                {
                    triPointsIds[1]= n;
                    continue;
                }

                a=std::abs(tempPoint[0]-AB[2][0]); b=std::abs(tempPoint[1]-AB[2][1]); c=std::abs(tempPoint[2]-AB[2][2]);
                //if(tempPoint[0]==AB[2][0] && tempPoint[1]==AB[2][1] && tempPoint[2]==AB[2][2])
                if(a<=minDist && b<=minDist && c<=minDist)
                {
                    triPointsIds[2]= n;
                    continue;
                }
            }

        }

        for (int j = 0; j < 3; j++)
        {
            if (triPointsIds[j] == -1)
            {
                current2P++;
                // Do not know what was this for it was in late algorithm but it does not make a difference
                // if (AB[j][0]== 0 && AB[j][1] == 0)
                // {
                //     vertices->SetPoint(current2P,AB[j][0],AB[j][1] - M_EPS, AB[j][2]);
                // }
                // else{
                vertices->SetPoint(current2P, AB[j]);
                // }
                triPointsIds[j] = current2P;
            }

        }

        triangles->InsertNextCell((vtkIdType)3,triPointsIds);

        vi[0] = pts[0]; vi[1] = triPointsIds[0]; vi[2] = triPointsIds[1];
        triangles->InsertNextCell((vtkIdType)3,vi);

        vi[0] = pts[1]; vi[1] = triPointsIds[2]; vi[2] = triPointsIds[0];
        triangles->InsertNextCell((vtkIdType)3,vi);

        vi[0] = pts[2]; vi[1] = triPointsIds[1]; vi[2] = triPointsIds[2];
        triangles->InsertNextCell((vtkIdType)3,vi);
        face++;
    }

    delete [] vi;
}


template<typename T>
void vtkSphereTesselator<T>::tesselate(const int& R)
{
    vtkCellArray *triangles = NULL;
    vtkPoints *vertices = NULL;

    for (int level = 0; level < R; level++)
    {

        vertices = vtkPoints::New();
        triangles  = vtkCellArray::New();

        TesselateTriangles(m_vertices , m_triangles, vertices, triangles);

        m_vertices->SetData(vertices->GetData());

        m_triangles->SetCells(triangles->GetNumberOfCells(),triangles->GetData());

        vertices->Delete();
        triangles->Delete();

    }

}

template<typename T>
void vtkSphereTesselator<T>::getvtkTesselation(vtkPolyData* t) {
    vtkPolyData* output = vtkPolyData::New();
    output->SetPoints(m_vertices);
    output->SetPolys(m_triangles);
    t->ShallowCopy(output);
    output->Delete();
}

//template<typename T>
//void vtkSphereTesselator<T>::m_flipOrientation(tesselation<T>* t) {
//    vertex<T> tmp;
//    for(int i=0; i<t->ntri; i++) {
//        tmp = t->tr[i].v[0];
//        t->tr[i].v[0] = t->tr[i].v[2];
//        t->tr[i].v[2] = tmp;
//    }
//}

//template<typename T>
//void vtkSphereTesselator<T>::tesselate(const int& l) {

//}

//template<typename T>
//tesselation<T> vtkSphereTesselator<T>::getTesselation() {
//    return m_tesselation;
//}

//template<typename T>
//void vtkSphereTesselator<T>::getvtkTesselation(const bool& clean, vtkPolyData* t) {

//}

template class vtkSphereTesselator<float>;
template class vtkSphereTesselator<double>;
}



