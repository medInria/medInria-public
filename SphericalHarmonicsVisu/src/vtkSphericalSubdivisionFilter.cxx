/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSphericalSubdivisionFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSphericalSubdivisionFilter.h"

#include "vtkCellArray.h"
#include "vtkEdgeTable.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "defs.hpp"

vtkStandardNewMacro(vtkSphericalSubdivisionFilter);

void vtkSphericalSubdivisionFilter::GenerateSubdivisionPoints (vtkPolyData *inputDS, vtkIntArray *edgeData, vtkPoints *outputPts, vtkPointData *outputPD)
{
  vtkIdType *pts = 0;
  int edgeId;
  vtkIdType npts, cellId, newId;
  vtkIdType p1, p2;
  vtkCellArray *inputPolys=inputDS->GetPolys();
  vtkEdgeTable *edgeTable;
  vtkIdList *cellIds = vtkIdList::New();
  vtkIdList *pointIds = vtkIdList::New();
  vtkPoints *inputPts=inputDS->GetPoints();
  vtkPointData *inputPD=inputDS->GetPointData();
  static double weights[2] = {.5, .5};

  // Create an edge table to keep track of which edges we've processed
  edgeTable = vtkEdgeTable::New();
  edgeTable->InitEdgeInsertion(inputDS->GetNumberOfPoints());

  pointIds->SetNumberOfIds(2);

  double total = inputPolys->GetNumberOfCells();
  double curr = 0;

  // Generate new points for subdivisions surface
  for (cellId=0, inputPolys->InitTraversal();
       inputPolys->GetNextCell(npts, pts); cellId++)
    {
    if ( inputDS->GetCellType(cellId) != VTK_TRIANGLE )
      {
      continue;
      }

    p1 = pts[2];
    p2 = pts[0];

    for (edgeId=0; edgeId < 3; edgeId++)
      {
      outputPD->CopyData (inputPD, p1, p1);
      outputPD->CopyData (inputPD, p2, p2);

      // Do we need to  create a point on this edge?
      if (edgeTable->IsEdge (p1, p2) == -1)
        {
        edgeTable->InsertEdge (p1, p2);
        // Compute Position andnew PointData using the same subdivision scheme
        pointIds->SetId(0,p1);
        pointIds->SetId(1,p2);
        newId = 
          this->InterpolatePositionNorm (inputPts, outputPts, pointIds, weights);
        outputPD->InterpolatePoint (inputPD, newId, pointIds, weights);
        }
      else // we have already created a point on this edge. find it
        {
        newId = this->FindEdge (inputDS, cellId, p1, p2, edgeData, cellIds);
        }
      edgeData->InsertComponent(cellId,edgeId,newId);
      p1 = p2;
      if (edgeId < 2)
        {
        p2 = pts[edgeId + 1];
        }
      } // each edge
    this->UpdateProgress(curr / total);
    curr += 1;
    } // each cell

  edgeTable->Delete();
  cellIds->Delete();
  pointIds->Delete();
}


vtkIdType vtkSphericalSubdivisionFilter::InterpolatePositionNorm (
        vtkPoints *inputPts, vtkPoints *outputPts,
        vtkIdList *stencil, double *weights)
{
  double xx[3], x[3], norm;
  int i, j;
  for (j = 0; j < 3; j++)
    {
    x[j] = 0.0;
    }

  for (i = 0; i < stencil->GetNumberOfIds(); i++)
    {
    inputPts->GetPoint(stencil->GetId(i), xx);
    for (j = 0; j < 3; j++)
      {
      x[j] += xx[j] * weights[i];
      }
    norm= 1/(sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
    x[0]=norm*x[0];    x[1]=norm*x[1];    x[2]=norm*x[2];
    if (x[0]==0 && x[1]==0)
        x[1]=x[1]- M_EPS;
    }
  return outputPts->InsertNextPoint (x);
}
