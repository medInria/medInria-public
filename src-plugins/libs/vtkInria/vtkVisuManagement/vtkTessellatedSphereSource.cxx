/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/









#include <vtkTessellatedSphereSource.h>

#include <cmath>
#include "vtkStreamingDemandDrivenPipeline.h"
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>

#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <vtkPointLocator.h>
#ifdef WIN32
#pragma warning(disable:4661)
#endif

vtkStandardNewMacro(vtkTessellatedSphereSource);

vtkTessellatedSphereSource::vtkTessellatedSphereSource() {
  this->PolyhedraType = icosahedron;
  this->Resolution=4;
  this->SetNumberOfInputPorts(0);
}


vtkTessellatedSphereSource::vtkTessellatedSphereSource(const int ip) {
  this->PolyhedraType = ip;
  this->Resolution=4;
  this->SetNumberOfInputPorts(0);
}


void vtkTessellatedSphereSource::initializeTesselation(vtkPoints* vertices, vtkCellArray* triangles) {
  vtkIdType* vi = new vtkIdType[3];

  switch( this->PolyhedraType ) {

  case tetrahedron: {
    double sqrt_3 = 0.5773502692;
    vertices->SetNumberOfPoints(4);
    //vertices->Allocate(4);

    /* 4 vertices */
    vertices->SetPoint(0,  sqrt_3,  sqrt_3,  sqrt_3);
    vertices->SetPoint(1, -sqrt_3, -sqrt_3,  sqrt_3);
    vertices->SetPoint(2, -sqrt_3,  sqrt_3, -sqrt_3);
    vertices->SetPoint(3,  sqrt_3, -sqrt_3, -sqrt_3);

    triangles->Allocate(4);

    /* Create Tetrahedron */
    vi[0] = 0; vi[1] = 1; vi[2] = 2;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 3; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 1; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 2; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);

    break;
  }

  case icosahedron: {

    double tau = 0.850650808352039932;
    double one = 0.525731112119133606;

    vertices->SetNumberOfPoints(12);

    /* 12 vertices */
    vertices->SetPoint(0,  tau,  one,  0.0);
    vertices->SetPoint(1, -tau,  one,  0.0);
    vertices->SetPoint(2, -tau, -one,  0.0);
    vertices->SetPoint(3,  tau, -one,  0.0);
    vertices->SetPoint(4,  one,  0.0,  tau);
    vertices->SetPoint(5,  one,  0.0, -tau);
    vertices->SetPoint(6, -one,  0.0, -tau);
    vertices->SetPoint(7, -one,  0.0,  tau);
    vertices->SetPoint(8,  0.0,  tau,  one);
    vertices->SetPoint(9,  0.0, -tau,  one);
    vertices->SetPoint(10, 0.0, -tau, -one);
    vertices->SetPoint(11, 0.0,  tau, -one);

    /* Create Icosahedron */
    triangles->Allocate(20);

    vi[0] = 4; vi[1] = 8; vi[2] = 7;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 4; vi[1] = 7; vi[2] = 9;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 6; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 10; vi[2] = 6;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 4; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 3; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 7; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 1; vi[2] = 6;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 0; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 11; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 9; vi[1] = 10; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 9; vi[1] = 2; vi[2] = 10;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 4; vi[2] = 0;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 11; vi[1] = 0; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 4; vi[1] = 9; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 3; vi[2] = 10;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 7; vi[1] = 8; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 6; vi[1] = 1; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 7; vi[1] = 2; vi[2] = 9;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 6; vi[1] = 10; vi[2] = 2;
    triangles->InsertNextCell((vtkIdType)3,vi);

    break;

  }

  case octahedron: {
    /* 6 vertices */
    vertices->SetNumberOfPoints(6);
    vertices->SetPoint(0, 1.0, 0.0,  0.0);
    vertices->SetPoint(1,-1.0, 0.0,  0.0);
    vertices->SetPoint(2, 0.0, 1.0,  0.0);
    vertices->SetPoint(3, 0.0,-1.0,  0.0);
    vertices->SetPoint(4, 0.0, 0.0,  1.0);
    vertices->SetPoint(5, 0.0, 0.0, -1.0);

    triangles->Allocate(8);

    vi[0] = 0; vi[1] = 4; vi[2] = 2;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 4; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 1; vi[1] = 4; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 3; vi[1] = 4; vi[2] = 0;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 2; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 1; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 1; vi[1] = 3; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 3; vi[1] = 0; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    break;
  }

  default: {

    double tau = 0.850650808352039932;
    double one = 0.525731112119133606;

    vertices->SetNumberOfPoints(12);
    //        vertices->Allocate(12);

    /* 12 vertices */
    vertices->SetPoint(0, tau,  one,  0.0);
    vertices->SetPoint(1,-tau,  one,  0.0);
    vertices->SetPoint(2,-tau, -one,  0.0);
    vertices->SetPoint(3, tau, -one,  0.0);
    vertices->SetPoint(4, one,  0.0,  tau);
    vertices->SetPoint(5, one,  0.0, -tau);
    vertices->SetPoint(6,-one,  0.0, -tau);
    vertices->SetPoint(7,-one,  0.0,  tau);
    vertices->SetPoint(8, 0.0,  tau,  one);
    vertices->SetPoint(9, 0.0, -tau,  one);
    vertices->SetPoint(10, 0.0, -tau, -one);
    vertices->SetPoint(11, 0.0,  tau, -one);

    triangles->Allocate(20);

    /* Create Icosahedron */
    vi[0] = 4; vi[1] = 8; vi[2] = 7;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 4; vi[1] = 7; vi[2] = 9;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 6; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 10; vi[2] = 6;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 4; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 0; vi[1] = 3; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 7; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 2; vi[1] = 1; vi[2] = 6;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 0; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 11; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 9; vi[1] = 10; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 9; vi[1] = 2; vi[2] = 10;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 8; vi[1] = 4; vi[2] = 0;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 11; vi[1] = 0; vi[2] = 5;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 4; vi[1] = 9; vi[2] = 3;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 5; vi[1] = 3; vi[2] = 10;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 7; vi[1] = 8; vi[2] = 1;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 6; vi[1] = 1; vi[2] = 11;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 7; vi[1] = 2; vi[2] = 9;
    triangles->InsertNextCell((vtkIdType)3,vi);
    vi[0] = 6; vi[1] = 10; vi[2] = 2;
    triangles->InsertNextCell((vtkIdType)3,vi);

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


void TesselateTriangles( vtkPoints* oldVertices, vtkCellArray* oldTriangles,
                         vtkPoints* vertices,vtkCellArray* triangles)
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
        vertices->SetPoint(current2P, AB[j]);
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

int vtkTessellatedSphereSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptutb
  vtkPolyData *output = vtkPolyData::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* m_vertices = vtkPoints::New();
  vtkCellArray* m_triangles = vtkCellArray::New();
  this->initializeTesselation(m_vertices, m_triangles);

  vtkCellArray *tempTriangles = NULL;
  vtkPoints *tempVertices = NULL;


  for (unsigned int level = 0; level < this->Resolution; level++)
  {

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
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Polyhedratype: " << this->PolyhedraType << "\n";

}




