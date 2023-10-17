#include "manualRegistration.h"
#include "manualRegistrationLandmarkController.h"

#include <medMessageController.h>
#include <medVtkViewBackend.h>

#include <vtkCellPicker.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPoints.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>

static QString indexColors[] = {
    "#00FECF", "#B05B6F", "#8CD0FF", "#3B9700", "#04F757", "#C8A1A1", "#1E6E00", "#C2FF99",
    "#7900D7", "#A77500", "#6367A9", "#A05837", "#6B002C", "#772600", "#D790FF", "#9B9700",
    "#549E79", "#FFF69F", "#201625", "#72418F", "#BC23FF", "#99ADC0", "#3A2465", "#922329",
    "#5B4534", "#FDE8DC", "#404E55", "#0089A3", "#CB7E98", "#A4E804", "#324E72", "#6A3A4C",
    "#83AB58", "#001C1E", "#D1F7CE", "#004B28", "#C8D0F6", "#A3A489", "#806C66", "#222800",
    "#BF5650", "#E83000", "#66796D", "#DA007C", "#FF1A59", "#8ADBB4", "#1E0200", "#5B4E51",
    "#C895C5", "#320033", "#FF6832", "#66E1D3", "#CFCDAC", "#D0AC94", "#7ED379", "#012C58", 

    "#000000", "#FFFF00", "#1CE6FF", "#FF34FF", "#FF4A46", "#008941", "#006FA6", "#A30059",
    "#FFDBE5", "#7A4900", "#0000A6", "#63FFAC", "#B79762", "#004D43", "#8FB0FF", "#997D87",
    "#5A0007", "#809693", "#FEFFE6", "#1B4400", "#4FC601", "#3B5DFF", "#4A3B53", "#FF2F80",
    "#61615A", "#BA0900", "#6B7900", "#00C2A0", "#FFAA92", "#FF90C9", "#B903AA", "#D16100",
    "#DDEFFF", "#000035", "#7B4F4B", "#A1C299", "#300018", "#0AA6D8", "#013349", "#00846F",
    "#372101", "#FFB500", "#C2FFED", "#A079BF", "#CC0744", "#C0B9B2", "#001E09", "#575329",
    "#00489C", "#6F0062", "#0CBD66", "#EEC3FF", "#456D75", "#B77B68", "#7A87A1", "#788D66",
    "#885578", "#FAD09F", "#FF8A9A", "#D157A0", "#BEC459", "#456648", "#0086ED", "#886F4C",
    "#34362D", "#B4A8BD", "#00A6AA", "#452C2C", "#636375", "#A3C8C9", "#FF913F", "#938A81"
};

vtkStandardNewMacro(manualRegistrationLandmarkController)

//----------------------------------------------------------------------------
manualRegistrationLandmarkControllerCommand::manualRegistrationLandmarkControllerCommand()
{
    this->Controller = nullptr;
}

//----------------------------------------------------------------------------
manualRegistrationLandmarkControllerCommand::~manualRegistrationLandmarkControllerCommand()
{
}

//----------------------------------------------------------------------------
void manualRegistrationLandmarkControllerCommand::SetController(manualRegistrationLandmarkController* arg)
{
    if (arg != this->Controller)
    {
        this->Controller = arg;
    }
}

//----------------------------------------------------------------------------
void manualRegistrationLandmarkControllerCommand::Execute(vtkObject *caller, unsigned long event, void *vtkNotUsed(callData))
{
    if (!this->Controller) 
        return;

    if ( event == vtkCommand::LeftButtonPressEvent )
    {
        /**************  Landmark addition  **************/

        /*************************************************/
        /**  First we find the 3D position of the click  */
        /*************************************************/
        vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
        if (! rwi->GetShiftKey() )
            return;
        int X, Y;
        X = rwi->GetEventPosition()[0];
        Y = rwi->GetEventPosition()[1];
        vtkRenderer* renderer = rwi->FindPokedRenderer(X,Y);
        if (!renderer)
            return;
        vtkCellPicker* pointpicker = vtkCellPicker::New();
        pointpicker->Pick(X,Y,0.0, renderer);
        vtkDataSet* dataset = pointpicker->GetDataSet();
        if (!dataset)
        {
            pointpicker->Delete();
            return;
        }
        vtkPoints* points = pointpicker->GetPickedPositions(); 
        if (!points || !points->GetNumberOfPoints())
        {
            pointpicker->Delete();
            return;
        }
        double* position = points->GetPoint ((vtkIdType)0);

        /*************************************************/
        /**  Third we add the landmark and invoke event  */
        /*************************************************/
        vtkPointHandleRepresentation2D * handleRep = vtkPointHandleRepresentation2D::New();
        handleRep->SetWorldPosition(position);
        vtkImageView2D * view = nullptr;
        if (this->Controller->GetInteractorCollection()->GetItemAsObject(0)==rwi )
        {
            view = static_cast<medVtkViewBackend*>(Controller->GetViewFixed()->backend())->view2D;
            manualRegistrationLandmark* landmark = manualRegistrationLandmark::New();
            landmark->SetViewRepRwi(view,handleRep,rwi);
            this->Controller->AddPoint(landmark,0);
        }
        else if (this->Controller->GetInteractorCollection()->GetItemAsObject(1)==rwi )
        {
            view = static_cast<medVtkViewBackend*>(Controller->GetViewMoving()->backend())->view2D;   
            manualRegistrationLandmark* landmark = manualRegistrationLandmark::New();
            landmark->SetViewRepRwi(view,handleRep,rwi);
            this->Controller->AddPoint(landmark,1);
        }
    }
    else if (event == vtkCommand::DeleteEvent)
    {
        manualRegistrationLandmark* landmark = manualRegistrationLandmark::SafeDownCast (caller);
        this->Controller->RequestDeletion(landmark);
        this->Controller->GetViewFixed()->viewWidget()->update();
        this->Controller->GetViewMoving()->viewWidget()->update();
    }
}

