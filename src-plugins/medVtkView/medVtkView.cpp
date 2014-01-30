#include <medVtkView.h>

#include <medImageViewFactory.h>

//#include <QSlider>

//#include <QVTKWidget2.h>

//#include <vtkRenderer.h>
//#include <vtkImageView2D.h>
//#include <vtkImageView3D.h>
//#include <vtkImageViewCollection.h>
//#include <vtkRenderer.h>
//#include <vtkGenericOpenGLRenderWindow.h>
//#include <vtkInteractorStyleTrackballCamera2.h>
//#include <vtkCamera.h>
//#include <vtkTextProperty.h>
//#include <vtkImageView2DCommand.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkTransferFunctionPresets.h>
//#include <vtkInriaInteractorStyleRubberBandZoom.h>

//#include <dtkCore/dtkAbstractViewFactory.h>
//#include <dtkCore/dtkAbstractViewInteractor.h>
//#include <dtkCore/dtkSignalBlocker.h>

//#include <medVtkViewObserver.h>
//#include <medAbstractDataImage.h>
//#include <medMetaDataKeys.h>


class medVtkViewPrivate
{
public:
//    // possible lut
//    QStringList lut;

//    // internal state
//    vtkImageView *currentView;
//    medAbstractData *currentData;
//    medVtkView::medVtkViewOrientation currentOrientation;
//    medVtkView::medVtkViewZoomMode currentZoomMode;
//    QString currentLut;
//    vtkInteractorStyle *interactorStyle2D;
//    vtkInteractorStyle *interactorStyle3D;

//    // renderers
//    vtkGenericOpenGLRenderWindow *renWin;
//    vtkRenderer *renderer2d;
//    vtkRenderer *renderer3d;

//    // views
//    vtkImageView2D *view2d;
//    vtkImageView3D *view3d;
//    vtkImageViewCollection *viewCollection;

//    // widgets
//    QWidget *mainWidget;
//    QWidget *toolWidget;
//    QVTKWidget2 *receiverWidget;
//    QSlider *slider;
//    QPushButton *linkPositionButton;
//    QPushButton *linkWindowLevelButton;
//    QPushButton *playButton;
//    QPushButton *closeButton;
//    QPushButton *maximizeButton;
//    static const int toolWidth = 15;
//    static const int toolheight = 15;

//    // observer
//    medVtkViewObserver *observer;

};

