/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiToolBox.h"

#include <itkBinaryContourImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkSliceBySliceImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractProcessLegacy.h>
#include <medAbstractRoi.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medRoiManagementToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>

#include <vtkCellArray.h>
#include <vtkContourRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkMatrix4x4.h>
#include <vtkParametricSpline.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>


const char *polygonRoiToolBox::generateBinaryImageButtonName = "generateBinaryImageButton";

class contourWidgetObserver : public vtkCommand
{
public:
    typedef QPair<unsigned int,unsigned int> PlaneIndexSlicePair;

    static contourWidgetObserver* New()
    {
        return new contourWidgetObserver;
    }

    void Execute ( vtkObject *caller, unsigned long event, void *callData );

    void setView ( vtkImageView2D *view )
    {
        this->view = view;
    }

    void setCurrentPolygonRoi ( polygonRoi * roi)
    {
        this->currentPolygonRoi = roi;
    }

    polygonRoi * getCurrentPolygonRoi ()
    {
        return this->currentPolygonRoi;
    }

    polygonRoi * getRoiToBeClosed ()
    {
        return this->roiToBeClosed;
    }

    void setToolBox ( polygonRoiToolBox * toolBox )
    {
        this->toolBox = toolBox;
    }

    inline void lock()
    {
        this->m_lock = 1;
    }
    inline void unlock()
    {
        this->m_lock = 0;
    }

protected:
    contourWidgetObserver();
    ~contourWidgetObserver();

private:
    int m_lock;
    vtkImageView2D *view;
    polygonRoiToolBox *toolBox;
    polygonRoi *roiToBeClosed;
    polygonRoi *currentPolygonRoi;
};

contourWidgetObserver::contourWidgetObserver()
{
    this->m_lock = 0;
    this->view = nullptr;
    this->toolBox = nullptr;
    this->roiToBeClosed = nullptr;
    this->currentPolygonRoi = nullptr;
}

contourWidgetObserver::~contourWidgetObserver(){}

void contourWidgetObserver::Execute ( vtkObject *caller, unsigned long event, void *callData )
{
    if ( this->m_lock )
    {
        return;
    }

    if (!this->view || !toolBox)
    {
        return;
    }

    if (event == vtkCommand::StartInteractionEvent)
    {
        if (currentPolygonRoi)
        {
            if(!toolBox->addNewCurve->isChecked())
            {
                currentPolygonRoi->forceInvisibilityOn(); // should be a delete but does not work yet !! TODO : solve this !
                currentPolygonRoi = nullptr;
                return;
            }

            if (!toolBox->viewsPlaneIndex.contains(toolBox->currentView))
            {
                QList<int> * planeIndexes = new QList<int>();
                for(int i=0; i<3; i++)
                {
                    planeIndexes->append(0);
                } // fill the list with 0;
                toolBox->viewsPlaneIndex.insert(toolBox->currentView,planeIndexes);
            }

            currentPolygonRoi->setIdSlice(view->GetSlice());
            currentPolygonRoi->setOrientation(view->GetViewOrientation());
            currentPolygonRoi->forceVisibilityOff();

            roiToBeClosed = currentPolygonRoi;
            currentPolygonRoi = nullptr;
        }
    }

    // ROI added in medRoiManagementToolBox even if not closed, or just a point
    QList<int> * planeIndexes = toolBox->viewsPlaneIndex.value(toolBox->currentView);
    AddRoiCommand * command = new AddRoiCommand(toolBox->currentView, roiToBeClosed, "Polygon rois", "NewRoi");
    medRoiManager::instance()->addToUndoRedoStack(command);
    planeIndexes->replace(view->GetViewOrientation(), toolBox->computePlaneIndex()); // save PlaneIndex for this view and orientation TODO : improve this so that we do it only once for each orientation

    roiToBeClosed->getContour()->RemoveObserver(this);
    toolBox->onAddNewCurve();
    roiToBeClosed = nullptr;

    // after first polygonROI created, interpolation and save are enabled
    toolBox->buttonsStateWhenROI();
}

