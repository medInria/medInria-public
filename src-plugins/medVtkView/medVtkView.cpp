/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkView.h>

#include <QWidget>
#include <QHash>

#include <QVTKWidget.h>

#include <vtkRenderer.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera2.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkInriaInteractorStyleRubberBandZoom.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>

#include <medViewFactory.h>
#include <medVtkViewBackend.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>
#include <medVtkViewObserver.h>
#include <medBoolGroupParameter.h>
#include <medBoolParameter.h>
#include <medDataIndexListParameter.h>
#include <medToolBox.h>
#include <medMetaDataKeys.h>
#include <medParameterPool.h>
#include <medParameterPoolManager.h>
#include <medSettingsManager.h>

// declare x11-specific function to prevent the window manager breaking thumbnail generation
#ifdef Q_WS_X11
void qt_x11_wait_for_window_manager(QWidget*);
#endif

class medVtkViewPrivate
{
public:
    // internal state
    vtkImageView *currentView; //2d or 3d dependig on the navigator orientation.

    vtkInteractorStyle *interactorStyle2D;

    // renderers
    vtkRenderWindow *renWin;
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;

    // widgets
    QPointer<QVTKWidget> viewWidget;

    medVtkViewObserver *observer;

    medBoolParameter *rubberBandZoomParameter;

    QScopedPointer<medVtkViewBackend> backend;
};

medVtkView::medVtkView(QObject* parent): medAbstractImageView(parent),
    d(new medVtkViewPrivate)
{
    // setup initial internal state of the view
    d->currentView = NULL;
    d->interactorStyle2D = NULL;

    // construct render window
        // renWin
    d->renWin = vtkRenderWindow::New();
    d->renWin->StereoCapableWindowOn();
    d->renWin->SetStereoTypeToCrystalEyes();
    d->renWin->SetAlphaBitPlanes(1);
    d->renWin->SetMultiSamples(0);
            // needed for imersive room
    if (qApp->arguments().contains("--stereo"))
        d->renWin->SetStereoRender(1);

    // construct views
        // view2d
    d->view2d = vtkImageView2D::New();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->SetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
    d->view2d->SetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
    d->view2d->SetKeyboardInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypeSlice);
    d->view2d->SetViewOrientation(vtkImageView2D::VIEW_ORIENTATION_AXIAL);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowRulerWidgetOn();
    d->interactorStyle2D = d->view2d->GetInteractorStyle(); // save interactorStyle
        // view3d.
    d->view3d = vtkImageView3D::New();
    d->view3d->SetShowBoxWidget(0);
    d->view3d->SetCroppingModeToOff();
    d->view3d->ShowScalarBarOff();
    d->view3d->GetTextProperty()->SetColor(1.0, 1.0, 1.0 );
    d->view3d->ShadeOn();
    d->view3d->GetRenderer()->GetActiveCamera()->SetPosition(0, -1, 0);
    d->view3d->GetRenderer()->GetActiveCamera()->SetViewUp (0, 0, 1);
    d->view3d->GetRenderer()->GetActiveCamera()->SetFocalPoint(0, 0, 0);

    // TrackballCamera2
    vtkInteractorStyleTrackballCamera2 *interactorStyle = vtkInteractorStyleTrackballCamera2::New();
    d->view3d->SetInteractorStyle(interactorStyle);
    interactorStyle->Delete();

    d->viewWidget = new QVTKWidget;
    // Event filter used to know if the view is selecetd or not
    d->viewWidget->installEventFilter(this);
    d->viewWidget->setFocusPolicy(Qt::ClickFocus );
    d->viewWidget->SetRenderWindow(d->renWin);
    d->viewWidget->setCursor(QCursor(Qt::CrossCursor));

    d->backend.reset(new medVtkViewBackend(d->view2d, d->view3d, d->renWin));

    d->observer = medVtkViewObserver::New();
    d->observer->setView(this);

    d->view2d->AddObserver(vtkImageView::CurrentPointChangedEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView::WindowLevelChangedEvent,  d->observer, 0);
    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraZoomEvent, d->observer, 0);
    d->view2d->GetInteractorStyle()->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView2DCommand::CameraPanEvent, d->observer, 0);
    d->view2d->AddObserver(vtkImageView2DCommand::CameraZoomEvent,d->observer,0);
    d->view3d->GetInteractorStyle()->AddObserver(vtkCommand::InteractionEvent, d->observer, 0);

    d->rubberBandZoomParameter = new medBoolParameter("RubberBandZoom", this);
    connect(d->rubberBandZoomParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableRubberBandZoom(bool)));

    // Disable rubberBandMode if we leave the application.
    QMainWindow * mainWindowApp = dynamic_cast<QMainWindow *>
            (qApp->property( "MainWindow" ).value<QObject *>());

    if(mainWindowApp)
      connect(mainWindowApp, SIGNAL(mainWindowDeactivated()), this, SLOT(resetKeyboardInteractionModifier()));

    this->initialiseNavigators();

    connect(this, SIGNAL(currentLayerChanged()), this, SLOT(changeCurrentLayer()));
    connect(this, SIGNAL(layerAdded(uint)), this, SLOT(buildMouseInteractionParamPool(uint)));

    // do this last, that way all previous calls to render will fail (which they should)
    d->view2d->SetRenderWindow (d->renWin);

    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent,d->observer,0);
    d->view2d->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyReleaseEvent,d->observer,0);
}

