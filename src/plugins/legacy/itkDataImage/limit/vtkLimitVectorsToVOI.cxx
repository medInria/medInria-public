/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLimitVectorsToVOI.h"

#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>

#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkCell.h>

vtkStandardNewMacro(vtkLimitVectorsToVOI)

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

    vtkPoints* points = input->GetPoints();

    vtkDoubleArray* vectors = vtkDoubleArray::SafeDownCast (input->GetPointData()->GetVectors());
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast (input->GetPointData()->GetScalars());
    vtkDoubleArray*      newvectors = vtkDoubleArray::New();
    vtkUnsignedCharArray* newcolors = vtkUnsignedCharArray::New();
    vtkPoints* newpoints = vtkPoints::New();
    vtkCellArray* newcells = vtkCellArray::New();

    newvectors->SetNumberOfComponents(3);
    newcolors->SetNumberOfComponents(3);

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
            vectors->GetTypedTuple (i, vector);
        if (colors)
            colors->GetTypedTuple (i, color);

        if(pt[0]>m_XMin && pt[0]<m_XMax &&
                pt[1]>m_YMin && pt[1]<m_YMax &&
                pt[2]>m_ZMin && pt[2]<m_ZMax )
        {
            newpoints->InsertNextPoint(pt);
            if (vectors)
                newvectors->InsertNextTypedTuple (vector);
            if (colors)
                newcolors->InsertNextTypedTuple (color);
        }

        i++;
    }

    if (vectors)
    {
        output->GetPointData()->SetVectors ( newvectors );
    }
    if (colors)
    {
        output->GetPointData()->SetScalars ( newcolors );
    }
    output->SetPoints ( newpoints );

    newpoints->Delete();
    newvectors->Delete();
    newcolors->Delete();
    newcells->Delete();

    return 1;
}
