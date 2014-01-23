#include <medVtkView.h>

#include <QSlider>

#include <QVTKWidget2.h>

#include <vtkRenderer.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkImageViewCollection.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera2.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkInriaInteractorStyleRubberBandZoom.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSignalBlocker.h>

#include <medVtkViewObserver.h>
#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>


class medVtkViewPrivate
{
public:
    // possible lut
    QStringList lut;

    // internal state
    vtkImageView *currentView;
    medAbstractData *currentData;
    medVtkView::medVtkViewOrientation currentOrientation;
    medVtkView::medVtkViewZoomMode currentZoomMode;
    QString currentLut;
    vtkInteractorStyle *interactorStyle2D;
    vtkInteractorStyle *interactorStyle3D;

    // renderers
    vtkGenericOpenGLRenderWindow *renWin;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;

    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkImageViewCollection *viewCollection;

    // widgets
    QWidget *mainWidget;
    QWidget *toolWidget;
    QVTKWidget2 *receiverWidget;
    QSlider *slider;
    QPushButton *linkPositionButton;
    QPushButton *linkWindowLevelButton;
    QPushButton *playButton;
    QPushButton *closeButton;
    QPushButton *maximizeButton;
    static const int toolWidth = 15;
    static const int toolheight = 15;

    // observer
    medVtkViewObserver *observer;

};

medVtkView::medVtkView(): d(new medVtkViewPrivate)
{
    // set property
    d->lut = this->getAvailableTransferFunctionPresets();
    this->addProperty("LookupTable", d->lut);

    // setup initial internal state of the view
    d->currentZoomMode = medVtkView::ZOOM_MODE_NORMAL;
    d->currentOrientation = medVtkView::VIEW_ORIENTATION_AXIAL;
    d->currentView = NULL;
    d->currentData = NULL;
    d->interactorStyle2D = NULL;
    d->interactorStyle3D = NULL;

    // construct render window
        // renWin
    d->renWin = vtkGenericOpenGLRenderWindow::New();
    d->renWin->StereoCapableWindowOn();
    d->renWin->SetStereoTypeToCrystalEyes();
            // needed for imersive room
    if (qApp->arguments().contains("--stereo"))
        d->renWin->SetStereoRender(1);
            // Necessary options for depth-peeling
    d->renWin->SetAlphaBitPlanes(1);
    d->renWin->SetMultiSamples(0);


    //construct renderers
        // renderer3d
    d->renderer3d = vtkRenderer::New();
    d->renderer3d->GetActiveCamera()->SetPosition(0, -1, 0);
    d->renderer3d->GetActiveCamera()->SetViewUp (0, 0, 1);
    d->renderer3d->GetActiveCamera()->SetFocalPoint(0, 0, 0);
        //renderer2d
    d->renderer2d = vtkRenderer::New();


    // construct views
        // view2d
    d->view2d = vtkImageView2D::New();
    d->view2d->SetRenderer(d->renderer2d);
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->SetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
    d->view2d->SetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
    d->view2d->SetKeyboardInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypeSlice);
    d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_AXIAL);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowRulerWidgetOn();
    d->view2d->SetRenderWindow (d->renWin); // set the interactor as well
    d->interactorStyle2D = d->view2d->GetInteractorStyle(); // save interactorStyle
        // view3d.
    d->view3d = vtkImageView3D::New();
    d->view3d->SetRenderer(d->renderer3d);
    d->view3d->SetShowBoxWidget(0);
    d->view3d->SetCroppingModeToOff();
    d->view3d->ShowScalarBarOff();
    d->view3d->GetTextProperty()->SetColor(1.0, 1.0, 1.0 );
    d->view3d->ShadeOn();
            //TODO find out what the purpose of this lines - RDE.
    vtkInteractorStyleTrackballCamera2 *interactorStyle = vtkInteractorStyleTrackballCamera2::New();
    d->view3d->SetInteractorStyle(interactorStyle);
    interactorStyle->Delete();
    d->view3d->SetRenderWindowInteractor(d->renWin->GetInteractor());
    d->view3d->SetRenderWindow(d->renWin);
    d->view3d->UnInstallInteractor();
    d->renWin->RemoveRenderer(d->renderer3d);
    d->interactorStyle3D = d->view3d->GetInteractorStyle(); // save interactorStyle
        // collection.
    d->viewCollection = vtkImageViewCollection::New();
    d->viewCollection->SetLinkCurrentPoint(0);
    d->viewCollection->SetLinkSliceMove(0);
    d->viewCollection->SetLinkColorWindowLevel(0);
    d->viewCollection->SetLinkCamera (0);
    d->viewCollection->SetLinkZoom(0);
    d->viewCollection->SetLinkPan(0);
    d->viewCollection->SetLinkTimeChange(0);
    d->viewCollection->SetLinkRequestedPosition(0);
    d->viewCollection->AddItem(d->view2d);
    d->viewCollection->AddItem(d->view3d);


    // construct main widget
        // retreive the main window
    QMainWindow * mainWindowApp = dynamic_cast< QMainWindow * >(
        qApp->property( "MainWindow" ).value< QObject * >() );

    d->mainWidget = new QWidget(mainWindowApp);
    QVBoxLayout *mainLayout = new QVBoxLayout(d->mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
        // toolWidget
    d->toolWidget = this->constructToolWidget(d->mainWidget);
    mainLayout->addWidget(d->toolWidget);
        //receiverWidget
    d->receiverWidget = new QVTKWidget2(d->mainWidget);
    d->receiverWidget->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
    d->receiverWidget->setFocusPolicy ( Qt::ClickFocus );
            //TODO: find out what is it for and write it here - RDE.
    d->receiverWidget->installEventFilter(this);
    d->receiverWidget->SetRenderWindow(d->renWin);
    mainLayout->addWidget(d->receiverWidget);


    // construct observer
    d->observer = medVtkViewObserver::New();
    d->observer->setSlider(d->slider);
    d->observer->setView(this);

    d->view2d->AddObserver(vtkImageView::CurrentPointChangedEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView::WindowLevelChangedEvent, d->observer, 0);
    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
    d->view3d->GetInteractorStyle()->AddObserver (vtkCommand::InteractionEvent, d->observer, 0);

    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, d->observer, 0);
    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyReleaseEvent, d->observer, 0);


    // deal with the loss of focus on the application.
    connect(mainWindowApp, SIGNAL(mainWindowDeactivated()), this, SLOT(processLostOfFocus()));
}

