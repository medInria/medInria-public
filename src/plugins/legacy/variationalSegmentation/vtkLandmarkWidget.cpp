/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <vtkLandmarkWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkWidgetRepresentation.h>

vtkStandardNewMacro(vtkLandmarkWidget)

void vtkLandmarkWidgetCommand::Execute(vtkObject *   caller, 
                                       unsigned long event,
                                       void *        callData)
{
    vtkLandmarkWidget *l = vtkLandmarkWidget::SafeDownCast(caller);
    vtkHandleWidget *widget = vtkHandleWidget::SafeDownCast(caller);

    if (!isLocked)
    {
        if (event == vtkCommand::InteractionEvent)
        {
            if (this->Landmark && l)
            {
                this->Landmark->SetCenter (l->GetCenter());
                if (this->Landmark->GetInteractor())
                {
                    this->Landmark->GetInteractor()->Render();
                }
            }
            if (this->Widget2D == widget)
            {
                if (this->Landmark)
                {
                    this->Landmark->SetCenter(dynamic_cast<vtkPointHandleRepresentation2D*>(this->Widget2D->GetRepresentation())->GetWorldPosition());
                }
            }
        }
        if ( (event == vtkCommand::EnableEvent) ||
             (event == vtkCommand::DisableEvent) )
        {
            if (this->Landmark && l)
            {
                this->Landmark->SetEnabled (l->GetEnabled());
            }
        }

        unsigned long eventToPropagate = event;
        if ( event == vtkCommand::EndInteractionEvent)
        {
            if (widget)
            {
                if (widget->GetInteractor()->GetControlKey())
                {
                    Landmark->InvokeEvent(vtkCommand::DeleteEvent);
                    eventToPropagate  = vtkCommand::DeleteEvent;
                }
                else
                {
                    Landmark->updateLandmarksPosFromWidget2D();
                    if (this->Landmark->GetBigBrother())
                    {
                        this->Landmark->InvokeEvent(vtkCommand::ModifiedEvent);
                    }
                    else
                    {
                        eventToPropagate = vtkCommand::ModifiedEvent;
                    }
                    l = Landmark;
                }
            }
            else
            {
                if (l && (l->GetInteractor()->GetControlKey()))
                {
                    Landmark->InvokeEvent(vtkCommand::DeleteEvent);
                }
            }
        }
        if ( event == vtkImageView2D::SliceChangedEvent )
        {
            this->Landmark->showOrHide2DWidget();
            eventToPropagate = event;
        }

        if (!this->Landmark->GetBigBrother())
        {
            this->Landmark->PropagateEventToLittleBrothers(eventToPropagate,l);
        }
    }
}

void vtkLandmarkWidgetCommand::SetLandmark (vtkLandmarkWidget *l)
{
    this->Landmark = l;
}

void vtkLandmarkWidgetCommand::SetWidget2D (vtkHandleWidget *widget)
{
    this->Widget2D = widget;
}

vtkLandmarkWidget::vtkLandmarkWidget()
{
    this->Command = vtkLandmarkWidgetCommand::New();
    this->Command->SetLandmark (this);
    this->Command->unlock();
    this->Value = 0.0;
    this->Widget2D = vtkHandleWidget::New();
    this->Command->SetWidget2D (this->Widget2D);
    this->AddObserver(vtkCommand::EndInteractionEvent, this->Command);
    this->Widget2D->AddObserver(vtkCommand::EndInteractionEvent, this->Command);
    this->View2D = nullptr;
    this->View3D = nullptr;
    this->ToDelete = false;
    this->BigBrother = nullptr;
    this->LittleBrothers = new QList<vtkLandmarkWidget *>();
}

vtkLandmarkWidget::~vtkLandmarkWidget()
{
    this->Off();

    this->Widget2D->RemoveAllObservers();
    this->RemoveAllObservers();
    if (View2D)
    {
        View2D->RemoveObserver(this->Command);
    }
    if (BigBrother)
    {
        this->RemoveObserver(BigBrother->GetCommand());
        BigBrother->GetLittleBrothers()->removeAt(BigBrother->GetLittleBrothers()->indexOf(this));
    }
    
    for(int i = 0;i<LittleBrothers->size();i++)
    {
        LittleBrothers->at(i)->RemoveObserver(this->Command);
    }
    LittleBrothers->clear();
    delete LittleBrothers;

    this->Command->Delete();
    this->Widget2D->Delete();
}

