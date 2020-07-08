/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "vtkFiberPickerCallback.h"
#include <vtkCellPicker.h>
#include <vtkAlgorithmOutput.h>
#include <vtkPolyDataAlgorithm.h>


vtkPolyData*extractPolyDataFromOuputPort(vtkAlgorithmOutput *inputPort)
{
    vtkPolyData *poResPolyData = nullptr;

    if (inputPort)
    {
        bool bIsVtkPolyDataAlgorithm = inputPort->GetProducer()->IsA("vtkPolyDataAlgorithm");
        if (bIsVtkPolyDataAlgorithm)
        {
            poResPolyData = static_cast<vtkPolyDataAlgorithm*>(inputPort->GetProducer())->GetOutput();
        }
    }

    return poResPolyData;
}

void vtkFiberPickerCallback::Execute(vtkObject *caller, unsigned long event, void*)
{
    vtkCellPicker *picker = vtkCellPicker::SafeDownCast(caller);
    if( !picker )
    {
        return;
    }

    int cellid = picker->GetCellId();
    picker->SetPath (nullptr); // to prevent a bounding box to appear

    if( cellid < 0 )
    {
        this->PickedCellId = -1;
        this->PickedActor->SetVisibility (0);
        return;
    }
    else
    {
        vtkPolyData *poExtractPolyDataFromInput = extractPolyDataFromOuputPort(inputPort);

        this->PickedCellId = cellid;
        if( poExtractPolyDataFromInput )
        {
            vtkCellArray *lines = poExtractPolyDataFromInput->GetLines();
            if( !lines )
            {
                return;
            }

            vtkCellArray *array = vtkCellArray::New();
            array->InsertNextCell ( poExtractPolyDataFromInput->GetCell (cellid) );

            this->PickedFiber->Initialize();
            this->PickedFiber->SetPoints ( poExtractPolyDataFromInput->GetPoints() );

            switch( poExtractPolyDataFromInput->GetCellType (cellid) )
            {
                case VTK_POLY_LINE:
                    this->PickedFiber->SetLines (array);
                    break;

                default:
                    break;
            }

            array->Delete();

            this->PickedMapper->SetInputData ( this->PickedFiber );
            this->PickedActor->SetVisibility (1);
        }
    }
}

void vtkFiberPickerCallback::DeletePickedCell()
{
    if( !this->FiberImage || !this->inputPort || this->PickedCellId==-1)
    {
        return;
    }

    vtkPolyData *poExtractPolyDataFromInput = extractPolyDataFromOuputPort(inputPort);

    vtkCell *pickedCell = poExtractPolyDataFromInput->GetCell (this->PickedCellId);

    int npts       = pickedCell->GetNumberOfPoints();
    vtkIdType *pto = pickedCell->GetPointIds()->GetPointer (0);

    vtkCellArray *realLines = this->FiberImage->GetLines();
    if( !realLines )
    {
        return;
    }

    vtkPoints *points = poExtractPolyDataFromInput->GetPoints();
    if( points != this->FiberImage->GetPoints() )
    {
        return;
    }

    realLines->InitTraversal();
    vtkIdType rnpts;
    vtkIdType const *rpto;

    vtkIdType test = realLines->GetNextCell (rnpts, rpto);
    int cellid = 0;
    while ( test!=0 )
    {
        if( rnpts == npts )
        {
            int i=0;
            for( i=0; i<npts && pto[i]==rpto[i];)
            {
                i++;
            }

            if( i==npts && pto[i-1]==rpto[i-1])
            {
                // Avoid const*
                vtkIdType *nonConstRpto;
                for (int j=0; j<npts; ++j)
                {
                    nonConstRpto[j]  = rpto[j];
                }
                
                // found and remove
                for( int j=0; j<npts; j++)
                {
                    nonConstRpto[j]=nonConstRpto[0]; // collapse the fiber on itself
                }

                this->FiberImage->Modified();

                if( this->FibersManager )
                {
                    this->FibersManager->Execute(); // force the box widget to refresh
                }

                this->PickedActor->SetVisibility (0);

                break;
            }
        }
        test = realLines->GetNextCell (rnpts, rpto);
        cellid++;
    }
}