polygonRoiToolBox::polygonRoiToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QVBoxLayout * layout = new QVBoxLayout();
    displayWidget->setLayout(layout);

    addNewCurve = new QPushButton(tr("Closed Polygon"));
    addNewCurve->setToolTip(tr("Activate closed polygon mode"));
    addNewCurve->setCheckable(true);
    addNewCurve->setObjectName("closedPolygonButton");
    connect(addNewCurve,SIGNAL(toggled(bool)),this,SLOT(onAddNewCurve()));

    interpolate = new QPushButton("Interpolate",this);
    interpolate->setToolTip("Interpolate between green master ROIs");
    interpolate->setObjectName("interpolateButton");
    connect(interpolate,SIGNAL(clicked()),this,SLOT(interpolateCurve()));

    extractRoiButton = new QPushButton("Extract ROI",this);
    extractRoiButton->setToolTip("Extract polygon ROI from a current mask in the view.\nBeware, do not use it with a non-binary dataset");
    extractRoiButton->setObjectName("extractRoiButton");
    connect(extractRoiButton,SIGNAL(clicked()),this,SLOT(extractROI()));

    repulsorTool = new QPushButton(tr("Repulsor"));
    repulsorTool->setToolTip(tr("Activate repulsor"));
    repulsorTool->setObjectName("repulsorTool");
    repulsorTool->setCheckable(true);
    connect(repulsorTool,SIGNAL(toggled(bool)),this,SLOT(activateRepulsor()));

    generateBinaryImage_button = new QPushButton(tr("Save Mask"));
    generateBinaryImage_button->setToolTip("Import the current mask to the non persistent database");
    generateBinaryImage_button->setObjectName(generateBinaryImageButtonName);
    connect(generateBinaryImage_button,SIGNAL(clicked()),this,SLOT(generateAndSaveBinaryImage()));

    currentView = nullptr;

    QHBoxLayout * ButtonLayout0 = new QHBoxLayout();
    layout->addLayout( ButtonLayout0 );
    ButtonLayout0->addWidget(addNewCurve);
    ButtonLayout0->addWidget(interpolate);
    ButtonLayout0->addWidget(extractRoiButton);

    QHBoxLayout * ButtonLayout1 = new QHBoxLayout();
    layout->addLayout( ButtonLayout1 );
    ButtonLayout1->addWidget(repulsorTool);
    ButtonLayout1->addWidget(generateBinaryImage_button);

    hashViewObserver = new QHash<medAbstractView*,contourWidgetObserver*>();

    // How to use
    QLabel* explanation = new QLabel(tr("Define a ROI: choose 'Closed Polygon' and click on the data set.\n")
                                     + tr("Remove a landmark: put the cursor on it and SUPPR.\n")
                                     + tr("Undo action: CTRL/CMD + z.\n")
                                     + tr("Redo action: CTRL/CMD + y.\n")
                                     + tr("Copy ROIs in current slice: CTRL/CMD + c.\n")
                                     + tr("Paste ROIs: CTRL/CMD + v.\n")
                                     );
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    //shortcuts
    undo_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+z", "Undo polygon manipulation")),this);
    redo_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+y", "Redo polygon manipulation")),this);
    copy_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+c", "Copy polygons in current slice")),this);
    paste_shortcut = new QShortcut(QKeySequence(tr("Ctrl+v", "Paste polygons")),this);

    //Shortcuts
    connect(undo_shortcut,  SIGNAL(activated()), this, SLOT(undo()));
    connect(redo_shortcut,  SIGNAL(activated()), this, SLOT(redo()));
    connect(copy_shortcut,  SIGNAL(activated()), this, SLOT(copyContours()));
    connect(paste_shortcut, SIGNAL(activated()), this, SLOT(pasteContours()));

    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();

    // buttons initialisation: view has no data
    disableButtons();

    // Add Roi Management Toolbox
    roiManagementToolBox = medToolBoxFactory::instance()->createToolBox("medRoiManagementToolBox");
    roiManagementToolBox->header()->hide();
    this->addWidget(roiManagementToolBox);

    // initialise buttons if every ROI have been deleted through medRoiManager
    connect(medRoiManager::instance(), SIGNAL(allRoisDeleted()), this, SLOT(buttonsStateAtDataOpeningBeforeROI()));
}

polygonRoiToolBox::~polygonRoiToolBox()
{
    delete hashViewObserver;
}

bool polygonRoiToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<polygonRoiToolBox>();
}

dtkPlugin* polygonRoiToolBox::plugin()
{
    medPluginManager *pm = medPluginManager::instance();
    dtkPlugin *plugin = pm->plugin ( "Polygon ROI" );
    return plugin;
}

medAbstractData *polygonRoiToolBox::processOutput()
{
    if (!m_maskData)
    {
        generateBinaryImage();
    }
    return m_maskData; // return output data
}

void polygonRoiToolBox::updateView()
{
    disableButtons();

    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);

    if (view)
    {
        // polygonRoiToolBox do not use meshes
        for (unsigned int i=0 ; i<v->layersCount() ; ++i)
        {
            medAbstractData *data = v->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh"))
            {
                handleDisplayError(medAbstractProcessLegacy::NO_MESH);
                return;
            }
        }

        // if data are not meshes
        buttonsStateAtDataOpeningBeforeROI();

        if (currentView != v)
        {
            bool repulsorOn = repulsorTool->isChecked();
            if (repulsorOn)
            {
                repulsorTool->setChecked(false);
            }
            currentView = v;

            // Handle new events with Roi management
            roiManagementToolBox->setWorkspace(getWorkspace());
            medRoiManager::instance()->setCurrentView(currentView);
            roiManagementToolBox->updateView();

            if(interactorStyleRepulsor != nullptr)
            {
                interactorStyleRepulsor->SetCurrentView(currentView);
            }
            if (repulsorOn)
            {
                repulsorTool->setChecked(true);
            }
            if (!hashViewObserver->contains(currentView))
            {
                contourWidgetObserver * observer = contourWidgetObserver::New();
                observer->setView(static_cast<medVtkViewBackend*>(currentView->backend())->view2D);
                observer->setToolBox(this);
                hashViewObserver->insert(currentView,observer);
            }

            connect(currentView, SIGNAL(closed()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
            connect(currentView, SIGNAL(layerRemoved(unsigned int)), this, SLOT(onLayerClosed()), Qt::UniqueConnection);
        }
    }
}

void polygonRoiToolBox::onViewClosed()
{
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
    hashViewObserver->remove(viewClosed);

    if (viewClosed == currentView)
    {
        currentView = nullptr;
    }
    m_maskData = nullptr;

    disableButtons();

    // If you close the last layer of a view through Layer Settings,
    // here, the view is not removed yet.
    roiManagementToolBox->clear();

    interactorStyleRepulsor = nullptr;
}

void polygonRoiToolBox::onLayerClosed()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);

    // We enter here only if onLayerClosed has not been called during a view removal
    if (v && (v->layersCount() > 0))
    {
        currentView = v;
        this->clear();
        buttonsStateAtDataOpeningBeforeROI();
    }
}