medVtkView::~medVtkView()
{
    disconnect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));
    disconnect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(render()));

    int c = layersCount()-1;
    for(int i=c; i>=0; i--)
        removeLayer(i);

    d->view2d->Delete();
    d->view3d->Delete();
    d->observer->Delete();
    if (d->renWin->GetOffScreenRendering() == 1)
        d->renWin->SetOffScreenRendering(0);
    d->renWin->Delete();
    delete d->viewWidget;

    delete d;
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
    return medViewFactory::instance()->registerView<medVtkView>(QString("medVtkView"), QStringList()<<"medAbstractData");
}

medViewBackend * medVtkView::backend() const
{
    return d->backend.data();
}

QString medVtkView::description() const
{
    return "medVtkView";
}

QWidget* medVtkView::viewWidget()
{
    return d->viewWidget;
}

void medVtkView::reset()
{
    d->view2d->Reset();
    d->view3d->Reset();
    this->render();

}

void medVtkView::render()
{
    if(this->is2D())
    {
        d->view2d->Modified();
        d->view2d->Render();
    }
    else
    {
        d->view3d->Modified();
        d->view3d->Render();
    }
}

QPointF medVtkView::mapWorldToDisplayCoordinates(const QVector3D & worldVec)
{
    // The following code is implemented without calling ren->SetWorldPoint,
    // because that generates an unnecessary modified event.

    vtkRenderer * ren = d->currentView->GetRenderer();

    // Get window for dimensions
    vtkWindow * win = ren->GetVTKWindow();

    if ( !win )
        return QPointF();

    double wx = worldVec.x();
    double wy = worldVec.y();
    double wz = worldVec.z();

    ren->WorldToView( wx, wy, wz );

    // get physical window dimensions
    const int * size = win->GetSize();
    int sizex = size[0];
    int sizey = size[1];

    const double * viewport = ren->GetViewport( );

    double dx = (wx + 1.0) * (sizex*(viewport[2] - viewport[0])) / 2.0 + sizex*viewport[0];
    double dy = (wy + 1.0) * (sizey*(viewport[3] - viewport[1])) / 2.0 + sizey*viewport[1];

    // Convert VTK display coordinates to Qt (flipped in Y)
    return QPointF( dx, sizey - 1 - dy );
}

QVector3D medVtkView::mapDisplayToWorldCoordinates(const QPointF & scenePoint)
{
    // The following code is implemented without calling ren->SetWorldPoint,
    // because that generates an unnecessary modified event.

    vtkRenderer * ren = d->view2d->GetRenderer();

    /* get physical window dimensions */
    vtkWindow * win = ren->GetVTKWindow();

    if ( !win )
        return QVector3D();

    const int * size = win->GetSize();
    int sizex = size[0];
    int sizey = size[1];

    const double * viewport = ren->GetViewport();

    // Convert Qt display coordinates to VTK (flipped in Y)
    const double dx = scenePoint.x();
    const double dy = sizey - 1 - scenePoint.y();

    double vx = 2.0 * (dx - sizex * viewport[0]) / (sizex * (viewport[2] - viewport[0])) - 1.0;
    double vy = 2.0 * (dy - sizey * viewport[1]) / (sizey * (viewport[3] - viewport[1])) - 1.0;
    double vz = 0.;

    if (this->is2D())
    {
        // Project the point into the view plane.
        //vtkCamera * cam = ren->GetActiveCamera();
        double pointInDisplayPlane[3];

        d->view2d->GetCurrentPoint(pointInDisplayPlane);

        ren->WorldToView(pointInDisplayPlane[0],pointInDisplayPlane[1],pointInDisplayPlane[2]);
        vz = pointInDisplayPlane[2];
    }
    ren->ViewToWorld(vx,vy,vz);

    return QVector3D( vx, vy, vz );
}