medVtkView::~medVtkView()
{
    delete d;
    d = NULL;
}

QString medVtkView::s_identifier()
{
    return "medVtkView";
}

QString medVtkView::identifier() const
{
    return medVtkView::s_identifier();
}

bool medVtkView::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewType ( medVtkView::s_identifier(), createMedVtkView );
}

dtkAbstractView *createMedVtkView()
{
    return new medVtkView;
}


void medVtkView::addLayer_impl(int layer)
{
    if(layer == 0)
    {
        qDebug() << "about to initialise the first layer of the " << this->identifier() <<": " << *this;
        d->currentData = this->dataAtLayer(0);
        this->setPatientStudySeriesName(d->currentData);

        medAbstractDataImage *imageData = dynamic_cast<medAbstractDataImage*>(d->currentData);
        if(imageData)
            this->setPreferedOrientation(imageData);
        else
        {
            //TODO: deal with the case where the first layer is not a medAbstractImage - RDE.
            qDebug()<<"The first data cannot be cast in medAbstractDataImage, not tested yet.";
            this->setOrientation(medVtkView::VIEW_ORIENTATION_AXIAL);
        }
        this->updateToolWidgets(d->currentData);
    }
}

void medVtkView::setPreferedOrientation(medAbstractDataImage *imageData)
{

    if(!d->view2d)
    {
        //TODO: I have the feeling that this check is useless.
        qWarning() << "From " << this->identifier() << ": " << this << ", " << "view2d is NULL.";
        return;
    }

    switch (d->view2d->GetViewOrientation())
    {
    case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
        this->setOrientation(medVtkView::VIEW_ORIENTATION_SAGITTAL);
        break;
    case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
        this->setOrientation(medVtkView::VIEW_ORIENTATION_CORONAL);
        break;
    case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
        this->setOrientation(medVtkView::VIEW_ORIENTATION_AXIAL);
        break;
    default:
        qDebug()<< "From " << this->identifier() << ": " << this << ", "
                << "unable to find preferred orientation, default is axial.";
         this->setOrientation(medVtkView::VIEW_ORIENTATION_AXIAL);
        break;
    }
}