//----------------------------------------------------------------------------
manualRegistrationLandmarkController::manualRegistrationLandmarkController()
{
    this->InteractorCollection = nullptr;
    this->Command = manualRegistrationLandmarkControllerCommand::New();
    this->Command->SetController(this);
    this->Enabled = 0;
    Points_Moving = new QList<manualRegistrationLandmark*>();
    Points_Fixed = new QList<manualRegistrationLandmark*>();
    Output = nullptr;

    for(int i = 0;i<128;i++)
    {
        Colors.push_back(indexColors[i]);
    }
}

//----------------------------------------------------------------------------
manualRegistrationLandmarkController::~manualRegistrationLandmarkController()
{
    this->Reset();
    delete Points_Moving;
    delete Points_Fixed;

    this->Command->Delete();

    if (this->InteractorCollection)
    {
        this->InteractorCollection->UnRegister(this);
    }
}

void manualRegistrationLandmarkController::SetInteractorCollection(vtkCollection* arg)
{
    if (arg != this->InteractorCollection)
    {
        if (this->InteractorCollection)
        {
            this->InteractorCollection->UnRegister(this);
        }
        this->InteractorCollection = arg;

        if (this->InteractorCollection)
        {
            this->InteractorCollection->Register(this);
        }
        this->SetEnabled(this->GetEnabled());
    }
}

void manualRegistrationLandmarkController::SetViewMoving(medAbstractLayeredView* view)
{
    ViewMoving = view;
}

void manualRegistrationLandmarkController::SetViewFixed(medAbstractLayeredView* view)
{
    ViewFixed = view;
}

void manualRegistrationLandmarkController::SetViewFuse(medAbstractLayeredView* view)
{
    ViewFuse = view;
}

void manualRegistrationLandmarkController::SetTbx(manualRegistrationToolBox* tbx)
{
    Tbx = tbx;
}

void manualRegistrationLandmarkController::AddPoint(manualRegistrationLandmark* landmark, unsigned int i)
{
    ClearUselessLandmarks();

    int size;
    QColor colorPoint;
    qreal r,g,b,a;
    colorPoint = Colors.takeFirst();
    Colors.push_back(colorPoint);
    colorPoint.getRgbF(&r,&g,&b,&a);
    if (i) 
    {
        if (Points_Moving->size()<Points_Fixed->size())
        {
            Points_Fixed->at(Points_Moving->size())->GetHandleRepresentation()->GetProperty()->GetColor(r,g,b);
        }
        
        size = Points_Moving->size();
    }
    else
    {
        if (Points_Fixed->size()<Points_Moving->size())
        {
            Points_Moving->at(Points_Fixed->size())->GetHandleRepresentation()->GetProperty()->GetColor(r,g,b);
        }

        size = Points_Fixed->size();
    }

    if (size >= 128)
    {
        medMessageController::instance().showError("You cannot add any more landmarks.", 3000);
        return;
    }

    vtkProperty2D * property2D = static_cast<vtkPointHandleRepresentation2D*>(landmark->GetHandleWidget()->GetRepresentation())->GetProperty();
    property2D->SetColor(r,g,b);
    property2D->SetPointSize(0.0);
    property2D->SetLineWidth(3.0);
    property2D->SetOpacity(1.0);

    if (i)
    {
        Points_Moving->append(landmark);
    }
    else
    {
        Points_Fixed->append(landmark);
    }

    if (!landmark->HasObserver (vtkCommand::DeleteEvent, this->Command))
    {
        landmark->AddObserver(vtkCommand::DeleteEvent, this->Command, 0);
    }

    Tbx->updateGUI(Points_Fixed->size(), Points_Moving->size());
}

