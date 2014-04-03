/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewItkDataImageInteractor.h>


#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>
#include <vtkImageViewCollection.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>

#include <medVtkViewBackend.h>
#include <medAbstractData.h>
#include <medImageViewFactory.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>
#include <medSlicingParameter.h>
#include <medVector3DParameter.h>
#include <medAbstractImageData.h>
#include <medCompositeParameter.h>

#include <QFormLayout>



class medVtkViewItkDataImageInteractorPrivate
{
public:
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkImageViewCollection *collection;
    medAbstractImageData *imageData;
    vtkRenderWindow *render;

    medAbstractImageView *medVtkView;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medIntParameter *opacityParam;
    medBoolParameter *visibiltyParameter;
    medCompositeParameter *windowLevelParameter;
    //medSlicingParameter *slicingParameter;
    medIntParameter *slicingParameter;

    medVector3DParameter *positionParameter;

    QWidget* toolbox;
    QWidget* toolbar;
    QImage thumbnail;

    int imageViewInternalLayer;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
{
    d->medVtkView = parent;
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->collection = vtkImageViewCollection::New();
    d->collection->AddItem(d->view2d);
    d->collection->AddItem(d->view3d);
    d->render = backend->renWin;
    d->imageData = NULL;

    d->lutParam = NULL;
    d->presetParam = NULL;
    d->opacityParam = NULL;
    d->visibiltyParameter = NULL;
    d->slicingParameter = NULL;
    d->windowLevelParameter = NULL;

    d->toolbox = NULL;
    d->toolbar = NULL;

    d->imageViewInternalLayer = 0;

}

medVtkViewItkDataImageInteractor::~medVtkViewItkDataImageInteractor()
{
    delete d;
    d = NULL;
}

QStringList medVtkViewItkDataImageInteractor::dataHandled()
{
    QStringList d = QStringList() << "itkDataImageChar3"
                                  << "itkDataImageUChar3"
                                  << "itkDataImageShort3"
                                  << "itkDataImageUShort3"
                                  << "itkDataImageInt3"
                                  << "itkDataImageLong3"
                                  << "itkDataImageULong3"
                                  << "itkDataImageFloat3"
                                  << "itkDataImageDouble3"
                                  << "itkDataImageRGB3"
                                  << "itkDataImageRGBA3"
                                  << "itkDataImageVector3";
    return  d;
}

QStringList medVtkViewItkDataImageInteractor::handled() const
{
    return medVtkViewItkDataImageInteractor::dataHandled();
}

QString medVtkViewItkDataImageInteractor::description() const
{
    return tr("Interactor for ITK images");
}


QString medVtkViewItkDataImageInteractor::identifier() const
{
    return "medVtkViewItkDataImageInteractor";
}

bool medVtkViewItkDataImageInteractor::registered()
{
    medImageViewFactory *factory = medImageViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkDataImageInteractor>("medVtkViewItkDataImageInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medVtkViewItkDataImageInteractor::dataHandled());
}

QList<medAbstractParameter*> medVtkViewItkDataImageInteractor::parameters()
{
    QList<medAbstractParameter*> params;
    params.append(d->lutParam);
    params.append(d->presetParam);
    params.append(d->opacityParam);
    params.append(d->visibiltyParameter);
    params.append(d->windowLevelParameter);
    params.append(d->slicingParameter);

    return params;
}


void medVtkViewItkDataImageInteractor::setData(medAbstractData *data)
{
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if (!(SetViewInput<itk::Image<char,3> >("itkDataImageChar3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<unsigned char,3> >("itkDataImageUChar3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<short,3> >("itkDataImageShort3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<unsigned short,3> >("itkDataImageUShort3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<int,3> >("itkDataImageInt3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<unsigned,3> >("itkDataImageUInt3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<long,3> >("itkDataImageLong3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<unsigned long,3> >("itkDataImageULong3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<float,3> >("itkDataImageFloat3", data , d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<double,3> >("itkDataImageDouble3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<itk::RGBPixel<unsigned char>,3> >("itkDataImageRGB3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<itk::RGBAPixel<unsigned char>,3> >("itkDataImageRGBA3", data, d->imageViewInternalLayer) ||
          SetViewInput<itk::Image<itk::Vector<unsigned char,3>,3> >("itkDataImageVector3", data, d->imageViewInternalLayer)))
    {
        qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
        return;
    }

    initParameters(d->imageData);
}

void medVtkViewItkDataImageInteractor::removeData()
{
    d->view2d->RemoveLayer(imageViewInternalLayer());
    d->view3d->RemoveLayer(imageViewInternalLayer());
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}


template <typename IMAGE>
bool medVtkViewItkDataImageInteractor::SetViewInput(const char* type, medAbstractData* data, int& layer)
{
    if (data->identifier() != type)
        return false;

    if (IMAGE* image = dynamic_cast<IMAGE*>((itk::Object*)(data->data())))
    {
        d->view2d->AddITKInput(image);
        d->view3d->AddITKInput(image);

        layer = d->view2d->GetNumberOfLayers()-1;

        return true;
    }
    return false;
}

void medVtkViewItkDataImageInteractor::initParameters(medAbstractImageData* data)
{
    d->imageData = data;

    d->lutParam = new medStringListParameter("Lut", this);
    QStringList lut = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    d->lutParam->addItems(lut);
    connect(d->lutParam, SIGNAL(valueChanged(QString)), this, SLOT(setLut(QString)));


    d->presetParam = new medStringListParameter("Preset", this);
    QStringList presets = QStringList() << "None" << "VR Muscles&Bones" << "Vascular I"
                                        << "Vascular II" << "Vascular III" << "Vascular IV"
                                        << "Standard" << "Soft" << "Soft on White"
                                        << "Soft on Blue" << "Red on White" << "Glossy" ;
    d->presetParam->addItems(presets);

    connect(d->presetParam, SIGNAL(valueChanged(QString)), this, SLOT(setPreset(QString)));

    d->opacityParam = new medIntParameter("Opacity", this);
    d->opacityParam->setRange(0, 100);
    d->opacityParam->setValue(100);
    connect(d->opacityParam, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)));

    d->visibiltyParameter = new medBoolParameter("Visibility", this);
    d->visibiltyParameter->setValue(true);
    connect(d->visibiltyParameter, SIGNAL(valueChanged(bool)), this, SLOT(setVisibility(bool)));



    d->view2d->GetInput()->Update();
    double* range = d->view2d->GetInput(d->imageViewInternalLayer)->GetScalarRange();
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    d->windowLevelParameter = new medCompositeParameter("WindowLevel", this);
    connect(d->windowLevelParameter, SIGNAL(valueChanged(QList<QVariant>)), this, SLOT(setWindowLevel(QList<QVariant>)));
    d->windowLevelParameter->addVariant("Window", QVariant(window), QVariant(range[0]), QVariant(range[1]));
    d->windowLevelParameter->addVariant("Level", QVariant(level), QVariant(range[0]), QVariant(range[1]));

    //TODO GPR-RDE: Shouldn't it be a navigator parameter?
    d->slicingParameter = new medIntParameter("Slicing", this);
    // slice orientation may differ from view orientation. Adapt slider range accordingly.
    int orientationId = d->view2d->GetSliceOrientation();
    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
        d->slicingParameter->setRange(0, d->imageData->zDimension()-1);
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
        d->slicingParameter->setRange (0, d->imageData->yDimension()-1);
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
        d->slicingParameter->setRange (0, d->imageData->xDimension()-1);

    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));

    d->positionParameter = new medVector3DParameter("position", this);
    connect(d->positionParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(moveToSliceAtPosition(QVector3D)));

    connect(d->medVtkView, SIGNAL(sliceChanged(int)), d->slicingParameter, SLOT(setValue(int)) );
    connect(d->medVtkView, SIGNAL(windowLevelChanged(double,double, unsigned int)), this, SLOT(updateWindowLevelParam(double,double, unsigned int)) );
    connect(d->medVtkView, SIGNAL(currentLayerChanged()), this, SLOT(updateImageViewInternalLayer()));

    if(d->medVtkView->layer(d->imageData) == 0)
    {
        switch(d->view2d->GetViewOrientation())
        {
        case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
            d->medVtkView->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
            d->medVtkView->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
            d->medVtkView->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
            break;
        }
    }
}

void medVtkViewItkDataImageInteractor::setOpacity (int opacity)
{
    double value = (double)(opacity) / 100.0;
    setOpacity(value);
}

void medVtkViewItkDataImageInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, imageViewInternalLayer());
    d->view2d->SetOpacity (opacity, imageViewInternalLayer());

    update();
}

double medVtkViewItkDataImageInteractor::opacity() const
{
     double opacity = static_cast<double>(d->opacityParam->value()) / 100.0;
     return opacity;
}

void medVtkViewItkDataImageInteractor::setVisibility(bool visible)
{
    if(visible)
    {
        d->view2d->SetVisibility(1, imageViewInternalLayer());
        d->view3d->SetVisibility(1, imageViewInternalLayer());
    }
    else
    {
        d->view2d->SetVisibility(0, imageViewInternalLayer());
        d->view3d->SetVisibility(0, imageViewInternalLayer());
    }

    update();
}

bool medVtkViewItkDataImageInteractor::visibility() const
{
    return d->visibiltyParameter->value();
}

QString medVtkViewItkDataImageInteractor::lut() const
{
    return d->lutParam->value();
}

void medVtkViewItkDataImageInteractor::setLut(QString value)
{
    typedef vtkTransferFunctionPresets Presets;
    vtkColorTransferFunction * rgb   = vtkColorTransferFunction::New();
    vtkPiecewiseFunction     * alpha = vtkPiecewiseFunction::New();

    Presets::GetTransferFunction(value.toStdString(), rgb, alpha );

    if (imageViewInternalLayer() == 0)
        d->view2d->SetTransferFunctions(rgb, alpha, imageViewInternalLayer());
    else
    {
        //TODO: find out why its not the same process - RDE
        vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
        d->view2d->SetLookupTable(lut, imageViewInternalLayer());
        lut->Delete();
    }

    d->view3d->SetTransferFunctions(rgb, alpha, imageViewInternalLayer());
    d->view3d->SetLookupTable(vtkLookupTableManager::GetLookupTable(value.toStdString()), imageViewInternalLayer());

    rgb->Delete();
    alpha->Delete();

    update();
}

void medVtkViewItkDataImageInteractor::setPreset(QString preset)
{
    //TODO to complete wat for theo stuff ?) - RDE
    DTK_UNUSED(preset);
}

QString medVtkViewItkDataImageInteractor::preset() const
{
  return d->presetParam->value();
}


QWidget* medVtkViewItkDataImageInteractor::toolBarWidget()
{
    if(!d->toolbar)
    {
        d->toolbar = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(d->toolbar);
        layout->setContentsMargins ( 0, 0, 0, 0 );
        layout->setSpacing ( 0 );
        QSlider *slider = d->slicingParameter->getSlider();
        slider->setOrientation(Qt::Horizontal);
        layout->addWidget(slider);
    }
    return d->toolbar;
}

QWidget* medVtkViewItkDataImageInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        layout->addRow(d->windowLevelParameter->getWidget());
        layout->addRow(d->lutParam->getLabel(), d->lutParam->getComboBox());
        layout->addRow(d->presetParam->getLabel(), d->presetParam->getComboBox());
    }
    return d->toolbox;
}