void medVtkView::setPatientStudySeriesName(medAbstractData *data)
{
    if(!d->view2d)
    {
        //TODO: I have the feeling that this check is useless.
        qWarning() << "From " << this->identifier() << ": " << this << ", " << "view2d is NULL.";
        return;
    }

    if(!d->view3d)
    {
        //TODO: I have the feeling that this check is useless.
        qWarning() << "From " << this->identifier() << ": " << this << ", " << "view3d is NULL.";
        return;
    }

    if(data->hasMetaData(medMetaDataKeys::PatientName.key()))
    {
        const QString patientName = data->metaDataValues(medMetaDataKeys::PatientName.key())[0];
        d->view2d->SetPatientName(patientName.toAscii().constData());
        d->view3d->SetPatientName(patientName.toAscii().constData());
    }

    if(data->hasMetaData(medMetaDataKeys::StudyDescription.key()))
    {
        const QString studyName = data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
        d->view2d->SetStudyName(studyName.toAscii().constData());
        d->view3d->SetStudyName(studyName.toAscii().constData());
    }

    if (data->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
    {
        const QString seriesName = data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];
        d->view2d->SetSeriesName(seriesName.toAscii().constData());
        d->view3d->SetSeriesName(seriesName.toAscii().constData());
    }
}

void medVtkView::updateToolWidgets(medAbstractData *data)
{
    // Block signals to slider and plyButton as long as 'blockers' live.
    dtkSignalBlocker sliderBlocker(d->slider);
    dtkSignalBlocker playButtonBlocker(d->playButton);

    switch (d->currentOrientation)
    {
    case medVtkView::VIEW_ORIENTATION_3D:
    {
        d->slider->hide();
        d->playButton->hide();
        break;
    }
    default:
    {
        //TODO: I have the feeling that this check is useless.
        if(!d->view2d)
        {
            qWarning() << "From " << this->identifier() << ": " << this << ", " << "view2D is NULL.";
            return;
        }


        int sliceOrientation = d->view2d->GetSliceOrientation();
        if(medAbstractDataImage *imageData = dynamic_cast<medAbstractDataImage *>(data))
        {
            switch(sliceOrientation)
            {
                case vtkImageView2D::SLICE_ORIENTATION_XY:
                    d->slider->setRange (0, imageData->zDimension()-1);
                    break;
                case vtkImageView2D::SLICE_ORIENTATION_XZ:
                    d->slider->setRange (0, imageData->yDimension()-1);
                    break;
                case vtkImageView2D::SLICE_ORIENTATION_YZ:
                    d->slider->setRange (0, imageData->xDimension()-1);
                    break;
            }
        }
        else
            d->slider->setRange (0, 99);

        unsigned int slice = d->view2d->GetSlice();
        d->slider->setValue(slice);
        d->slider->show();
        d->playButton->show();
        break;
    }
    }//end switch
}

void medVtkView::moveToSlice(int slice)
{
    d->observer->lock();
    d->view2d->SetSlice(slice);
    d->observer->unlock();

    double *pos = d->view2d->GetCurrentPoint();
    QVector3D position (pos[0], pos[1], pos[2]);
    emit positionChanged (position, this->isPositionLinked());

    d->view2d->Render();
}

void medVtkView::processLostOfFocus()
{
    // This function must contains all the different actions that we want to happen
    // in case the software loses the focus
    this->setZoomMode(medVtkView::ZOOM_MODE_NORMAL);
}


