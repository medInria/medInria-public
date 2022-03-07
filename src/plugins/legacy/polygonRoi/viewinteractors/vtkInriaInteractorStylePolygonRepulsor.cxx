/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <polygonLabel.h>

#include <vtkCallbackCommand.h>
#include <viewinteractors/vtkContourOverlayRepresentation.h>
#include <vtkContourRepresentation.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

vtkStandardNewMacro(vtkInriaInteractorStylePolygonRepulsor)

//----------------------------------------------------------------------------
vtkInriaInteractorStylePolygonRepulsor::vtkInriaInteractorStylePolygonRepulsor()
{
    this->Position[0] = this->Position[1] = 0;
    this->On = 0;
    this->Radius = 25;

    this->RepulsorProperty = vtkProperty2D::New();
    this->RepulsorProperty->SetColor(1,0.6,0);
    this->RepulsorProperty->SetLineWidth(3);

    this->RepulsorActor = vtkCircleActor2D::New();
    this->RepulsorActor->SetVisibility(0);
    this->RepulsorActor->SetProperty(this->RepulsorProperty);
    this->manager = nullptr;
}

//----------------------------------------------------------------------------
vtkInriaInteractorStylePolygonRepulsor::~vtkInriaInteractorStylePolygonRepulsor()
{
    this->RepulsorActor->Delete();
    this->RepulsorProperty->Delete();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStylePolygonRepulsor::OnMouseMove()
{
    if (!this->Interactor)
    {
        return;
    }

    if (!this->On)
    {
        Superclass::OnMouseMove();
        return;
    }

    this->Position[0] = this->Interactor->GetEventPosition()[0];
    this->Position[1] = this->Interactor->GetEventPosition()[1];
    this->RepulsorActor->SetCenter(this->Position[0], this->Position[1]);
    this->RedefinePolygons();
    this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStylePolygonRepulsor::OnLeftButtonDown()
{
    if (!this->Interactor)
    {
        return;
    }
    if (!manager)
    {
        return;
    }
    if (!this->CurrentRenderer)
    {
        return;
    }

    this->Position[0] = this->Interactor->GetEventPosition()[0];
    this->Position[1] = this->Interactor->GetEventPosition()[1]; 
    this->FindPokedRenderer(this->Position[0], this->Position[1]);

    this->On = 1;

    this->CurrentRenderer->AddViewProp(this->RepulsorActor);
    this->RepulsorActor->SetVisibility(0);

    //-------------------------------------------COMPUTE RADIUS--------------------------------------------------------------//
    double pos[2];
    pos[0] = (double)Position[0];
    pos[1] = (double)Position[1];
    double dist = manager->getMinimumDistanceFromNodesToMouse(pos);
    this->Radius = (int)((dist+0.5)*0.8);
    this->RepulsorActor->SetRadius(this->Radius);
    this->RepulsorActor->SetCenter(this->Position[0], this->Position[1]);
    this->RepulsorActor->SetVisibility(1);
    this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStylePolygonRepulsor::OnLeftButtonUp()
{
    if (!this->Interactor)
    {
        return;
    }
    if (!manager)
    {
        return;
    }
    if (!this->CurrentRenderer)
    {
        return;
    }
    this->CurrentRenderer->RemoveViewProp(this->RepulsorActor);
    this->On = 0;
    this->Interactor->Render();

    for(int i = 0; i<this->ListPolygonsToSave.size(); i++)
    {
        vtkContourOverlayRepresentation::SafeDownCast(this->ListPolygonsToSave[i]->getContour()->GetContourRepresentation())->SetneedToSaveState(true);
        this->ListPolygonsToSave[i]->saveState();
    }
    this->ListPolygonsToSave.clear();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStylePolygonRepulsor::SetManager(polygonLabel *closestManagerInSlice)
{
    this->manager = closestManagerInSlice;
    OnLeftButtonUp();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStylePolygonRepulsor::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Position: " << this->Position[0] << "," << this->Position[1] << endl;
}

bool vtkInriaInteractorStylePolygonRepulsor::IsInRepulsorDisk(double *pt)
{
    int X = pt[0]-Position[0];
    int Y = pt[1]-Position[1];
    int RadiusSquare = this->Radius * this->Radius;
    return (X*X + Y*Y < RadiusSquare);
}

void vtkInriaInteractorStylePolygonRepulsor::RedefinePolygons()
{
    if (!manager)
    {
        return;
    }
    for (polygonRoi * roi : manager->getRois())
    {
        bool contourChanged = false;
        if (roi->isVisible())
        {
            vtkContourWidget *contour = roi->getContour();
            vtkPolyData *polyData = vtkPolyData::New();
            contour->GetContourRepresentation()->GetNodePolyData(polyData);
            vtkPoints *points = polyData->GetPoints();
            int n = 0;
            QList<double*> listPoints;
            for (int k = 0; k < points->GetNumberOfPoints(); k++)
            {
                double *point = new double[3]();
                point[0] = points->GetPoint(k)[0];
                point[1] = points->GetPoint(k)[1];
                point[2] = points->GetPoint(k)[2];
                listPoints.append(point);
            }
            for (int k = 0; k < listPoints.size(); k++)
            {
                float maxN = 10.0;
                float minD = 10.0;
                float maxD = 70.0;
                double pt1[3];
                DisplayPointFromPolygon(pt1,listPoints,k);
                if (this->IsInRepulsorDisk(pt1))
                {
                    double pt1dx = pt1[0]-this->Position[0];
                    double pt1dx2 = pt1dx * pt1dx;
                    double pt1dy =  pt1[1]-this->Position[1];
                    double  pt1dy2 = pt1dy * pt1dy;
                    double pt1d = sqrt(pt1dx2 + pt1dy2);
                    pt1[0] += pt1dx/pt1d*this->Radius-pt1dx;
                    pt1[1] += pt1dy/pt1d*this->Radius-pt1dy;
                    this->Interactor->FindPokedRenderer(this->Position[0],this->Position[1])->SetDisplayPoint(pt1);
                    this->Interactor->FindPokedRenderer(this->Position[0],this->Position[1])->DisplayToWorld();
                    double *ptWorldtmp = this->Interactor->FindPokedRenderer(this->Position[0],this->Position[1])->GetWorldPoint();
                    double *ptWorld1 = new double[3]();
                    ptWorld1[0] = ptWorldtmp[0];
                    ptWorld1[1] = ptWorldtmp[1];
                    ptWorld1[2] = ptWorldtmp[2];
                    listPoints.replace(k,ptWorld1);
                    contourChanged= true;

                    for( int delta = -1; delta <= 1; delta++ )
                    {
                        int D = k+delta;
                        if(D==-1)
                        {
                            D = listPoints.size()-1;
                        }
                        else if(D==listPoints.size())
                        {
                            D = 0;
                        }

                        if(D!=k && D>=0 && D<listPoints.size())
                        {
                            double pt2[3];
                            DisplayPointFromPolygon(pt2,listPoints,D);
                            double distance = sqrt(vtkMath::Distance2BetweenPoints(pt2,pt1));
                            if(distance<=minD && distance<this->Radius)
                            {
                                listPoints.removeAt(D);
                                if(delta==-1)
                                {
                                    k--;
                                }
                            }
                            else if((distance>=maxD || distance>=this->Radius*1.5) && n<maxN)
                            {
                                double pt3[3] = {0,0,0};
                                pt3[0] = (pt2[0]+pt1[0])/2.0 ;
                                pt3[1] = (pt2[1]+pt1[1])/2.0 ;
                                pt3[2] = (pt2[2]+pt1[2])/2.0 ;

                                this->Interactor->FindPokedRenderer(this->Position[0],this->Position[1])->SetDisplayPoint(pt3);
                                this->Interactor->FindPokedRenderer(this->Position[0],this->Position[1])->DisplayToWorld();
                                double *ptWorld3 = new double[3]();
                                ptWorld3[0] = ptWorldtmp[0];
                                ptWorld3[1] = ptWorldtmp[1];
                                ptWorld3[2] = ptWorldtmp[2];
                                int index = (delta==-1)? k : k+1;
                                if(delta==-1)
                                {
                                    k++;
                                }
                                listPoints.insert(index,ptWorld3);
                                n++;
                            }
                        }
                    }
                }
            }
            if (contourChanged)
            {
                points->Reset();
                for (int k = 0; k < listPoints.size(); k++)
                {
                    points->InsertNextPoint(listPoints[k]);
                }
                polyData->SetPoints(points);
                contour->Initialize(polyData);
                if (!this->ListPolygonsToSave.contains(roi))
                {
                    this->ListPolygonsToSave.append(roi);
                }
            }
        }

    }
}

void vtkInriaInteractorStylePolygonRepulsor::DisplayPointFromPolygon(double *displayPoint, QList<double*> list, int ind)
{
    this->Interactor->FindPokedRenderer(this->Position[0], this->Position[1])->SetWorldPoint(list[ind]);
    this->Interactor->FindPokedRenderer(this->Position[0], this->Position[1])->WorldToDisplay();
    double *ptDisplay = this->Interactor->FindPokedRenderer(this->Position[0], this->Position[1])->GetDisplayPoint();
    displayPoint[0] = ptDisplay[0];
    displayPoint[1] = ptDisplay[1];
    displayPoint[2] = ptDisplay[2];
}