QVector3D medVtkView::viewCenter()
{
    vtkRenderer * ren;
    if(this->is2D())
        ren = d->view2d->GetRenderer();
    else
        ren = d->view3d->GetRenderer();    double fp[3];
    ren->GetActiveCamera()->GetFocalPoint( fp);
    return QVector3D( fp[0], fp[1], fp[2] );
}

QVector3D medVtkView::viewPlaneNormal()
{
    double vpn[3];
    vtkRenderer * ren;
    if(this->is2D())
        ren = d->view2d->GetRenderer();
    else
        ren = d->view3d->GetRenderer();    ren->GetActiveCamera()->GetViewPlaneNormal(vpn);
    return QVector3D(vpn[0], vpn[1], vpn[2]);
}

QVector3D medVtkView::viewUp()
{
    double vup[3];
    vtkRenderer * ren;
    if(this->is2D())
        ren = d->view2d->GetRenderer();
    else
        ren = d->view3d->GetRenderer();

    ren->GetActiveCamera()->GetViewUp(vup);
    return QVector3D(vup[0], vup[1], vup[2]);
}

bool medVtkView::is2D()
{
    return this->orientation() == medImageView::VIEW_ORIENTATION_AXIAL ||
           this->orientation() == medImageView::VIEW_ORIENTATION_CORONAL ||
           this->orientation() == medImageView::VIEW_ORIENTATION_SAGITTAL;
}

qreal medVtkView::sliceThickness()
{
    double cr[2] = {0, 0};
    d->view2d->GetRenderer()->GetActiveCamera()->GetClippingRange(cr);
    return std::fabs(cr[1] - cr[0]);
}

qreal medVtkView::scale()
{
    double scale;
    if (this->is2D())
    {
        // The height of the viewport in world coordinates
        double camScale = d->view2d->GetRenderer()->GetActiveCamera()->GetParallelScale();
        double heightInPx = d->view2d->GetRenderWindow()->GetSize()[1];
        // return pixels per world coord.
        scale = heightInPx / camScale;
    }
    else
    {
        // Return scale at fp.
        double vup[4];
        d->view3d->GetRenderer()->GetActiveCamera()->GetViewUp(vup);
        vup[3] = 0;  //intentionally zero and not one.
        double MVup[4];
        d->view3d->GetRenderer()->GetActiveCamera()->GetViewTransformMatrix()->MultiplyPoint(vup, MVup);
        double lScale = vtkMath::Norm(MVup) / vtkMath::Norm(vup);
        //We now have the scale in viewport coords. (normalised).
        double heightInPx = d->view3d->GetRenderWindow()->GetSize()[1];
        scale = heightInPx *lScale;
    }

    if (scale < 0)
        scale *= -1;
    return scale;
}

void medVtkView::changeCurrentLayer()
{
    displayDataInfo(this->currentLayer());
}