void vtkLandmarkWidget::SetEnabled(int val)
{
    Superclass::SetEnabled(val);
    this->Interactor->RemoveObservers(vtkCommand::MouseMoveEvent,
                                      reinterpret_cast<vtkCommand*>(this->EventCallbackCommand));
    // the sphere widget should not be movable. All movements should go through the 2d view -> widget2d.
}

void vtkLandmarkWidget::SetView2D(vtkImageView2D *View)
{
    if (View2D)
    {
        View2D->RemoveObserver(this->Command);
    }

    View2D = View;
    View2D->AddObserver(vtkImageView2D::SliceChangedEvent, Command);
}

void vtkLandmarkWidget::AddBrothers(vtkLandmarkWidget *LittleBrother)
{
    this->LittleBrothers->append(LittleBrother);
    LittleBrother->AddObserver(vtkCommand::ModifiedEvent, this->Command);
    LittleBrother->AddObserver(vtkCommand::DeleteEvent, this->Command);
}

QList<vtkLandmarkWidget*> * vtkLandmarkWidget::GetLittleBrothers()
{
    return this->LittleBrothers;
}

void vtkLandmarkWidget::PropagateEventToLittleBrothers(unsigned long event, vtkLandmarkWidget *l)
{
    int indexofL = -1;
    this->Command->lock();
    
    if (event == vtkCommand::ModifiedEvent)
    {
        if (l)
        {
            if (l!=this)
            {
                this->updatePosition(l->GetCenter());
                this->showOrHide2DWidget();
                indexofL = LittleBrothers->indexOf(l);
            }
            for (int i = 0; i<LittleBrothers->size(); i++)
            {
                if (i != indexofL)
                {
                    LittleBrothers->at(i)->updatePosition(this->GetCenter());
                    LittleBrothers->at(i)->showOrHide2DWidget();
                }
            }
            this->InvokeEvent(vtkCommand::PlacePointEvent);
        }
    }

    if (event == vtkCommand::DeleteEvent)
    {
        if (l != this)
        {
            if (l)
            {
                this->InvokeEvent(vtkCommand::DeleteEvent);
                indexofL = LittleBrothers->indexOf(l);
            }
            
            for (int i = 0; i<LittleBrothers->size(); i++)
            {
                if (i != indexofL)
                {
                    LittleBrothers->at(i)->InvokeEvent(vtkCommand::DeleteEvent);
                }
            }
        }
    }
    this->Command->unlock();
}

void vtkLandmarkWidget::showOrHide2DWidget()
{
    if (Widget2D->GetInteractor() && Widget2D->GetRepresentation()->GetRenderer())
    {
        if (Widget2D->GetInteractor()->GetRenderWindow())
        {
            if (indices[View2D->GetSliceOrientation()] != View2D->GetSlice() || ToDelete)
            {
                Widget2D->Off();
            }
            else
            {
                Widget2D->On();
            }
        }
    }
}        

void vtkLandmarkWidget::updateLandmarksPosFromWidget2D()
{
    vtkPointHandleRepresentation2D *pointRep = dynamic_cast<vtkPointHandleRepresentation2D*> (Widget2D->GetRepresentation());
    this->SetCenter(pointRep->GetWorldPosition());
    int new_indices[3];
    View2D->GetImageCoordinatesFromWorldCoordinates(pointRep->GetWorldPosition(), new_indices);
    int orientation = View2D->GetSliceOrientation();
    new_indices[orientation] = indices[orientation]; // the slice id of the current Orientation cannot change, it would not make sense. This line is here to prevent that.
    SetIndices(new_indices);
}

void vtkLandmarkWidget::updatePosition(double *worldPos)
{
    vtkPointHandleRepresentation2D *pointRep = dynamic_cast<vtkPointHandleRepresentation2D*> (Widget2D->GetRepresentation());
    pointRep->SetWorldPosition(worldPos);
    this->SetCenter(worldPos);
    int new_indices[3];
    View2D->GetImageCoordinatesFromWorldCoordinates(pointRep->GetWorldPosition(), new_indices);
    SetIndices(new_indices);
}

void vtkLandmarkWidget::cleanUpLittleBrothersReferences()
{
    for(int i = 0; i<LittleBrothers->size(); i++)
    {
        LittleBrothers->at(i)->SetBigBrother(nullptr);
    }
}