QWidget* medVtkViewItkDataImageInteractor::layerWidget()
{
    QSlider *slider = d->opacityParam->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return d->opacityParam->getSlider();
}

void medVtkViewItkDataImageInteractor::setWindowLevel (double &window, double &level)
{
    QList<QVariant> value;
    value.append(QVariant(window));
    value.append(QVariant(level));
    d->windowLevelParameter->setValue(value);
}

void medVtkViewItkDataImageInteractor::windowLevel(double &window, double &level)
{
    window = d->windowLevelParameter->value().at(0).toDouble();
    level = d->windowLevelParameter->value().at(1).toDouble();
}

void medVtkViewItkDataImageInteractor::moveToSlice(int slice)
{
    int zslice = d->view2d->GetSlice();

    if(slice!=zslice)
      d->view2d->SetSlice ( slice );

    /*  double *pos = view->GetCurrentPoint();
        QVector3D position ( pos[0], pos[1], pos[2] );
        emit positionChanged ( position, this->positionLinked() );*/
    update();
}

void medVtkViewItkDataImageInteractor::moveToSliceAtPosition(const QVector3D &position)
{
//    double pos[3];
//    pos[0] = position.x();
//    pos[1] = position.y();
//    pos[2] = position.z();
//    d->observer->lock();
//    d->currentView->SetCurrentPoint ( pos );
//    d->currentView->UpdateCursorPosition(pos);
//    d->observer->unlock();

//    // update slider, if currentView is 2D view
//    if ( vtkImageView2D *view2d = vtkImageView2D::SafeDownCast ( d->currentView ) )
//    {
//        unsigned int zslice = view2d->GetSlice();
//        dtkSignalBlocker sliderBlocker( d->slider );
//        d->slider->setValue ( zslice );
//    }
    double pos[3];
    pos[0] = position.x();
    pos[1] = position.y();
    pos[2] = position.z();

    d->view2d->SetCurrentPoint ( pos );
    d->view2d->UpdateCursorPosition(pos);

    if(d->medVtkView->is2D())
    {
        unsigned int zslice = d->view2d->GetSlice();
        d->slicingParameter->setValue ( zslice );
    }
}