void manualRegistrationLandmarkController::SetEnabled(unsigned int arg)
{
    this->Enabled = arg;

    if (this->InteractorCollection)
    {
        this->InteractorCollection->InitTraversal();
        vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(this->InteractorCollection->GetNextItemAsObject());
        while(interactor)
        {
            if (arg)
            {
                if (!interactor->HasObserver(vtkCommand::LeftButtonPressEvent, this->Command))
                {
                    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, this->Command, 0);
                }
            }
            else
            {
                interactor->RemoveObserver(this->Command);
            }
            interactor = vtkRenderWindowInteractor::SafeDownCast(this->InteractorCollection->GetNextItemAsObject());
        }
    }
}

int manualRegistrationLandmarkController::checkLandmarks()
{
    if (Points_Fixed->size() != Points_Moving->size())
    {
        medMessageController::instance().showError("The number of landmarks is not the same on both views", 3000);
        return medAbstractProcessLegacy::FAILURE;
    }

    if (!Points_Fixed->size())
    {
        medMessageController::instance().showError("You didn't put any landmark!", 3000);
        return medAbstractProcessLegacy::FAILURE;
    }

    return medAbstractProcessLegacy::SUCCESS;
}

QList<manualRegistrationLandmark*> * manualRegistrationLandmarkController::getPoints_Moving()
{
    return Points_Moving;
}

QList<manualRegistrationLandmark*> * manualRegistrationLandmarkController::getPoints_Fixed()
{
    return Points_Fixed;
}

void manualRegistrationLandmarkController::Reset()
{
    if(ViewFixed)
    {
        for(int i=0; i<Points_Fixed->size(); i++)
        {
            RequestDeletion(Points_Fixed->at(i));
        }
    }

    if(ViewMoving)
    {
        for(int i=0; i<Points_Moving->size(); i++)
        {
            RequestDeletion(Points_Moving->at(i));
        }
    }

    Points_Fixed->clear();
    Points_Moving->clear();
}

void manualRegistrationLandmarkController::ClearUselessLandmarks()
{
    int sizeMoving = Points_Moving->size();
    int sizeFixed = Points_Fixed->size();
    int size = sizeMoving;
    if (size < sizeFixed)
    {
        size =  sizeFixed;
    }

    for(int i=0; i<size; i++)
    {
        if(i < sizeFixed && Points_Fixed->at(i)->GetToDelete())
        {
            manualRegistrationLandmark* landmark = Points_Fixed->takeAt(i);
            landmark->RemoveAllObservers();
            landmark->Delete();
            sizeFixed = Points_Fixed->size();
        }
        if(i < sizeMoving && Points_Moving->at(i)->GetToDelete())
        {
            manualRegistrationLandmark* landmark = Points_Moving->takeAt(i);
            landmark->RemoveAllObservers();
            landmark->Delete();
            sizeMoving = Points_Moving->size();
        }
        size = sizeMoving;

        if (size < sizeFixed)
        {
            size = sizeFixed;
        }
    }
}

void manualRegistrationLandmarkController::RequestDeletion(manualRegistrationLandmark* landmark)
{
    if (InteractorCollection->GetItemAsObject(0) == landmark->GetHandleWidget()->GetInteractor())
    {
        int i = Points_Fixed->indexOf(landmark);
        landmark->SetToDelete(true);
        if (Points_Moving->size() > i)
        {
            Points_Moving->at(i)->SetToDelete(true);
        }
    }
    else if (InteractorCollection->GetItemAsObject(1) == landmark->GetHandleWidget()->GetInteractor())
    {
        int i = Points_Moving->indexOf(landmark);
        landmark->SetToDelete(true);
        if (Points_Fixed->size()>i)
        {
            Points_Fixed->at(i)->SetToDelete(true);
        }
    }

    int cptFixed = 0;
    int cptMoving = 0;

    int size = Points_Moving->size();
    if (Points_Fixed->size()>size)
    {
        size = Points_Fixed->size();
    }

    for (int i=0; i<size; i++)
    {
        if (Points_Fixed->size() > i && !Points_Fixed->at(i)->GetToDelete())
        {
            cptFixed++;
        }
        if (Points_Moving->size() > i && !Points_Moving->at(i)->GetToDelete())
        {
            cptMoving++;
        }
    }

    // Update the user label with the number of current landmarks
    Tbx->updateGUI(cptFixed,cptMoving);
}