void polygonRoiToolBox::onAddNewCurve()
{
    if (currentView)
    {
        if (!hashViewObserver->contains(currentView) || !hashViewObserver->value(currentView)->getCurrentPolygonRoi())
        {
            if (repulsorTool->isChecked() && addNewCurve->isChecked())
            {
                repulsorTool->setChecked(false);
            }

            vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

            polygonRoi *roi = new polygonRoi(view2d);

            vtkContourWidget *currentContour = roi->getContour();

            contourWidgetObserver *observer = hashViewObserver->value(currentView);
            observer->setCurrentPolygonRoi(roi);
            currentContour->AddObserver(vtkCommand::StartInteractionEvent,observer); // todo put this observer in polygonRoi manage the adding to the roimanagementtoolbox through it and
            currentContour->AddObserver(vtkCommand::EndInteractionEvent,observer);

            roi->forceVisibilityOn();
        }
    }
}

void polygonRoiToolBox::activateRepulsor()
{
    if (currentView)
    {
        if (repulsorTool->isChecked() && addNewCurve->isChecked())
        {
            addNewCurve->setChecked(false);
        }

        if (interactorStyleRepulsor == nullptr)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
            interactorStyleRepulsor->SetCurrentView(currentView);
        }

        vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
        if (repulsorTool->isChecked())
        {
            vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::SafeDownCast(view2D->GetInteractor()->GetInteractorStyle());
            interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
            view2D->SetInteractorStyle(interactorStyleRepulsor);
            view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline
        }
        else
        {
            vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::New();
            interactorStyle2D->SetLeftButtonInteraction(interactorStyleRepulsor->GetLeftButtonInteraction());
            view2D->SetInteractorStyle(interactorStyle2D);
            view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline
            interactorStyle2D->Delete();
        }
    }
}

QList<medSeriesOfRoi*> * polygonRoiToolBox::getListOfView(medAbstractView *view)
{
    return medRoiManager::instance()->getSeriesOfRoi()->value(view);
}

void polygonRoiToolBox::copyContours()
{
    medRoiManager * manager = medRoiManager::instance();
    manager->setCurrentView(currentView);
    manager->copy();
}
void polygonRoiToolBox::pasteContours()
{
    medRoiManager * manager = medRoiManager::instance();
    manager->setCurrentView(currentView);
    manager->paste();
}

void polygonRoiToolBox::undo()
{
    if (!currentView)
    {
        return;
    }
    medRoiManager::instance()->setCurrentView(currentView);
    medRoiManager::instance()->undo();
    currentView->render();
}

void polygonRoiToolBox::redo()
{
    if (!currentView)
    {
        return;
    }
    medRoiManager::instance()->setCurrentView(currentView);
    medRoiManager::instance()->redo();
    currentView->render();
}

void polygonRoiToolBox::reorderPolygon(vtkPolyData *poly)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

    double displayPoint[3];
    double *worldPoint;
    double xmin = VTK_DOUBLE_MAX;
    int xminIndex = 0;
    double ymin = VTK_DOUBLE_MAX;
    int yminIndex = 0;

    for(int i=0; i<poly->GetNumberOfPoints(); i++)
    {
        worldPoint = poly->GetPoint(i);
        vtkInteractorObserver::ComputeWorldToDisplay(view2d->GetRenderer(), worldPoint[0], worldPoint[1], worldPoint[2], displayPoint);

        if (displayPoint[0] < xmin)
        {
            xmin =  displayPoint[0];
            xminIndex= i;
        }

        if (displayPoint[1] < ymin)
        {
            ymin =  displayPoint[1];
            yminIndex= i;
        }
    }

    int dist = xminIndex-yminIndex;
    bool reverse =((abs(dist)>poly->GetNumberOfPoints()/2) != (dist<0));
    vtkSmartPointer<vtkPoints> reorderedPoints = vtkSmartPointer<vtkPoints>::New();
    vtkPoints *points = poly->GetPoints();
    if (reverse)
    {
        for(int i=0; i<poly->GetNumberOfPoints(); i++)
        {
            double p[3];
            points->GetPoint(xminIndex,p);
            xminIndex--;
            if (xminIndex < 0)
            {
                xminIndex = poly->GetNumberOfPoints()-1;
            }
            reorderedPoints->InsertNextPoint(p);
        }
    }
    else
    {
        for(int i=0; i<poly->GetNumberOfPoints(); i++)
        {
            double p[3];
            points->GetPoint(xminIndex, p);
            xminIndex = (xminIndex+1) % poly->GetNumberOfPoints();
            reorderedPoints->InsertNextPoint(p);
        }
    }
    poly->SetPoints(reorderedPoints);
}

void polygonRoiToolBox::resampleCurve(vtkPolyData *poly,int nbPoints)
{
    vtkSmartPointer<vtkParametricSpline> spline =vtkSmartPointer<vtkParametricSpline>::New();
    poly->GetPoints()->InsertNextPoint(poly->GetPoints()->GetPoint(0));
    spline->SetPoints(poly->GetPoints());

    vtkPoints *points = vtkPoints::New();
    double p[3];
    double u[3];
    for(int j=0; j<nbPoints+1; j++)
    {
        u[0] = u[1] = u[2] = j/(double)(nbPoints+1);
        spline->Evaluate(u, p, nullptr);
        points->InsertNextPoint(p);
    }

    poly->SetPoints(points);
}

