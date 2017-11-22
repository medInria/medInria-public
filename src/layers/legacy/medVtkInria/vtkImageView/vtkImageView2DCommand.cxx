/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef _MSC_VER
#  pragma warning (disable : 4018)
#endif

#include "vtkImageView2DCommand.h"

#include <vtkInteractorStyle.h>
#include <vtkAssemblyPath.h>
#include <vtkCamera.h>
#include <vtkImageView2D.h>
#include <vtkInteractorStyleImageView2D.h>
#include "vtkAbstractPropPicker.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"
#include "vtkImageViewCornerAnnotation.h"
#include "vtkCornerAnnotation.h"

#include <string>
#include <sstream>

vtkImageView2DCommand::vtkImageView2DCommand()
{
    this->Viewer = NULL;
}

vtkImageView2DCommand::~vtkImageView2DCommand()
{
}


void
vtkImageView2DCommand::Execute(vtkObject*    caller,
                               unsigned long event,
                               void*         callData)
{
    vtkInteractorStyleImageView2D *isi =
            static_cast<vtkInteractorStyleImageView2D *>(caller);

    if (!isi || !this->Viewer || (!this->Viewer->GetMedVtkImageInfo() || !this->Viewer->GetMedVtkImageInfo()->initialized))
    {
        return;
    }

    // Reset
    if (event == vtkImageView2DCommand::ResetViewerEvent)
    {
        this->Viewer->Reset();
        this->Viewer->Render();
        return;
    }
    // Reset
    if (event == vtkImageView2DCommand::ResetWindowLevelEvent)
    {
        this->Viewer->ResetWindowLevel();
        this->Viewer->Render();
        return;
    }

    // Start
    if (event == vtkImageView2DCommand::StartWindowLevelEvent)
    {
        this->InitialWindow = this->Viewer->GetColorWindow();
        this->InitialLevel = this->Viewer->GetColorLevel();
        return;
    }

    // Adjust the window level here
    if (event == vtkImageView2DCommand::WindowLevelEvent)
    {

        int *size = this->Viewer->GetRenderWindow()->GetSize();

        double* range = this->Viewer->GetMedVtkImageInfo()->scalarRange;
        double windowImage = range[1]-range[0];
        double levelImage = 0.5*(range[1]+range[0]);

        double window = this->InitialWindow;
        double level = this->InitialLevel;

        double mouseToLevelSpeed = 0.25;

        // Compute normalized delta
        double dx = 4.0 *
                (isi->GetWindowLevelCurrentPosition()[0] -
                isi->GetWindowLevelStartPosition()[0]) / size[0];
        double dy = 4.0 *
                (isi->GetWindowLevelStartPosition()[1] -
                isi->GetWindowLevelCurrentPosition()[1]) / size[1];


        dx = dx * windowImage * mouseToLevelSpeed;
        dy = dy * levelImage * mouseToLevelSpeed;

        // Compute new window level
        double newWindow = dx + window;
        double newLevel = level - dy;

        // Keep window values above 0
        if (newWindow < 0)
        {
            double levelMin = range[0] - 0.5 * windowImage;
            double levelMax = range[1] + 0.5 * windowImage;

            newWindow = std::min(0.1, (levelMax-levelMin) / 1000);
        }

        this->Viewer->SetColorWindowLevel(newWindow, newLevel);
        return;
    }

    if (event == vtkImageView2DCommand::CharEvent)
    {
        vtkRenderWindowInteractor *rwi = this->Viewer->GetRenderWindow()->GetInteractor();

        if (rwi->GetKeyCode() == 'n')
        {
            this->Viewer->SetInterpolate ((this->Viewer->GetInterpolate() + 1)%2);
            this->Viewer->Render();
        }

        return;
    }

    // Start Slice Move
    if (event == vtkImageView2DCommand::StartSliceMoveEvent)
    {
        return;
    }

    // End Slice Move
    if (event == vtkImageView2DCommand::EndSliceMoveEvent)
    {
        return;
    }

    // Move Slice
    if (event == vtkImageView2DCommand::SliceMoveEvent)
    {
        this->Viewer->SetSlice (this->Viewer->GetSlice()+isi->GetSliceStep());
        this->Viewer->Render();
        return;
    }

    // Start Slice Move
    if (event == vtkImageView2DCommand::StartTimeChangeEvent)
    {
        return;
    }

    // End Slice Move
    if (event == vtkImageView2DCommand::EndTimeChangeEvent)
    {
        return;
    }

    // Move Slice
    /*if (event == vtkImageView2DCommand::TimeChangeEvent)
    {
        this->Viewer->SetTimeIndex (this->Viewer->GetTimeIndex()+isi->GetSliceStep());
        this->Viewer->Render();
        return;
    }*/

    // Position requested
    if (event == vtkImageView2DCommand::RequestedPositionEvent)
    {
        double position[3];
        this->Viewer->GetWorldCoordinatesFromDisplayPosition (isi->GetRequestedPosition (), position);
        this->Viewer->SetCurrentPoint(position);
        this->Viewer->Render();
        return;
    }

    // The cursor left the view, cursor position needs to be reset
    if (event == vtkCommand::LeaveEvent)
    {
        this->Viewer->UpdateCursorPosition(this->Viewer->GetCurrentPoint());
        this->Viewer->Render();
        return;
    }

    // Cursor Position requested
    if (event == vtkImageView2DCommand::RequestedCursorInformationEvent)
    {
        double position[3];
        this->Viewer->GetWorldCoordinatesFromDisplayPosition (isi->GetRequestedPosition (), position);
        this->Viewer->vtkImageView::UpdateCursorPosition(position);
        this->Viewer->Render();
        return;
    }

    // default move : align cursor and update annotation accordingly
    if( event == vtkImageView2DCommand::DefaultMoveEvent)
    {
        vtkRenderWindowInteractor *rwi = this->Viewer->GetInteractor();
        if (!rwi)
            return;

        int* xy = rwi->GetEventPosition();
        double position[3];
        this->Viewer->GetWorldCoordinatesFromDisplayPosition (xy, position);
        this->Viewer->SetCurrentPoint(position);
        if (isi->GetState() == VTKIS_NONE)
            this->Viewer->Render();

        return;
    }

    // Zooming
    if (event == vtkImageView2DCommand::CameraZoomEvent)
    {
        this->Viewer->Modified();
        return;
    }

    // Panning
    if (event == vtkImageView2DCommand::CameraPanEvent)
    {
        this->Viewer->Modified();
        return;

    }

}


