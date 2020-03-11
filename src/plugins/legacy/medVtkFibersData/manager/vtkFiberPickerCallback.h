#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medVtkFibersDataPluginExport.h>

#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkFibersManager.h>


/**
   vtkFiberPickerCallback declaration & implementation
*/
class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkFiberPickerCallback : public vtkCommand
{
public:
    static vtkFiberPickerCallback*New()
    {
        return new vtkFiberPickerCallback;
    }

    virtual void Execute ( vtkObject*, unsigned long, void*);

    void SetInputConnection(vtkAlgorithmOutput *input)
    {
        inputPort = input;
    }

    void SetFiberImage (vtkPolyData *image)
    {
        this->FiberImage = image;
    }

    vtkPolyData*GetFiberImage() const
    {
        return this->FiberImage;
    }

    void SetFibersManager (vtkFibersManager *manager)
    {
        this->FibersManager = manager;
    }

    vtkActor*GetPickedActor() const
    {
        return this->PickedActor;
    }

    void DeletePickedCell();

protected:
    vtkFiberPickerCallback()
    {
        inputPort = nullptr;
        this->FiberImage   = nullptr;
        this->FibersManager= nullptr;
        this->PickedFiber  = vtkPolyData::New();
        this->PickedMapper = vtkPolyDataMapper::New();
        this->PickedActor  = vtkActor::New();

        this->PickedMapper->SetInputData ( this->PickedFiber );
        this->PickedActor->SetMapper ( this->PickedMapper );
        this->PickedActor->GetProperty()->SetColor ( 1.0, 0.0, 0.0 );
        this->PickedActor->GetProperty()->SetLineWidth ( 5.0 );

        this->PickedFiber->Initialize();
        this->PickedFiber->Allocate();

        this->PickedCellId = -1;
    }

    ~vtkFiberPickerCallback()
    {
        this->PickedFiber->Delete();
        this->PickedMapper->Delete();
        this->PickedActor->Delete();
    }

private:
    vtkAlgorithmOutput *inputPort;
    vtkPolyData *FiberImage;

    vtkPolyData *PickedFiber;
    vtkPolyDataMapper *PickedMapper;
    vtkActor *PickedActor;
    int PickedCellId;

    vtkFibersManager *FibersManager;
};