QList<vtkPolyData* > polygonRoiToolBox::generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb)
{
    int max = curve2->GetNumberOfPoints();

    vtkSmartPointer<vtkPolyData> startCurve = curve1, endCurve = curve2;
    bool curve2ToCurve1 = false;

    if (curve1->GetNumberOfPoints()>=curve2->GetNumberOfPoints())
    {
        max = curve1->GetNumberOfPoints();
        startCurve = curve2;
        endCurve = curve1;
        curve2ToCurve1 = true;
    }

    //Homogenise the points distribution on the curve
    resampleCurve(startCurve, max);
    resampleCurve(endCurve, max);

    reorderPolygon(startCurve);
    reorderPolygon(endCurve);

    vtkSmartPointer<vtkPoints> bufferPoints = vtkPoints::New();
    QList<vtkPolyData*> list;
    for(int i=1; i<=nb; i++)
    {
        vtkPolyData *poly = vtkPolyData::New();
        vtkCellArray *cells = vtkCellArray::New();
        vtkPolyLine *polyLine = vtkPolyLine::New();

        polyLine->GetPointIds()->SetNumberOfIds(startCurve->GetNumberOfPoints());

        bufferPoints->Reset();
        for(int k=0; k<startCurve->GetNumberOfPoints(); k++)
        {
            double p1[3],p2[3],p3[3];
            startCurve->GetPoint(k, p1);
            endCurve->GetPoint(k, p2);
            if (curve2ToCurve1)
            {
                p3[0]= p1[0] +(p2[0]-p1[0])*((i)/(double)(nb+1));
                p3[1]= p1[1] +(p2[1]-p1[1])*((i)/(double)(nb+1));
                p3[2]= p1[2] +(p2[2]-p1[2])*((i)/(double)(nb+1));
            }
            else
            {
                p3[0]= p2[0] +(p1[0]-p2[0])*((i)/(double)(nb+1));
                p3[1]= p2[1] +(p1[1]-p2[1])*((i)/(double)(nb+1));
                p3[2]= p2[2] +(p1[2]-p2[2])*((i)/(double)(nb+1));
            }
            bufferPoints->InsertNextPoint(p3);
            polyLine->GetPointIds()->SetId(k, k);
        }
        cells->InsertNextCell(polyLine);

        vtkPoints * polyPoints = vtkPoints::New();
        polyPoints->DeepCopy(bufferPoints);
        poly->SetPoints(polyPoints);
        poly->SetLines(cells);
        list.append(poly);
    }
    return list;
}

polygonRoiToolBox::ListRois polygonRoiToolBox::interpolateBetween2Polygon(const QList<polygonRoi*> &rois, int firstIndex)
{
    ListRois outputRois;

    polygonRoi * polyRoi;
    vtkContourRepresentation* contour;

    // Contour first ROI
    polyRoi = rois.at(firstIndex);
    contour = polyRoi->getContour()->GetContourRepresentation();
    vtkSmartPointer<vtkPolyData> curveMin = contour->GetContourRepresentationAsPolyData();
    int curveMinNbNode = contour->GetNumberOfNodes();
    int minSlice = rois.at(firstIndex)->getIdSlice();

    // Contour second ROI
    polyRoi = rois.at(firstIndex+1);
    contour = polyRoi->getContour()->GetContourRepresentation();
    vtkSmartPointer<vtkPolyData> curveMax = contour->GetContourRepresentationAsPolyData();
    int curveMaxNbNode = contour->GetNumberOfNodes();
    int maxSlice = rois.at(firstIndex+1)->getIdSlice();

    // Compute intermediate ROIs between two successive ROIs
    QList<vtkPolyData *> listPolyData = generateIntermediateCurves(curveMax,curveMin,maxSlice-minSlice-1);
    double number = ceil(1.5 * (double)(curveMinNbNode));
    if (curveMaxNbNode > curveMinNbNode)
    {
        number = ceil(1.5 * (double)(curveMaxNbNode));
    }

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

    for (int i = minSlice+1;i<maxSlice;i++)
    {
        polygonRoi *polyRoi = new polygonRoi(view2d);
        vtkContourWidget *contour = polyRoi->getContour();
        contour->SetEnabled(true);

        // Points decimation // TODO put this in generateIntermediateCurves
        int nbPointsPoly = listPolyData[i-(minSlice+1)]->GetNumberOfPoints();
        int divPoly = (int)floor((double)nbPointsPoly/number);
        vtkPoints * points = listPolyData[i-(minSlice+1)]->GetPoints();
        vtkPoints * decimatedPoints = vtkPoints::New();

        for (int k = nbPointsPoly-1; k>=0; k--)
        {
            if (k%divPoly==0)
            {
                decimatedPoints->InsertNextPoint(points->GetPoint(k));
            }
        }
        listPolyData[i-(minSlice+1)]->SetPoints(decimatedPoints);

        contour->Initialize(listPolyData[i-(minSlice+1)]);

        vtkContourRepresentation *contourRep = contour->GetContourRepresentation();

        int nbPoints = contourRep->GetNumberOfNodes();

        contourRep->DeleteNthNode(nbPoints-1); // delete the last node, no need for that normally
        contourRep->SetClosedLoop(1);

        polyRoi->setIdSlice(i);
        polyRoi->setMasterRoi(false);

        outputRois.append(polyRoi);
    }

    return outputRois;
}

