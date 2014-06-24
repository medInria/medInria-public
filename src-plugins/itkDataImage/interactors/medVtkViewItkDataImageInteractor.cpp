/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewItkDataImageInteractor.h>

#include <QVTKWidget.h>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>

#include <medVtkViewBackend.h>
#include <medAbstractData.h>
#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>
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
    medAbstractImageData *imageData;
    vtkRenderWindow *render;

    medAbstractImageView *view;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medBoolParameter *enableWindowLevelParameter;
    medIntParameter *slicingParameter;

    medDoubleParameter *windowParameter;
    medDoubleParameter *levelParameter;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->imageData = NULL;

    d->lutParam = NULL;
    d->presetParam = NULL;
    d->slicingParameter = NULL;
    d->enableWindowLevelParameter = NULL;
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
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkDataImageInteractor>("medVtkViewItkDataImageInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medVtkViewItkDataImageInteractor::dataHandled());
}

QList<medAbstractParameter*> medVtkViewItkDataImageInteractor::linkableParameters()
{
    QList<medAbstractParameter*> params;
    params.append(d->lutParam);
    params.append(d->presetParam);
    params.append(this->visibilityParameter());
    params.append(this->windowLevelParameter());
    params.append(this->opacityParameter());

    return params;
}

QList<medBoolParameter*> medVtkViewItkDataImageInteractor::mouseInteractionParameters()
{
    QList<medBoolParameter*> params;
    params.append(d->enableWindowLevelParameter);

    return params;
}

void medVtkViewItkDataImageInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if (!(SetViewInput<itk::Image<char,3> >("itkDataImageChar3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned char,3> >("itkDataImageUChar3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<short,3> >("itkDataImageShort3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned short,3> >("itkDataImageUShort3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<int,3> >("itkDataImageInt3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned,3> >("itkDataImageUInt3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<long,3> >("itkDataImageLong3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned long,3> >("itkDataImageULong3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<float,3> >("itkDataImageFloat3", data , d->view->layer(data)) ||
          SetViewInput<itk::Image<double,3> >("itkDataImageDouble3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::RGBPixel<unsigned char>,3> >("itkDataImageRGB3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::RGBAPixel<unsigned char>,3> >("itkDataImageRGBA3", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::Vector<unsigned char,3>,3> >("itkDataImageVector3", data, d->view->layer(data))))
    {
        qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
        return;
    }

    d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
    initParameters(d->imageData);
}

void medVtkViewItkDataImageInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->imageData));
    d->view3d->RemoveLayer(d->view->layer(d->imageData));
    if(d->view->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}


template <typename IMAGE>
bool medVtkViewItkDataImageInteractor::SetViewInput(const char* type, medAbstractData* data, int layer)
{
    if (data->identifier() != type)
        return false;

    if (IMAGE* image = dynamic_cast<IMAGE*>((itk::Object*)(data->data())))
    {
        d->view2d->SetITKInput(image, layer);
        d->view3d->SetITKInput(image, layer);
        return true;
    }
    return false;
}

void medVtkViewItkDataImageInteractor::initParameters(medAbstractImageData* data)
{
    d->imageData = data;

    d->lutParam = new medStringListParameter("Lut", this);
    QStringList luts = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    foreach(QString lut, luts)
        d->lutParam->addItem(lut);

    connect(d->lutParam, SIGNAL(valueChanged(QString)), this, SLOT(setLut(QString)));


    d->presetParam = new medStringListParameter("Preset", this);
    QStringList presets = QStringList() << "None" << "VR Muscles&Bones" << "Vascular I"
                                        << "Vascular II" << "Vascular III" << "Vascular IV"
                                        << "Standard" << "Soft" << "Soft on White"
                                        << "Soft on Blue" << "Red on White" << "Glossy" ;
    foreach(QString preset, presets)
        d->presetParam->addItem(preset);

    connect(d->presetParam, SIGNAL(valueChanged(QString)), this, SLOT(setPreset(QString)));

    if(d->view->layer(data) > 0)
        this->opacityParameter()->setValue(0.5);
    else
       this->opacityParameter()->setValue(1);

    d->view2d->GetInput()->Update();
    double* range = d->view2d->GetInput(d->view->layer(data))->GetScalarRange();
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    this->windowLevelParameter()->addVariant("Window", QVariant(window), QVariant(range[0]), QVariant(range[1] + 2 * level));
    this->windowLevelParameter()->addVariant("Level", QVariant(level), QVariant(range[0]), QVariant(range[1] + 2 * level));

    d->windowParameter = new medDoubleParameter("Window", this);
    connect(d->windowParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindow(double)));
    d->windowParameter->setRange(range[0], range[1] + 2 * level);
    d->windowParameter->setValue(window);

    d->levelParameter = new medDoubleParameter("Level", this);
    connect(d->levelParameter, SIGNAL(valueChanged(double)), this, SLOT(setLevel(double)));
    d->levelParameter->setRange(range[0], range[1] + 2 * level);
    d->levelParameter->setValue(level);

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

    d->enableWindowLevelParameter = new medBoolParameter("Windowing", this);
    d->enableWindowLevelParameter->setIcon(QIcon (":/icons/wlww.png"));
    d->enableWindowLevelParameter->setToolTip (tr("Windowing"));
    connect(d->enableWindowLevelParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableWIndowLevel(bool)));

    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)),
            this, SLOT(updateSlicingParam()));
    connect(d->view, SIGNAL(currentLayerChanged()), this, SLOT(updateImageViewInternalLayer()));

    if(d->view->layer(d->imageData) == 0)
    {
        switch(d->view2d->GetViewOrientation())
        {
        case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
            break;
        }
    }
}

void medVtkViewItkDataImageInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, d->view->layer(d->imageData));
    d->view2d->SetOpacity (opacity, d->view->layer(d->imageData));

    update();
}

void medVtkViewItkDataImageInteractor::setVisibility(bool visible)
{
    if(visible)
    {
        d->view2d->SetVisibility(1, d->view->layer(d->imageData));
        d->view3d->SetVisibility(1, d->view->layer(d->imageData));
    }
    else
    {
        d->view2d->SetVisibility(0, d->view->layer(d->imageData));
        d->view3d->SetVisibility(0, d->view->layer(d->imageData));
    }

    update();
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

    if (d->view->layer(d->imageData) == 0)
        d->view2d->SetTransferFunctions(rgb, alpha, d->view->layer(d->imageData));
    else
    {
        //TODO: find out why its not the same process - RDE
        vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
        d->view2d->SetLookupTable(lut, d->view->layer(d->imageData));
        lut->Delete();
    }

    d->view3d->SetTransferFunctions(rgb, alpha, d->view->layer(d->imageData));
    d->view3d->SetLookupTable(vtkLookupTableManager::GetLookupTable(value.toStdString()), d->view->layer(d->imageData));

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


QWidget* medVtkViewItkDataImageInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QWidget* medVtkViewItkDataImageInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    QHBoxLayout *wLayout = new QHBoxLayout;
    d->windowParameter->getSlider()->setOrientation(Qt::Horizontal);
    wLayout->addWidget(d->windowParameter->getSlider());
    wLayout->addWidget(d->windowParameter->getSpinBox());

    QHBoxLayout *lLayout = new QHBoxLayout;
    d->levelParameter->getSlider()->setOrientation(Qt::Horizontal);
    lLayout->addWidget(d->levelParameter->getSlider());
    lLayout->addWidget(d->levelParameter->getSpinBox());

    layout->addRow(d->windowParameter->getLabel(), wLayout);
    layout->addRow(d->levelParameter->getLabel(), lLayout);
    layout->addRow(d->lutParam->getLabel(), d->lutParam->getComboBox());
    layout->addRow(d->presetParam->getLabel(), d->presetParam->getComboBox());

    return toolbox;
}

QWidget* medVtkViewItkDataImageInteractor::buildLayerWidget()
{
        this->opacityParameter()->getSlider()->setOrientation(Qt::Horizontal);
        return this->opacityParameter()->getSlider();
}

void medVtkViewItkDataImageInteractor::setWindow(double window)
{
    if(d->view2d->GetColorWindow(d->view->layer(d->imageData)) == window)
        return;

    d->view2d->SetColorWindow(window, d->view->layer(d->imageData));
    d->view3d->SetColorWindow(window, d->view->layer(d->imageData));

    this->update();
}

void medVtkViewItkDataImageInteractor::setLevel(double level)
{
    if(d->view2d->GetColorLevel(d->view->layer(d->imageData)) == level)
        return;

    d->view2d->SetColorLevel(level, d->view->layer(d->imageData));
    d->view3d->SetColorLevel(level, d->view->layer(d->imageData));

    this->update();
}

void medVtkViewItkDataImageInteractor::setWindowLevel(QList<QVariant> values)
{
    if(values.size() != 2 )
    {
        qWarning() << "Window/Level parameters are incorrect";
        return;
    }

    bool needUpdate = false;
    if (d->view2d->GetColorWindow(d->view->layer(d->imageData)) != values.at(0))
    {
        double w = values.at(0).toDouble();
        d->view2d->SetColorWindow(w, d->view->layer(d->imageData));
        d->view3d->SetColorWindow(w, d->view->layer(d->imageData));
        needUpdate = true;
    }
    if (d->view2d->GetColorLevel(d->view->layer(d->imageData)) != values.at(1))
    {
        double l = values.at(1).toDouble();
        d->view2d->SetColorLevel(l, d->view->layer(d->imageData));
        d->view3d->SetColorLevel(l, d->view->layer(d->imageData));
        needUpdate = true;
    }

    if(needUpdate)
        this->update();

    d->levelParameter->setValue(values.at(1).toDouble());
    d->windowParameter->setValue(values.at(0).toDouble());
}

void medVtkViewItkDataImageInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
     }
 }

void medVtkViewItkDataImageInteractor::update()
{
    if(d->view->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}

void medVtkViewItkDataImageInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medVtkViewItkDataImageInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}


void medVtkViewItkDataImageInteractor::updateImageViewInternalLayer()
{
    if( d->view->layer(d->imageData) != d->view->currentLayer() )
        return;

    d->view2d->SetCurrentLayer(d->view->layer(d->imageData));
    d->view3d->SetCurrentLayer(d->view->layer(d->imageData));
}

void medVtkViewItkDataImageInteractor::updateSlicingParam()
{
    if(!d->view->is2D())
        return;

    //TODO Should be set according to the real number of slice of this data and
    // not according to vtkInria (ie. first layer droped) - RDE

    // slice orientation may differ from view orientation. Adapt slider range accordingly.
//    int orientationId = d->view2d->GetSliceOrientation();
//    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
//        d->slicingParameter->setRange(0, d->imageData->zDimension()-1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
//        d->slicingParameter->setRange (0, d->imageData->yDimension()-1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
//        d->slicingParameter->setRange (0, d->imageData->xDimension()-1);

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}

void medVtkViewItkDataImageInteractor::enableWIndowLevel(bool enable)
{
    if(enable)
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeWindowLevel );

}