medVtkView::medVtkView(): medAbstractImageView(parent),
    d(new medVtkViewPrivate)
{
//    // set property
//    d->lut = this->getAvailableTransferFunctionPresets();
//    this->addProperty("LookupTable", d->lut);

//    // setup initial internal state of the view
//    d->currentZoomMode = medVtkView::ZOOM_MODE_NORMAL;
//    d->currentOrientation = medVtkView::VIEW_ORIENTATION_AXIAL;
//    d->currentView = NULL;
//    d->currentData = NULL;
//    d->interactorStyle2D = NULL;
//    d->interactorStyle3D = NULL;

//    // construct render window
//        // renWin
//    d->renWin = vtkGenericOpenGLRenderWindow::New();
//    d->renWin->StereoCapableWindowOn();
//    d->renWin->SetStereoTypeToCrystalEyes();
//            // needed for imersive room
//    if (qApp->arguments().contains("--stereo"))
//        d->renWin->SetStereoRender(1);
//            // Necessary options for depth-peeling
//    d->renWin->SetAlphaBitPlanes(1);
//    d->renWin->SetMultiSamples(0);


//    //construct renderers
//        // renderer3d
//    d->renderer3d = vtkRenderer::New();
//    d->renderer3d->GetActiveCamera()->SetPosition(0, -1, 0);
//    d->renderer3d->GetActiveCamera()->SetViewUp (0, 0, 1);
//    d->renderer3d->GetActiveCamera()->SetFocalPoint(0, 0, 0);
//        //renderer2d
//    d->renderer2d = vtkRenderer::New();


//    // construct views
//        // view2d
//    d->view2d = vtkImageView2D::New();
//    d->view2d->SetRenderer(d->renderer2d);
//    d->view2d->SetBackground(0.0, 0.0, 0.0);
//    d->view2d->SetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
//    d->view2d->SetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
//    d->view2d->SetKeyboardInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypeSlice);
//    d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_AXIAL);
//    d->view2d->CursorFollowMouseOff();
//    d->view2d->ShowImageAxisOff();
//    d->view2d->ShowScalarBarOff();
//    d->view2d->ShowRulerWidgetOn();
//    d->view2d->SetRenderWindow (d->renWin); // set the interactor as well
//    d->interactorStyle2D = d->view2d->GetInteractorStyle(); // save interactorStyle
//        // view3d.
//    d->view3d = vtkImageView3D::New();
//    d->view3d->SetRenderer(d->renderer3d);
//    d->view3d->SetShowBoxWidget(0);
//    d->view3d->SetCroppingModeToOff();
//    d->view3d->ShowScalarBarOff();
//    d->view3d->GetTextProperty()->SetColor(1.0, 1.0, 1.0 );
//    d->view3d->ShadeOn();
//            //TODO find out what the purpose of this lines - RDE.
//    vtkInteractorStyleTrackballCamera2 *interactorStyle = vtkInteractorStyleTrackballCamera2::New();
//    d->view3d->SetInteractorStyle(interactorStyle);
//    interactorStyle->Delete();
//    d->view3d->SetRenderWindowInteractor(d->renWin->GetInteractor());
//    d->view3d->SetRenderWindow(d->renWin);
//    d->view3d->UnInstallInteractor();
//    d->renWin->RemoveRenderer(d->renderer3d);
//    d->interactorStyle3D = d->view3d->GetInteractorStyle(); // save interactorStyle
//        // collection.
//    d->viewCollection = vtkImageViewCollection::New();
//    d->viewCollection->SetLinkCurrentPoint(0);
//    d->viewCollection->SetLinkSliceMove(0);
//    d->viewCollection->SetLinkColorWindowLevel(0);
//    d->viewCollection->SetLinkCamera (0);
//    d->viewCollection->SetLinkZoom(0);
//    d->viewCollection->SetLinkPan(0);
//    d->viewCollection->SetLinkTimeChange(0);
//    d->viewCollection->SetLinkRequestedPosition(0);
//    d->viewCollection->AddItem(d->view2d);
//    d->viewCollection->AddItem(d->view3d);


//    // construct main widget
//        // retreive the main window
//    QMainWindow * mainWindowApp = dynamic_cast< QMainWindow * >(
//        qApp->property( "MainWindow" ).value< QObject * >() );

//    d->mainWidget = new QWidget(mainWindowApp);
//    QVBoxLayout *mainLayout = new QVBoxLayout(d->mainWidget);
//    mainLayout->setContentsMargins(0, 0, 0, 0);
//    mainLayout->setSpacing(0);
//        // toolWidget
//    d->toolWidget = this->constructToolWidget(d->mainWidget);
//    mainLayout->addWidget(d->toolWidget);
//        //receiverWidget
//    d->receiverWidget = new QVTKWidget2(d->mainWidget);
//    d->receiverWidget->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
//    d->receiverWidget->setFocusPolicy ( Qt::ClickFocus );
//            //TODO: find out what is it for and write it here - RDE.
//    d->receiverWidget->installEventFilter(this);
//    d->receiverWidget->SetRenderWindow(d->renWin);
//    mainLayout->addWidget(d->receiverWidget);


//    // construct observer
//    d->observer = medVtkViewObserver::New();
//    d->observer->setSlider(d->slider);
//    d->observer->setView(this);

//    d->view2d->AddObserver(vtkImageView::CurrentPointChangedEvent, d->observer, 0);
//    d->view2d->AddObserver(vtkImageView::WindowLevelChangedEvent, d->observer, 0);
//    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
//    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
//    d->view2d->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
//    d->view2d->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
//    d->view3d->GetInteractorStyle()->AddObserver (vtkCommand::InteractionEvent, d->observer, 0);

//    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, d->observer, 0);
//    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyReleaseEvent, d->observer, 0);


//    // deal with the loss of focus on the application.
//    connect(mainWindowApp, SIGNAL(mainWindowDeactivated()), this, SLOT(processLostOfFocus()));
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
    return medImageViewFactory::instance()->registerView("medVtkView",
                                                         QStringList() << "itk" << "mesh" << "prout");
}

