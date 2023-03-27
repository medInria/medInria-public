/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "vtkContourOverlayRepresentation.h"

#include <vtkCommand.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPointPlacer.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

vtkStandardNewMacro(vtkContourOverlayRepresentation)

//----------------------------------------------------------------------
vtkContourOverlayRepresentation::vtkContourOverlayRepresentation()
{
    this->InteractionOffset[0] = 0.5;
    this->InteractionOffset[1] = 0.5;
    needToSaveState = true;
}

//----------------------------------------------------------------------
vtkContourOverlayRepresentation::~vtkContourOverlayRepresentation()
{
}

void vtkContourOverlayRepresentation::UpdateContourWorldPositionsBasedOnDisplayPositions()
{
    double dispPos[3];
    double W[4];

    for(unsigned int i=0; i<this->Internal->Nodes.size(); i++)
    {
        W[0] = this->Internal->Nodes[i]->WorldPosition[0];
        W[1] = this->Internal->Nodes[i]->WorldPosition[1];
        W[2] = this->Internal->Nodes[i]->WorldPosition[2];

        vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,W[0], W[1], W[2], dispPos);
        this->Renderer->DisplayToNormalizedDisplay( dispPos[0], dispPos[1] );
        this->Internal->Nodes[i]->NormalizedDisplayPosition[0] = dispPos[0];
        this->Internal->Nodes[i]->NormalizedDisplayPosition[1] = dispPos[1];

        for (unsigned int j=0; j<this->Internal->Nodes[i]->Points.size(); j++)
        {
            W[0] = this->Internal->Nodes[i]->Points[j]->WorldPosition[0];
            W[1] = this->Internal->Nodes[i]->Points[j]->WorldPosition[1];
            W[2] = this->Internal->Nodes[i]->Points[j]->WorldPosition[2];

            vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer, W[0], W[1], W[2], dispPos);
            this->Renderer->DisplayToNormalizedDisplay( dispPos[0], dispPos[1] );
            this->Internal->Nodes[i]->Points[j]->NormalizedDisplayPosition[0] = dispPos[0];
            this->Internal->Nodes[i]->Points[j]->NormalizedDisplayPosition[1] = dispPos[1];
        }
    }
}

void vtkContourOverlayRepresentation::WidgetInteraction(double eventPos[2])
{
    Superclass::WidgetInteraction(eventPos);
    needToSaveState = true;
}

int vtkContourOverlayRepresentation::ComputeInteractionState(int X, int Y, int vtkNotUsed(modified))
{
    double pos[3], xyz[3];

    // Cursor position in display
    xyz[0] = static_cast<double>(X);
    xyz[1] = static_cast<double>(Y);
    xyz[2] = 1.0;

    bool foundNearbyPoint = false;

    // Check if a contour point is nearby the cursor position
    for (int i=0; i<this->GetNumberOfNodes(); i++)
    {
        this->GetNthNodeDisplayPosition(i, pos);

        double tol2 = this->PixelTolerance * this->PixelTolerance;

        if ( vtkMath::Distance2BetweenPoints(xyz,pos) <= tol2 )
        {
            if (this->Renderer && this->Renderer->GetRenderWindow())
            {
                this->Renderer->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_HAND);
            }
            this->InteractionState = vtkContourRepresentation::Nearby;
            foundNearbyPoint = true;
            break;
        }
    }

    if (foundNearbyPoint == false)
    {
        if (this->Renderer && this->Renderer->GetRenderWindow())
        {
            this->Renderer->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_CROSSHAIR);
        }
        this->InteractionState = vtkContourRepresentation::Outside;
    }

    return this->InteractionState;
}

int vtkContourOverlayRepresentation::DeleteLastNode()
{
    int result = Superclass::DeleteLastNode();
    if (result)
    {
        needToSaveState = true;
    }
    return result;
}

int vtkContourOverlayRepresentation::DeleteActiveNode()
{
    int result = Superclass::DeleteActiveNode();
    if (result)
    {
        needToSaveState = true;
    }
    return result;
}

int vtkContourOverlayRepresentation::DeleteNthNode( int n )
{
    int result = Superclass::DeleteNthNode(n);
    if (result)
    {
        needToSaveState = true;
    }
    return result;
}

void vtkContourOverlayRepresentation::ClearAllNodes()
{
    Superclass::ClearAllNodes();
}

int vtkContourOverlayRepresentation::AddNodeOnContour( int X, int Y )
{
    int result = Superclass::AddNodeOnContour(X,Y);
    if (result)
    {
        needToSaveState = true;
    }
    return result;
}

int vtkContourOverlayRepresentation::AddNodeAtDisplayPosition(int X, int Y)
{
    int result = Superclass::AddNodeAtDisplayPosition(X,Y);
    if (result)
    {
        needToSaveState = true;
    }
    this->InvokeEvent(vtkCommand::PlacePointEvent);
    return result;
}

int vtkContourOverlayRepresentation::AddNodeAtDisplayPosition(double displayPos[2])
{
    double worldPos[4];
    double worldOrient[9] = {1.0,0.0,0.0,
                             0.0,1.0,0.0,
                             0.0,0.0,1.0};

    // Compute the world position from the display position
    // based on the concrete representation's constraints
    // If this is not a valid display location return 0
    if ( !this->PointPlacer->ComputeWorldPosition( this->Renderer,
                                                   displayPos, worldPos,
                                                   worldOrient) )
    {
        return 0;
    }

    // this is a hack: the previous method call should be able to compute
    // the correct world position. The problem is it uses the camera's
    // focal point, which is not correctly updated in at least one case:
    // when the views are linked and the 'current point' is changed
    // by double-clicking.
    vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, displayPos[0], displayPos[1], 0.0, worldPos);

    this->AddNodeAtPositionInternal( worldPos, worldOrient, displayPos );
    return 1;
}

int vtkContourOverlayRepresentation::GetNthNodeWorldPosition(int n, double worldPos[])
{
    return vtkContourRepresentation::GetNthNodeWorldPosition(n, worldPos);
}

int vtkContourOverlayRepresentation::GetIntermediatePointWorldPosition(int n, int idx, double point[])
{
    return vtkContourRepresentation::GetIntermediatePointWorldPosition(n, idx, point);
}

int vtkContourOverlayRepresentation::FindClosestPointOnContour(int X, int Y, double worldPos[], int *idx)
{
    return vtkContourRepresentation::FindClosestPointOnContour(X,Y,worldPos,idx);
}