void polygonRoiToolBox::interpolateCurve()
{
    if (currentView)
    {
        this->setToolBoxOnWaitStatusForNonRunnableProcess();

        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

        typedef QList<medSeriesOfRoi*> ListOfSeriesOfRois;
        ListRois *list;
        ListOfSeriesOfRois *listSeries = medRoiManager::instance()->getSeriesOfRoi()->value(currentView);

        // Get list of Rois
        list = listSeries->at(0)->getListOfRois();
        if (list && !list->isEmpty())
        {
            int orientation = view2d->GetViewOrientation();

            QList<polygonRoi*> rois;
            QList<polygonRoi*> masterRois;
            QList<unsigned int> masterRoisIndexes;

            for(int i=0; i<list->size(); i++)
            {
                polygonRoi *polyRoi = dynamic_cast<polygonRoi*>(list->at(i)); // TODO : need to test if the cast goes well we cannot be sure that the Rois are polygonRoi
                if(!polyRoi->getContour()->GetContourRepresentation()->GetClosedLoop())
                {
                    displayMessageError("Non-closed polygon at slice: " + QString::number(polyRoi->getIdSlice()+1) + ". Operation aborted");
                    this->setToolBoxOnReadyToUse();
                    return;
                }
                if (polyRoi->getOrientation() == orientation)
                {
                    rois.append(polyRoi);
                    if(polyRoi->isMasterRoi())
                    {
                        masterRois.append(polyRoi);
                        masterRoisIndexes.append(i);//remember master ROIs positions
                    }
                }
            }

            if(masterRois.size() >= 2)
            {
                //Let's remove the undesired ROIs
                ListRois roisToBeRemoved;
                for (unsigned int i=masterRoisIndexes.first(); i<=masterRoisIndexes.last(); i++)
                {
                    if(!rois.at(i)->isMasterRoi()) //master ROIs shouldn't be removed
                    {
                        roisToBeRemoved.append(rois.at(i));
                    }
                }

                DeleteSeveralRoisCommand *deleteCommand = new DeleteSeveralRoisCommand(currentView,roisToBeRemoved,"Polygon rois","deleteSeveralRois");
                medRoiManager::instance()->addToUndoRedoStack(deleteCommand);


                // Compute interpolation between master ROIs
                ListRois interpolationOutput;
                for(int i=0 ; i<masterRois.size()-1 ; i++)
                {
                    ListRois outputRois = interpolateBetween2Polygon(masterRois, i);
                    interpolationOutput.append(outputRois);
                }

                // Send signal of new Rois to medRoiManager
                AddSeveralRoisCommand *command = new AddSeveralRoisCommand(currentView, interpolationOutput, "Polygon rois", "NewRois");
                medRoiManager::instance()->addToUndoRedoStack(command);
            }
            else
            {
                displayMessageError("Interpolate needs at least 2 master ROIs");
            }
        }

        this->setToolBoxOnReadyToUse();
    }
}

void polygonRoiToolBox::generateBinaryImage()
{
    if (currentView)
    {
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

        QList<medSeriesOfRoi*> *listROI = getListOfView(currentView);
        if (!listROI || listROI->isEmpty())
        {
            return; // check if ROI or not
        }

        ListRois *list = listROI->at(0)->getListOfRois();

        QList<QPair<vtkPolyData *,PlaneIndexSlicePair> > listPolyData = QList<QPair<vtkPolyData *,PlaneIndexSlicePair> >();

        int orientation = view2d->GetViewOrientation();

        for(int i=0; i<list->size(); i++)
        {
            polygonRoi *polyRoi = dynamic_cast<polygonRoi*>(list->at(i));  // TODO : need to test if the cast goes well we cannot be sure that the Rois are polygonRoi
            vtkContourWidget *contour =  polyRoi->getContour();
            unsigned int orientationOfRoi = polyRoi->getOrientation();
            unsigned int idSlice = polyRoi->getIdSlice();
            if (viewsPlaneIndex.empty())
            {
                return;
            }
            unsigned char planeIndex = viewsPlaneIndex.value(currentView)->at(orientationOfRoi);
            vtkContourRepresentation *contourRep = contour->GetContourRepresentation();
            view2d->SetViewOrientation(orientationOfRoi); // we set the view Orientation to the orientation of the ROI, to retreive the polyData with world coordinates based on the display from the orientation.
            listPolyData.append(QPair<vtkPolyData*,PlaneIndexSlicePair>(contourRep->GetContourRepresentationAsPolyData(),PlaneIndexSlicePair(idSlice,planeIndex)));
        }
        view2d->SetViewOrientation(orientation);

        QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > listPolygon = createImagePolygons(listPolyData);
        binaryImageFromPolygon(listPolygon);
    }
}

void polygonRoiToolBox::generateAndSaveBinaryImage()
{
    generateBinaryImage();
    medDataManager::instance()->importData(m_maskData, false);
}

QList<QPair<vtkPolygon*,polygonRoiToolBox::PlaneIndexSlicePair> > polygonRoiToolBox::createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > listPolygon = QList<QPair<vtkPolygon*,PlaneIndexSlicePair> >();

    for(int i=0; i<listPoly.size(); i++)
    {
        vtkPolygon *polygon = vtkPolygon::New();
        vtkPoints *points = vtkPoints::New();

        const int nb = listPoly.at(i).first->GetNumberOfPoints();

        vtkIdType ids[1000];
        double imagePreviousPoint[3] ={0,0,0};
        unsigned int nbPoint = 0;

        unsigned int x1=0, y1=0, z1=0;
        switch (listPoly.at(i).second.second)
        {
            case 0 :
            {
                x1=1;
                y1=2;
                z1=0;
                break;
            }
            case 1 :
            {
                x1=0;
                y1=2;
                z1=1;
                break;
            }
            case 2 :
            {
                x1=0;
                y1=1;
                z1=2;
                break;
            }
        }

        for(int j=0; j<nb; j++)
        {
            double *point = listPoly.at(i).first->GetPoint(j);

            int imagePoint[3];
            double imagePointDouble[3];

            view2d->GetImageCoordinatesFromWorldCoordinates(point,imagePoint);

            imagePointDouble[x1]= (double)imagePoint[x1];
            imagePointDouble[y1]= (double)imagePoint[y1];

            imagePointDouble[z1]= (double)listPoly.at(i).second.first;

            if (imagePointDouble[x1] == imagePreviousPoint[x1] && imagePointDouble[y1] == imagePreviousPoint[y1] && imagePointDouble[z1] == imagePreviousPoint[z1])
            {
                continue;
            }

            points->InsertNextPoint(imagePointDouble);
            ids[nbPoint]=nbPoint;
            nbPoint++;

            imagePreviousPoint[x1] = imagePointDouble[x1];
            imagePreviousPoint[y1] = imagePointDouble[y1];
            imagePreviousPoint[z1] = imagePointDouble[z1];
        }

        polygon->Initialize(points->GetNumberOfPoints(), ids, points);

        listPolygon.append(QPair<vtkPolygon*,PlaneIndexSlicePair>(polygon,listPoly.at(i).second));
    }

    return listPolygon;
}