void medVtkView::displayDataInfo(uint layer)
{
    medAbstractData *data = layerData(layer);
    if ( data )
    {
        if ( data->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
        {
            const QString patientName = data->metaDataValues ( medMetaDataKeys::PatientName.key() ) [0];
            d->view2d->SetPatientName ( patientName.toAscii().constData() );
            d->view3d->SetPatientName ( patientName.toAscii().constData() );
        }

        if ( data->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
        {
            const QString studyName = data->metaDataValues ( medMetaDataKeys::StudyDescription.key() ) [0];
            d->view2d->SetStudyName ( studyName.toAscii().constData() );
            d->view3d->SetStudyName ( studyName.toAscii().constData() );
        }

        if ( data->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
        {
            const QString seriesName = data->metaDataValues ( medMetaDataKeys::SeriesDescription.key() ) [0];
            d->view2d->SetSeriesName ( seriesName.toAscii().constData() );
            d->view3d->SetSeriesName ( seriesName.toAscii().constData() );
        }
    }
}

QImage medVtkView::buildThumbnail(const QSize &size)
{
    this->blockSignals(true);//we dont want to send things that would ending up on updating some gui things or whatever. - RDE
    int w(size.width()), h(size.height());

    // will cause crashes if any calls to renWin->Render() happened before this line
    d->viewWidget->resize(w,h);
    d->renWin->SetSize(w,h);
    render();

#ifdef Q_WS_X11
    // X11 likes to animate window creation, which means by the time we grab the
    // widget, it might not be fully ready yet, in which case we get artefacts.
    // Only necessary if rendering to an actual screen window.
    if(d->renWin->GetOffScreenRendering() == 0) {
        qt_x11_wait_for_window_manager(d->viewWidget);
    }
#endif

    QImage thumbnail = QPixmap::grabWidget(d->viewWidget).toImage();

    this->blockSignals(false);
    return thumbnail;
}

void medVtkView::buildMouseInteractionParamPool(uint layer)
{
    medSettingsManager * mnger = medSettingsManager::instance();
    QString interaction = mnger->value("interactions","mouse", "Windowing").toString();

    QList<medBoolParameter*> params;

    params.append(primaryNavigator()->mouseInteractionParameters());
    foreach (medAbstractNavigator* navigator, this->extraNavigators())
        params.append(navigator->mouseInteractionParameters());

    params.append(primaryInteractor(layer)->mouseInteractionParameters());
    foreach (medAbstractInteractor* interactor, this->extraInteractors(layer))
        params.append(interactor->mouseInteractionParameters());

    // add all mouse interaction params of the view in the "Mouse interaction" pool
    foreach (medBoolParameter* param, params)
    {
        medParameterPoolManager::instance()->linkParameter(param, "Mouse Interaction");
        connect(param, SIGNAL(valueChanged(bool)), this, SLOT(saveMouseInteractionSettings(bool)));

        // and activate the new inserted parameter according to what was activated in other views
        if(param->name() == interaction)
            param->setValue(true);
    }

    // Deal with rubber Zoom mode.
    medParameterPoolManager::instance()->linkParameter(d->rubberBandZoomParameter, "Mouse Interaction");
}

void medVtkView::saveMouseInteractionSettings(bool parameterEnabled)
{
    if(parameterEnabled)
    {
        medBoolParameter *parameter = dynamic_cast<medBoolParameter *>(this->sender());
        if(parameter)
            medSettingsManager::instance()->setValue("interactions","mouse", parameter->name());
    }
}

void medVtkView::enableRubberBandZoom(bool enable)
{
    if(this->orientation() == medImageView::VIEW_ORIENTATION_3D)
        return;

    if(enable)
    {
        medSettingsManager * mnger = medSettingsManager::instance();
        QString interaction = mnger->value("interactions","mouse", "Windowing").toString();

        vtkInriaInteractorStyleRubberBandZoom * interactorStyle = vtkInriaInteractorStyleRubberBandZoom::New();
        interactorStyle->AddObserver( vtkImageView2DCommand::CameraZoomEvent,d->observer,0 );
        interactorStyle->AddObserver( vtkImageView2DCommand::CameraPanEvent,d->observer,0 );

        interactorStyle->RightButtonModeOn();
        if(interaction != "Zooming")
            interactorStyle->LeftButtonModeOff();

        d->view2d->GetInteractor()->SetInteractorStyle(interactorStyle);
        interactorStyle->Delete();
    }
    else
        d->view2d->GetInteractor()->SetInteractorStyle(d->interactorStyle2D);
}

medBoolParameter* medVtkView::rubberBandZoomParameter() const
{
    return d->rubberBandZoomParameter;
}

void medVtkView::setOffscreenRendering(bool isOffscreen)
{
    d->renWin->SetOffScreenRendering(isOffscreen ? 1 : 0);
}

void medVtkView::resetKeyboardInteractionModifier()
{
    d->rubberBandZoomParameter->setValue(false);
}