QWidget* medVtkView::constructToolWidget(QWidget* parent)
{
    //construct the components of the tool widget
    QWidget *toolWidget = new QWidget(parent);
    QHBoxLayout *toolsLayout = new QHBoxLayout;
    toolsLayout->setContentsMargins(0, 0, 0, 0);
    toolsLayout->setSpacing(0);

        // playButton
    d->playButton = new QPushButton(toolWidget);
    QIcon playIcon(":/icons/play.png");
    playIcon.addPixmap(QPixmap(":/icons/pause.png"), QIcon::Normal, QIcon::On);
    d->playButton->setIcon(playIcon);
    d->playButton->setToolTip(tr("Play through the slices") );
    d->playButton->setCheckable(true);
    d->playButton->setMaximumHeight(medVtkViewPrivate::toolheight);
    d->playButton->setMaximumWidth(medVtkViewPrivate::toolWidth);
    d->playButton->setFocusPolicy (Qt::NoFocus);
    d->playButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->playButton->setObjectName("tool");
    connect(d->playButton, SIGNAL(clicked(bool)), this, SLOT(play(bool)));
    toolsLayout->addWidget(d->playButton);

        // slider
    d->slider = new QSlider(Qt::Horizontal, toolWidget);
    d->slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->slider->setFocusPolicy(Qt::NoFocus);
    toolsLayout->addWidget(d->slider);
    connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(moveToSLice(int)));


        // linkPositionButton
    d->linkPositionButton = new QPushButton (toolWidget);
    QIcon linkIcon(":/icons/broken_link.png");
    linkIcon.addFile(":/icons/link.svg", QSize(48,48), QIcon::Normal, QIcon::On);
    d->linkPositionButton->setIcon(linkIcon);
    d->linkPositionButton->setToolTip(tr("Link the position with other views"));
    d->linkPositionButton->setCheckable(true);
    d->linkPositionButton->setMaximumHeight(medVtkViewPrivate::toolheight);
    d->linkPositionButton->setMaximumWidth(medVtkViewPrivate::toolWidth);
    d->linkPositionButton->setFocusPolicy(Qt::NoFocus);
    d->linkPositionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->linkPositionButton->setObjectName("tool");
    connect(d->linkPositionButton, SIGNAL(clicked(bool)), this, SLOT (toggleLinkPosition(bool)));
    connect(d->linkPositionButton, SIGNAL(clicked(bool)), this, SLOT (toggleLinkCamera(bool)));
    toolsLayout->addWidget(d->linkPositionButton);


        // linkWindowLevelButton
    d->linkWindowLevelButton = new QPushButton(toolWidget);
    d->linkWindowLevelButton->setIcon(linkIcon);
    d->linkWindowLevelButton->setToolTip(tr("Link the window/level with other views"));
    d->linkWindowLevelButton->setCheckable(true);
    d->linkWindowLevelButton->setMaximumHeight(medVtkViewPrivate::toolheight);
    d->linkWindowLevelButton->setMaximumWidth(medVtkViewPrivate::toolWidth);
    d->linkWindowLevelButton->setFocusPolicy(Qt::NoFocus);
    d->linkWindowLevelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->linkWindowLevelButton->setObjectName("tool");
    connect(d->linkWindowLevelButton, SIGNAL(clicked(bool)) ,this, SLOT(toggleLinkWindowing(bool)));
    toolsLayout->addWidget(d->linkWindowLevelButton);

        // fullScreenButton
    d->maximizeButton = new QPushButton(toolWidget);
    QIcon maximizeIcon(":/icons/maximize.png");
    maximizeIcon.addFile(":/icons/un_maximize.svg", QSize(48,48), QIcon::Normal, QIcon::On);
    d->maximizeButton->setIcon(maximizeIcon);
    d->maximizeButton->setToolTip(tr("(Un)Maximize the view"));
    d->maximizeButton->setCheckable(true);
    d->maximizeButton->setMaximumHeight(medVtkViewPrivate::toolheight);
    d->maximizeButton->setMaximumWidth(medVtkViewPrivate::toolWidth);
    d->maximizeButton->setFocusPolicy (Qt::NoFocus);
    d->maximizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->maximizeButton->setObjectName("tool");
    connect(d->maximizeButton, SIGNAL(clicked(bool)), this, SIGNAL(toggleMaximize(bool)));
    toolsLayout->addWidget(d->maximizeButton);


        // closeButton
    d->closeButton = new QPushButton(toolWidget);
    d->closeButton->setIcon(QIcon(":/icons/whitecross.svg"));
    d->closeButton->setToolTip(tr("Close View"));
    d->closeButton->setCheckable(false);
    d->closeButton->setMaximumHeight(medVtkViewPrivate::toolheight);
    d->closeButton->setMaximumWidth(medVtkViewPrivate::toolWidth);
    d->closeButton->setFocusPolicy(Qt::NoFocus);
    d->closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->closeButton->setObjectName ("tool");
    connect(d->closeButton, SIGNAL(clicked()), this, SIGNAL(closing()));
    toolsLayout->addWidget(d->closeButton);

    return toolWidget;
}

