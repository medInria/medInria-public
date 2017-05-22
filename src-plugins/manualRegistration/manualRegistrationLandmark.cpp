/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistrationLandmark.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSeedRepresentation.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkProperty2D.h>
#include <vtkObjectFactory.h>
#include <vtkHandleWidget.h>
#include <manualRegistrationLandmarkController.h>

vtkCxxRevisionMacro(manualRegistrationLandmark, "$Revision: 11092014 $");
vtkStandardNewMacro(manualRegistrationLandmark);

class vtkSeedCallback : public vtkCommand
{
public:
    static vtkSeedCallback *New()
    { 
        return new vtkSeedCallback; 
    }

    vtkSeedCallback() {}

    void SetView ( vtkImageView2D *view )
    {
        this->View = view;
        //view->AddObserver(vtkImageView2D::SliceChangedEvent,this);
    }

    virtual void Execute(vtkObject*, unsigned long event, void *calldata)
    {
        if(event == vtkCommand::EndInteractionEvent)
        {
            int ind[3];
            //// when clicking update slice and orientation
            double Wpos[3];
            double * Dpos;
            Dpos = Roi->GetHandleRepresentation()->GetDisplayPosition();
            int DposInt[2];
            DposInt[0] = (unsigned int)Dpos[0];
            DposInt[1] = (unsigned int)Dpos[1];
            View->GetWorldCoordinatesFromDisplayPosition(DposInt,Wpos);
            Roi->GetHandleRepresentation()->SetWorldPosition(Wpos);
            View->GetImageCoordinatesFromWorldCoordinates(Roi->GetHandleRepresentation()->GetWorldPosition(),ind);
            Roi->SetIndices(ind);
            Roi->ShowOrHide();

            // Remove chosen landmark and related one on other view,
            // cf. manualRegistrationLandmarkControllerCommand::Execute
            if (Roi->GetHandleWidget() && Roi->GetHandleWidget()->GetInteractor())
            {
                std::string keycode = Roi->GetHandleWidget()->GetInteractor()->GetKeySym();
                if (keycode == "BackSpace")
                {
                    Roi->InvokeEvent(vtkCommand::DeleteEvent);
                }
            }
        }
        else if(event == vtkImageView2D::SliceChangedEvent)
        {
            Roi->ShowOrHide();
        }
    }

    void setRoi(manualRegistrationLandmark *Roi)
    {
        this->Roi = Roi;
    }

private:
    manualRegistrationLandmark* Roi;
    vtkImageView2D *View;
};

manualRegistrationLandmark::manualRegistrationLandmark()

{
    // Create the representation
    HandleWidget = vtkHandleWidget::New();
    SeedCallback = vtkSmartPointer<vtkSeedCallback>::New();
    SeedCallback->setRoi(this);
    ToDelete = false;
}

manualRegistrationLandmark::~manualRegistrationLandmark()
{
    HandleWidget->RemoveAllObservers();
    HandleWidget->Delete();
    HandleWidget=0;

    SeedCallback = 0;
}

void manualRegistrationLandmark::SetViewRepRwi(vtkImageView2D * view, vtkPointHandleRepresentation2D * rep,vtkRenderWindowInteractor * rwi)
{
    HandleWidget->SetRepresentation(rep);
    HandleWidget->SetInteractor(rwi);
    this->View = view;
    view->GetImageCoordinatesFromWorldCoordinates(rep->GetWorldPosition(),Indices);
    SeedCallback->SetView(view);
    view->AddObserver(vtkImageView2D::SliceChangedEvent,SeedCallback);
    HandleWidget->SetEnabled(1);
    view->Render();
    if (!HandleWidget->HasObserver(vtkCommand::EndInteractionEvent, SeedCallback))
        HandleWidget->AddObserver(vtkCommand::EndInteractionEvent, SeedCallback, -1);

    this->On();
}

void manualRegistrationLandmark::Off()
{ 
    HandleWidget->Off();
}
void manualRegistrationLandmark::On()
{
    HandleWidget->On();
}

void manualRegistrationLandmark::ShowOrHide()
{
    if (!ToDelete && View)
    {
        if (HandleWidget)
        {
            if (HandleWidget->GetInteractor() && HandleWidget->GetRepresentation())
            {
                if (HandleWidget->GetInteractor()->GetRenderWindow() &&
                        HandleWidget->GetRepresentation()->GetRenderer())
                {
                    if (Indices[View->GetSliceOrientation()]!=View->GetSlice())
                    {
                        Off();
                    }
                    else
                    {
                        On();
                    }
                }
            }
        }
    }
}

vtkPointHandleRepresentation2D * manualRegistrationLandmark::GetHandleRepresentation()
{
    return static_cast<vtkPointHandleRepresentation2D*>(HandleWidget->GetRepresentation());
}

int *  manualRegistrationLandmark::GetIndices()
{
    return Indices;
}

void manualRegistrationLandmark::SetIndices(int ind[3])
{
    Indices[0]=ind[0];Indices[1]=ind[1];Indices[2]=ind[2];
}

void manualRegistrationLandmark::SetToDelete(bool val)
{
    ToDelete = val;
    if (ToDelete)
        Off();
    else
        On();
}