void polygonRoiToolBox::binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys)
{
    m_maskData = medAbstractDataFactory::instance()->create( "itkDataImageUChar3" );
    medAbstractImageData *inputData = qobject_cast<medAbstractImageData*>(currentView->layerData(currentView->currentLayer()));// TODO : currentlayer looks dangerous
    if (!inputData)
    {
        qDebug()<<"polygonRoiToolBox::binaryImageFromPolygon data error.";
        return;
    }
    initializeMaskData(inputData,m_maskData);
    UChar3ImageType::Pointer m_itkMask = dynamic_cast<UChar3ImageType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );

    for(int k=0; k<polys.size(); k++)
    {
        double bounds[6];
        polys[k].first->GetBounds(bounds);
        unsigned int x=0, y=0, z=0;
        unsigned int bx=0, by=0;
        unsigned int dimX=0, dimY=0;
        switch (polys[k].second.second)
        {
            case 0 :
            {
                dimX = inputData->yDimension();
                dimY = inputData->zDimension();
                x = 1;
                y = 2;
                z = 0;
                bx = 2;
                by = 4;
                break;
            }
            case 1 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->zDimension();
                x = 0;
                y = 2;
                z = 1;
                bx = 0;
                by = 4;
                break;
            }
            case 2 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->yDimension();
                x = 0;
                y = 1;
                z = 2;
                bx = 0;
                by = 2;
                break;
            }
        }

        // Qt rasterization
        int nbPoints = polys[k].first->GetPoints()->GetNumberOfPoints();
        vtkPoints *pointsArray = polys[k].first->GetPoints();
        QPolygonF polygon;
        for(int i=0; i<nbPoints; i++)
        {
            polygon << QPointF(pointsArray->GetPoint(i)[x], pointsArray->GetPoint(i)[y]);
        }

        QPainterPath path;
        path.addPolygon(polygon);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        QImage imagePolygon(dimX,dimY,QImage::Format_RGB16);
        imagePolygon.fill(Qt::black);
        QPainter painter;
        painter.begin(&imagePolygon);
        painter.fillPath(path,brush);

        for(int i=bounds[bx]; i<=bounds[bx+1]; i++)
        {
            for(int j=bounds[by]; j<=bounds[by+1]; j++)
            {
                QRgb val = imagePolygon.pixel(i,j);

                if (val==qRgb(255,255,255))
                {
                    UChar3ImageType::IndexType index;
                    index[x]=i;
                    index[y]=j;
                    index[z]=polys[k].second.first;
                    m_itkMask->SetPixel(index,1);
                }
            }
        }
    }

    medUtilities::setDerivedMetaData(m_maskData, inputData, "polygon roi");
}

void polygonRoiToolBox::initializeMaskData( medAbstractData *imageData, medAbstractData *maskData )
{
    UChar3ImageType::Pointer mask = UChar3ImageType::New();

    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData *mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);

    UChar3ImageType::RegionType region;
    region.SetSize(0, ( mImage->Dimension() > 0 ? mImage->xDimension() : 1 ) );
    region.SetSize(1, ( mImage->Dimension() > 1 ? mImage->yDimension() : 1 ) );
    region.SetSize(2, ( mImage->Dimension() > 2 ? mImage->zDimension() : 1 ) );

    UChar3ImageType::DirectionType direction;
    UChar3ImageType::SpacingType spacing;
    UChar3ImageType::PointType origin;

    direction.Fill(0);
    spacing.Fill(0);
    origin.Fill(0);
    for (unsigned int i = 0; i < mask->GetImageDimension(); ++i)
    {
        direction(i,i) = 1;
    }

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->Dimension());

    switch (mImage->Dimension())
    {
        case 2:
        {
            itk::ImageBase <2> * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 4:
        {
            itk::ImageBase <4> * imageDataOb = dynamic_cast<itk::ImageBase <4> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 3:
        default:
        {
            itk::ImageBase <3> *imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }
    }

    mask->SetOrigin(origin);
    mask->SetDirection(direction);
    mask->SetSpacing(spacing);
    mask->SetLargestPossibleRegion(region);
    mask->SetBufferedRegion(region);
    mask->Allocate();
    mask->FillBuffer(0);

    maskData->setData((QObject*)(mask.GetPointer()));
}

int polygonRoiToolBox::computePlaneIndex()
{
    typedef  UChar3ImageType::DirectionType::InternalMatrixType::element_type ElemType;
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    int planeIndex = -1;

    if (currentView->is2D())
    {
        const QVector3D vpn = currentView->viewPlaneNormal();

        vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );
        vtkMatrix4x4 *direction = view2d->GetOrientationMatrix();
        double absDotProductMax = 0;
        planeIndex = 0;

        for (unsigned int i = 0; i < 3; ++i)
        {
            double dotProduct = 0;
            for (unsigned int j = 0; j < 3; ++j)
            {
                dotProduct += direction->GetElement(j,i) * vecVpn[j];
            }

            if (fabs(dotProduct) > absDotProductMax)
            {
                planeIndex = i;
                absDotProductMax = fabs(dotProduct);
            }
        }
    }
    return planeIndex;
}

void polygonRoiToolBox::disableButtons()
{
    addNewCurve->setEnabled(false);
    addNewCurve->setChecked(false);
    repulsorTool->setEnabled(false);
    repulsorTool->setChecked(false);
    generateBinaryImage_button->setEnabled(false);
    interpolate->setEnabled(false);
    extractRoiButton->setEnabled(false);
}