QStringList& medVtkView::getAvailableTransferFunctionPresets() const
{
    QStringList lut;
    std::vector<std::string> presets = vtkTransferFunctionPresets::GetAvailablePresets();
    for (std::vector<std::string>::iterator it(presets.begin()); it != presets.end(); ++it)
        lut << QString::fromStdString (*it);

    return lut;
}


void medVtkView::setOrientation(medVtkViewOrientation orientation)
{
    if (orientation == d->currentOrientation)
    {
        qDebug()<< "From " << this->identifier() << ": " << this << ", " << "attempt to set twice the same orientation";
        return;
    }

    d->currentOrientation = orientation;

    // in case the max range becomes smaller than the actual value, a signal is emitted and
    // we don't want it
    dtkSignalBlocker sliderBlocker(d->slider);
    dtkSignalBlocker thisBlocker(this);


    // retreive current view state.
    double currentPoint[3];
    QVector <double> windows;
    QVector <double> levels;
    int timeIndex = 0;

    if (d->currentView)
    {
        d->currentView->GetCurrentPoint(pcurrentPoint);
        timeIndex = d->currentView->GetTimeIndex();

        // for the moment only do this if going to 3d mode:
        if (orientation == medVtkView::VIEW_ORIENTATION_3D)
        {
            for (int i =0; i < d->currentView->GetNumberOfLayers(); i++)
            {
                windows << d->view2d->GetColorWindow(i);
                levels << d->view2d->GetColorLevel(i);
            }
        }

        // unset current view
        d->currentView->UnInstallInteractor();
        d->currentView->SetRenderWindow(0);
        d->renWin->RemoveRenderer(d->currentView->GetRenderer());
    }


    // update internal state
    switch(d->currentOrientation)
    {
    case medVtkView::VIEW_ORIENTATION_3D:
    {
        d->currentView = d->view3d;
        for (int i=0; i < d->currentView->GetNumberOfLayers(); i++)
        {
            d->currentView->SetColorWindow(windows[i], i);
            d->currentView->SetColorLevel(levels[i], i);
        }
        break;
    }
    case medVtkView::VIEW_ORIENTATION_AXIAL:
    {
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_AXIAL);
        d->currentView = d->view2d;
        break;
    }
    case medVtkView::VIEW_ORIENTATION_SAGITTAL:
    {
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
        d->currentView = d->view2d;
        break;
    }
    case medVtkView::VIEW_ORIENTATION_CORONAL:
    {
        d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_CORONAL);
        d->currentView = d->view2d;
        break;
    }
    }   // end switch

    // reset current view
    d->currentView->SetRenderWindow(d->renWin);
    d->currentView->SetCurrentPoint(currentPoint);
    d->currentView->SetTimeIndex(timeIndex);

    // update tool widgets
    this->updateToolWidgets(d->currentData);
}


void medVtkView::setZoomMode(medVtkViewZoomMode zoomMode)
{
    d->currentZoomMode = zoomMode;
    switch(d->currentZoomMode)
    {
    case medVtkView::ZOOM_MODE_NORMAL:
    {
         d->view2d->GetInteractor()->SetInteractorStyle(d->interactorStyle2D);
        break;
    }
    case medVtkView::ZOOM_MODE_RUBBERBAND:
    {
        // tmp change of interactor style to enjoy rubber band zooming.
        vtkInriaInteractorStyleRubberBandZoom * interactorStyle = vtkInriaInteractorStyleRubberBandZoom::New();
        interactorStyle->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
        interactorStyle->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
        d->view2d->GetInteractor()->SetInteractorStyle(interactorStyle);
        interactorStyle->Delete();
        break;
    }
    }//end switch
}