void medVtkViewItkDataImageInteractor::setWindow(double window)
{
    d->view2d->SetColorWindow(window, imageViewInternalLayer());
    d->view3d->SetColorWindow(window, imageViewInternalLayer());
}

void medVtkViewItkDataImageInteractor::setLevel(double level)
{
    d->view2d->SetColorLevel(level, imageViewInternalLayer());
    d->view3d->SetColorLevel(level, imageViewInternalLayer());

}

void medVtkViewItkDataImageInteractor::setWindowLevel(QList<QVariant> values)
{
    if(values.size() != 2 )
    {
        qWarning() << "Window/Level parameters are incorrect";
        return;
    }
    setWindow(values.at(0).toDouble());
    setLevel(values.at(1).toDouble());
    update();
}

void medVtkViewItkDataImageInteractor::update()
{
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}

void medVtkViewItkDataImageInteractor::updateWidgets()
{
    // slice orientation may differ from view orientation. Adapt slider range accordingly.
    int orientationId = d->view2d->GetSliceOrientation();
    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
        d->slicingParameter->setRange(0, d->imageData->zDimension()-1);
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
        d->slicingParameter->setRange (0, d->imageData->yDimension()-1);
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
        d->slicingParameter->setRange (0, d->imageData->xDimension()-1);

    // update slider position
    if(d->medVtkView->is2D())
    {
        unsigned int zslice = d->view2d->GetSlice();
        d->slicingParameter->setValue ( zslice );
    }

}