void polygonRoiToolBox::clear()
{
    medToolBox::clear();

    if(currentView)
    {
        // We need to update the medSliderL
        roiManagementToolBox->updateView();

        ListRois *list;
        QList<medSeriesOfRoi*> *listSeries = medRoiManager::instance()->getSeriesOfRoi()->value(currentView);

        if(listSeries != nullptr && listSeries->at(0)->getListOfRois()->size() != 0)
        {
            // Get list of Rois
            list = listSeries->at(0)->getListOfRois();
            DeleteSeveralRoisCommand * deleteCommand = new DeleteSeveralRoisCommand(currentView,*list,"Polygon rois","deleteSeveralRois");
            medRoiManager::instance()->addToUndoRedoStack(deleteCommand);

            m_maskData = nullptr;
            disableButtons();
        }
    }
}

void polygonRoiToolBox::buttonsStateAtDataOpeningBeforeROI()
{
    // At opening of a data, before first ROI
    disableButtons();
    addNewCurve->setEnabled(true);
    extractRoiButton->setEnabled(true);
}
void polygonRoiToolBox::buttonsStateWhenROI()
{
    addNewCurve->setEnabled(true);
    extractRoiButton->setEnabled(true);
    repulsorTool->setEnabled(true);
    interpolate->setEnabled(true);
    generateBinaryImage_button->setEnabled(true);
}

template <class ImageType> UChar3ImageType::Pointer cast(medAbstractData *input)
{
    typedef itk::CastImageFilter< ImageType, UChar3ImageType > CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(dynamic_cast< ImageType*>((itk::Object*)(input->data())));
    caster->Update();
    return caster->GetOutput();
}

UChar3ImageType::Pointer extract2dContour(UChar3ImageType::Pointer input)
{
    typedef itk::Image <unsigned char, 2> Mask2dType;

    typedef itk::MinimumMaximumImageCalculator <UChar3ImageType> ImageCalculatorFilterType;
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
            = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(input);
    imageCalculatorFilter->Compute();

    // Check if the dataset is a mask
    itk::ImageRegionIterator<UChar3ImageType> it(input, input->GetRequestedRegion() );
    while(!it.IsAtEnd())
    {
        if ((it.Get() != imageCalculatorFilter->GetMinimum())
                && (it.Get() != imageCalculatorFilter->GetMaximum()))
        {
            // Odd values have been found. The dataset is not a mask
            return nullptr;
        }
        ++it;
    }

    typedef itk::BinaryContourImageFilter <Mask2dType, Mask2dType> binaryContourImageFilterType;
    binaryContourImageFilterType::Pointer contourFilter = binaryContourImageFilterType::New ();
    contourFilter->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    contourFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());

    typedef itk::SliceBySliceImageFilter< UChar3ImageType, UChar3ImageType> SbSFilterType;
    SbSFilterType::Pointer sliceBySliceFilter = SbSFilterType::New();
    sliceBySliceFilter->SetFilter( contourFilter );
    sliceBySliceFilter->SetInput( input );
    sliceBySliceFilter->Update();
    return sliceBySliceFilter->GetOutput();
}

UChar3ImageType::Pointer polygonRoiToolBox::castToUChar3(medAbstractImageData *input)
{
    if (!input)
    {
        displayMessageError("No input found!");
        return nullptr;
    }

    QString id = input->identifier();

    UChar3ImageType::Pointer img = UChar3ImageType::New();

    if ( id == "itkDataImageChar3" )
    {
        img = cast<itk::Image <char,3> >(input);
    }
    else if ( id == "itkDataImageUChar3" )
    {
        img = dynamic_cast< UChar3ImageType*>((itk::Object*)(input->data()));
    }
    else if ( id == "itkDataImageShort3" )
    {
        img = cast<itk::Image <short,3> >(input);
    }
    else if ( id == "itkDataImageUShort3" )
    {
        img = cast<itk::Image <unsigned short,3> >(input);
    }
    else if ( id == "itkDataImageInt3" )
    {
        img = cast<itk::Image <int,3> >(input);
    }
    else if ( id == "itkDataImageUInt3" )
    {
        img = cast<itk::Image <unsigned int,3> >(input);
    }
    else if ( id == "itkDataImageLong3" )
    {
        img = cast<itk::Image <long,3> >(input);
    }
    else if ( id == "itkDataImageULong3" )
    {
        img = cast<itk::Image <unsigned long,3> >(input);
    }
    else if ( id == "itkDataImageFloat3" )
    {
        img = cast<itk::Image <float,3> >(input);
    }
    else if ( id == "itkDataImageDouble3" )
    {
        img = cast<itk::Image <double,3> >(input);
    }
    else
    {
        displayMessageError("Pixel type not yet implemented");
        img =  nullptr;
    }

    return img;
}

void polygonRoiToolBox::showExtractROIWarningPopUp()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Extract ROI error");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("An error occured, please check that your current layer is a mask.");
    msgBox.exec();
}

