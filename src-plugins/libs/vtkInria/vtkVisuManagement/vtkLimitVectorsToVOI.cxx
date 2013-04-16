/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLimitVectorsToVOI.h"

#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkCell.h>


vtkCxxRevisionMacro(vtkLimitVectorsToVOI, "$Revision: 880 $");
vtkStandardNewMacro(vtkLimitVectorsToVOI);

vtkLimitVectorsToVOI::vtkLimitVectorsToVOI()
{
  m_XMin = 0;
  m_XMax = -1;
  m_YMin = 0;
  m_YMax = -1;
  m_ZMin = 0;
  m_ZMax = -1;
}


void vtkLimitVectorsToVOI::SetVOI (const int &xmin, const int &xmax, const int &ymin,
                                  const int &ymax, const int &zmin, const int &zmax)
{
  m_XMin = xmin;
  m_XMax = xmax;
  m_YMin = ymin;
  m_YMax = ymax;
  m_ZMin = zmin;
  m_ZMax = zmax;
  this->Modified();
}


int vtkLimitVectorsToVOI::RequestData (vtkInformation *vtkNotUsed(request),
                                       vtkInformationVector **inputVector,
                                       vtkInformationVector *outputVector)
{

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  
  // get the input and ouptut
  vtkUnstructuredGrid *input =  vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  output->Initialize();
  //output->Allocate();

  vtkPoints* points = input->GetPoints();
  //newpoints->DeepCopy(points);
  // double zero[3] = {0,0,0 };

//   bool is_in_points = true;
  vtkDoubleArray* vectors = vtkDoubleArray::SafeDownCast (input->GetPointData()->GetVectors());
  vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast (input->GetPointData()->GetScalars());
//   if (!vectors)
//   {
//     vectors = vtkDoubleArray::SafeDownCast (input->GetCellData()->GetVectors());
//     colors = vtkUnsignedCharArray::SafeDownCast (input->GetCellData()->GetScalars());
//     is_in_points = false;
//   }
  
//   if (!vectors || !colors)
//   {
//     return 0;
//   }
  
  vtkDoubleArray*      newvectors = vtkDoubleArray::New();
  vtkUnsignedCharArray* newcolors = vtkUnsignedCharArray::New();
  vtkPoints* newpoints = vtkPoints::New();
  vtkCellArray* newcells = vtkCellArray::New();
  
  newvectors->SetNumberOfComponents(3);
  newcolors->SetNumberOfComponents(3);
  
  
  

//   if (is_in_points)
//   {
    int i=0;
    int npts = 0;
    npts = input->GetNumberOfPoints();
    
    while(i<npts)
    {
      double vector[3];
      unsigned char color[3];
      double* pt;
      
      pt = points->GetPoint (i);
      if (vectors)
	vectors->GetTupleValue (i, vector);
      if (colors)
	colors->GetTupleValue (i, color);
      
      if(pt[0]>m_XMin && pt[0]<m_XMax &&
	 pt[1]>m_YMin && pt[1]<m_YMax &&
	 pt[2]>m_ZMin && pt[2]<m_ZMax )
      {
	newpoints->InsertNextPoint(pt);
	if (vectors)
	  newvectors->InsertNextTupleValue (vector);
	if (colors)
	  newcolors->InsertNextTupleValue (color);
      }
      
      i++;
    }

    if (vectors)
      output->GetPointData()->SetVectors ( newvectors );
    if (colors)
      output->GetPointData()->SetScalars ( newcolors );
    output->SetPoints ( newpoints );
//   }
//   else
//   {
//     i=0;
//     npts = 0;
//     npts = input->GetCells()->GetNumberOfCells();
//     int* typearray = new int[npts];
    
//     while(i<npts)
//     {
//       vtkCell* cell;
//       double vector[3];
//       unsigned char color[3];
//       double* bounds;
//       double* point;
      
//       vectors->GetTupleValue (i, vector);
//       colors->GetTupleValue (i, color);
//       cell = input->GetCell (i);
//       bounds = cell->GetBounds();
//       typearray[i] = input->GetCellType (i);
//       point = input->GetPoint (cell->GetPointId (0));
      
//       //      std::cout<<""<<std::endl;
      
//       if(bounds[0]>m_XMin && bounds[1]<m_XMax &&
// 	 bounds[2]>m_YMin && bounds[3]<m_YMax &&
// 	 bounds[4]>m_ZMin && bounds[5]<m_ZMax )
//       {
// 	newcells->InsertNextCell (cell);
// 	newvectors->InsertNextTupleValue (vector);
// 	newcolors->InsertNextTupleValue (color);
// 	newpoints->InsertNextPoint(point);
//       }
      
//       i++;
//     }
//     output->SetCells (typearray, newcells);
//     output->GetCellData()->SetVectors ( newvectors );
//     output->GetCellData()->SetScalars ( newcolors );
//     output->SetPoints ( newpoints );
    
//     delete [] typearray;
    
//   }
  

  
  
 
  newpoints->Delete();  
  newvectors->Delete();
  newcolors->Delete();
  newcells->Delete();
  
  

  
//   vtkCellArray* lines = input->GetLines();
//   if( lines==0 )
//     return 0;
//   lines->InitTraversal();


//   int npt, *pto;
//   int test = lines->GetNextCell (npt, pto);
  
//   while( test!=0 )
//   {
//     for( int i=0; i<npt; )
//     {
//       double* pt = points->GetPoint (pto[i]);
//       if( pt[0]>m_XMin && pt[0]<m_XMax &&
//           pt[1]>m_YMin && pt[1]<m_YMax &&
//           pt[2]>m_ZMin && pt[2]<m_ZMax )
//       {
//         output->InsertNextCell (VTK_POLY_LINE, npt, pto);
//         break;
//       }

//       for(int j=0;j<2;j++)
//         i++;
//     }
    
//     test = lines->GetNextCell (npt, pto);
//   }


  return 1;
  
}