void medVtkViewItkDataImageInteractor::updateWindowLevelParam(double window, double level, unsigned int layer)
{
    if( d->medVtkView->layer(d->imageData) != layer )
        return;

    QList<QVariant> values;
    values.append(QVariant(window));
    values.append(QVariant(level));
    d->windowLevelParameter->setValue(values);
}

QImage medVtkViewItkDataImageInteractor::generateThumbnail(const QSize &size)
{
    int w(size.width()), h(size.height());

    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();

    d->render->SetOffScreenRendering(1);

    d->medVtkView->viewWidget()->resize(w,h);
    d->render->vtkRenderWindow::SetSize(w,h);
    d->view2d->Reset();
    d->view2d->Render();

    QGLWidget *glWidget = dynamic_cast<QGLWidget *>(d->medVtkView->viewWidget());
    d->thumbnail = glWidget->grabFrameBuffer();

    d->render->SetOffScreenRendering(0);

    return d->thumbnail;
}

int medVtkViewItkDataImageInteractor::imageViewInternalLayer()
{
    return d->imageViewInternalLayer;
}

void medVtkViewItkDataImageInteractor::setImageViewInternalLayer(int layer)
{
    d->imageViewInternalLayer = layer;
}

void medVtkViewItkDataImageInteractor::updateImageViewInternalLayer()
{
    if( d->medVtkView->layer(d->imageData) != d->medVtkView->currentLayer() )
        return;

    d->view2d->SetCurrentLayer(d->imageViewInternalLayer);
    d->view3d->SetCurrentLayer(d->imageViewInternalLayer);

}