void polygonRoiToolBox::extractROI()
{
    this->setToolBoxOnWaitStatusForNonRunnableProcess();

    medAbstractImageData *input = qobject_cast<medAbstractImageData*>(currentView->layerData(currentView->currentLayer()));

    UChar3ImageType::Pointer img = castToUChar3(input);

    if (img)
    {
        UChar3ImageType::Pointer contourImage = extract2dContour(img);

        if (contourImage)
        {
            UChar3ImageType::SizeType size = contourImage->GetLargestPossibleRegion().GetSize();
            UChar3ImageType::IndexType px;

            vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

            vtkSmartPointer<vtkPoints>  bufferPoints = vtkPoints::New();
            for (unsigned int sl=0 ; sl<size[2] ; sl++) // go through slices
            {
                vtkPolyData *poly = vtkPolyData::New();
                bufferPoints->Reset();
                bool isEmpty = true;

                for (unsigned int y=0 ; y<size[1] ; y++)
                {
                    for (unsigned int x=0 ; x<size[0] ; x++)
                    {
                        px[0] = x;
                        px[1] = y;
                        px[2] = sl;
                        if (contourImage->GetPixel(px) != 0)
                        {
                            addPointToBuffer(view2d, bufferPoints, px);
                            isEmpty = false;
                            break;
                        }
                    }
                    if (!isEmpty)
                        break;
                }
                if (!isEmpty)
                {
                    //contour indices are added into bufferPoints
                    getContourIndices(view2d, contourImage, px, bufferPoints);

                    vtkPoints *polyPoints =vtkPoints::New();
                    polyPoints->DeepCopy(bufferPoints);
                    poly->SetPoints(polyPoints);

                    polygonRoi *polyRoi = new polygonRoi(view2d);
                    vtkContourWidget * contour = polyRoi->getContour();
                    // Points decimation // TODO put this in generateIntermediateCurves
                    int nbPointsPoly = poly->GetNumberOfPoints();
                    vtkPoints *points = poly->GetPoints();
                    vtkPoints *decimatedPoints = vtkPoints::New();

                    for (int k = nbPointsPoly-1; k >= 0; k--)
                    {
                        if (k % HANDLE_PERIOD == 0) //one handle every HANDLE_PERIOD points
                        {
                            decimatedPoints->InsertNextPoint(points->GetPoint(k));
                        }
                    }
                    poly->SetPoints(decimatedPoints);
                    contour->Initialize(poly);

                    vtkContourRepresentation *contourRep = contour->GetContourRepresentation();
                    contourRep->SetClosedLoop(1);

                    if (!viewsPlaneIndex.contains(currentView))
                    {
                        QList<int> *planeIndexes=new QList<int>();
                        for (int i = 0; i < 3; i++)
                        {
                            planeIndexes->append(0); // fill the list with 0;
                        }
                        viewsPlaneIndex.insert(currentView,planeIndexes);
                    }

                    polyRoi->setIdSlice(sl);
                    polyRoi->setOrientation(view2d->GetViewOrientation());
                    polyRoi->setMasterRoi(false);
                    polyRoi->forceVisibilityOff();

                    AddRoiCommand *command = new AddRoiCommand(currentView, polyRoi,"Polygon rois","NewRois");
                    medRoiManager::instance()->addToUndoRedoStack(command);
                    QList<int> *planeIndexes= viewsPlaneIndex.value(currentView);
                    planeIndexes->replace(view2d->GetViewOrientation(),computePlaneIndex());
                    // save PlaneIndex for this view and orientation TODO : improve this so that we do it only once for each orientation

                    buttonsStateWhenROI();
                }
            }
        }
        else
        {
            showExtractROIWarningPopUp();
        }
    }

    this->setToolBoxOnReadyToUse();
}

void polygonRoiToolBox::addPointToBuffer(vtkImageView2D *view2d, vtkSmartPointer<vtkPoints> bufferPoints, itk::ImageBase<3>::IndexType point)
{
    double worldPoint[3];
    int displayPoint[3];
    displayPoint[0] = point[0];
    displayPoint[1] = point[1];
    displayPoint[2] = point[2];
    view2d->GetWorldCoordinatesFromImageCoordinates(displayPoint, worldPoint);
    bufferPoints->InsertNextPoint(worldPoint[0], worldPoint[1], worldPoint[2]);
}

void polygonRoiToolBox::getContourIndices(vtkImageView2D *view2d, UChar3ImageType::Pointer contourImage, UChar3ImageType::IndexType px, vtkSmartPointer<vtkPoints> bufferPoints)
{
    UChar3ImageType::IndexType px2 = px;
    do
    {
        UChar3ImageType::IndexType pxLeft, pxBottomLeft, pxBottom, pxBottomRight, pxRight, pxTopRight, pxTop, pxTopLeft;
        pxLeft = pxBottomLeft = pxBottom = pxBottomRight = pxRight = pxTopRight = pxTop = pxTopLeft = px2;
        pxLeft[0] = px2[0]-1;

        pxBottomLeft[0] = px2[0]-1;
        pxBottomLeft[1] = px2[1]+1;

        pxBottom[1] = px2[1]+1;

        pxBottomRight[0] = px2[0]+1;
        pxBottomRight[1] = px2[1]+1;

        pxRight[0] = px2[0]+1;

        pxTopRight[0] = px2[0]+1;
        pxTopRight[1] = px2[1]-1;

        pxTop[1] = px2[1]-1;

        pxTopLeft[0] = px2[0]-1;
        pxTopLeft[1] = px2[1]-1;

        if (contourImage->GetPixel(pxLeft) != 0)
        {
            px2 = pxLeft;
        }
        else if (contourImage->GetPixel(pxBottomLeft) != 0)
        {
            px2 = pxBottomLeft;
        }
        else if (contourImage->GetPixel(pxBottom) != 0)
        {
            px2 = pxBottom;
        }
        else if (contourImage->GetPixel(pxBottomRight) != 0)
        {
            px2 = pxBottomRight;
        }
        else if (contourImage->GetPixel(pxRight) != 0)
        {
            px2 = pxRight;
        }
        else if (contourImage->GetPixel(pxTopRight) != 0)
        {
            px2 = pxTopRight;
        }
        else if (contourImage->GetPixel(pxTop) != 0)
        {
            px2 = pxTop;
        }
        else if (contourImage->GetPixel(pxTopLeft) != 0)
        {
            px2 = pxTopLeft;
        }
        else
            break;

        addPointToBuffer(view2d, bufferPoints, px2);
        contourImage->SetPixel(px2, 0);
    }
    while (px2 != px);
}
