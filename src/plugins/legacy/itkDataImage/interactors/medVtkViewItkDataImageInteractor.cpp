/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

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
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkImageProperty.h>

#include <medVtkViewBackend.h>
#include <medAbstractData.h>
#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medStringListParameterL.h>
#include <medIntParameterL.h>
#include <medBoolParameterL.h>
#include <medDoubleParameterL.h>
#include <medVector3DParameterL.h>
#include <medAbstractImageData.h>
#include <medCompositeParameterL.h>

#include <QFormLayout>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QComboBox>

#include <vtkItkConversion.h>

class medVtkViewItkDataImageInteractorPrivate
{
public:
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractImageData *imageData;

    medAbstractImageView *view;

    medStringListParameterL *lutParam;
    medStringListParameterL *presetParam;
    medBoolParameterL *enableWindowLevelParameter;
    medIntParameterL *slicingParameter;

    medDoubleParameterL *minIntensityParameter;
    medDoubleParameterL *maxIntensityParameter;

    medBoolParameterL *enableInterpolation;

    QMap <QString,QString> presetToLut;

    bool isFloatImage;
    double intensityStep;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->imageData = nullptr;

    d->lutParam = nullptr;
    d->presetParam = nullptr;
    d->slicingParameter = nullptr;
    d->enableWindowLevelParameter = nullptr;

    d->enableInterpolation = nullptr;

    d->isFloatImage = false;
    d->intensityStep = 0;
    m_poConv = nullptr;
}

medVtkViewItkDataImageInteractor::~medVtkViewItkDataImageInteractor()
{
    delete m_poConv;
    delete d;
    d = nullptr;
}

QStringList medVtkViewItkDataImageInteractor::dataHandled()
{
    QStringList d = QStringList() << "itkDataImageChar3"
                                  << "itkDataImageUChar3"
                                  << "itkDataImageShort3"
                                  << "itkDataImageUShort3"
                                  << "itkDataImageInt3"
                                  << "itkDataImageUInt3"
                                  << "itkDataImageLong3"
                                  << "itkDataImageULong3"
                                  << "itkDataImageFloat3"
                                  << "itkDataImageDouble3"
                                  << "itkDataImageRGB3"
                                  << "itkDataImageRGBA3"
                                  << "itkDataImageVectorUChar3";
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

QList<medAbstractParameterL*> medVtkViewItkDataImageInteractor::linkableParameters()
{
    QList<medAbstractParameterL*> params;
    params.append(d->lutParam);
    params.append(d->slicingParameter);
    params.append(d->presetParam);
    params.append(this->visibilityParameter());
    params.append(this->windowLevelParameter());
    params.append(d->minIntensityParameter);
    params.append(d->maxIntensityParameter);
    params.append(this->opacityParameter());
    params.append(d->enableInterpolation);

    return params;
}

QList<medBoolParameterL*> medVtkViewItkDataImageInteractor::mouseInteractionParameters()
{
    QList<medBoolParameterL*> params;
    params.append(d->enableWindowLevelParameter);

    return params;
}

void medVtkViewItkDataImageInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(d->imageData)
    {
        if (!SetViewInput (data, d->view->layer(data)))
        {
            qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
            return;
        }

        if( d->imageData->PixelType() == typeid(double) || d->imageData->PixelType() == typeid(float) )
        {
            d->isFloatImage = true;
        }

        initParameters(d->imageData);

        double* range = d->view2d->GetScalarRange(getCurrentImageDataLayer());
        this->initWindowLevelParameters(range);
    }
}

void medVtkViewItkDataImageInteractor::removeData()
{
    unsigned int imageLayer = getCurrentImageDataLayer();
    d->view2d->RemoveLayer(imageLayer);
    d->view3d->RemoveLayer(imageLayer);
}

bool medVtkViewItkDataImageInteractor::SetViewInput(medAbstractData* data, int layer)
{
    bool bRes = true;

    m_poConv = vtkItkConversionInterface::createInstance(data);

    if (m_poConv)
    {
        itk::DataObject::Pointer image = (itk::DataObject*)(data->data());
        vtkAlgorithmOutput *poVtkAlgoOutputPort = nullptr;
        vtkMatrix4x4 *poMatrix = nullptr;
      
        bRes = m_poConv->SetITKInput(image);
        if (bRes)
        {
            bRes = m_poConv->GetConversion(poVtkAlgoOutputPort, poMatrix);
            if (bRes)
            {
                d->view2d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                d->view3d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
            }
            poMatrix->Delete();
        }
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

void medVtkViewItkDataImageInteractor::initParameters(medAbstractImageData* data)
{
    d->imageData = data; //used among others for 4D
    d->lutParam = new medStringListParameterL("Lut", this);
    QStringList luts = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni Inversed" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body" << "Binary Map";
    for(QString lut : luts)
    {
        d->lutParam->addItem(lut);
    }
    connect(d->lutParam, SIGNAL(valueChanged(QString)), this, SLOT(setLut(QString)));

    d->presetParam = new medStringListParameterL("Preset", this);
    QStringList presets = QStringList() << "None" << "VR Muscles&Bones" << "Vascular I"
                                        << "Vascular II" << "Vascular III" << "Vascular IV"
                                        << "Standard" << "Glossy" ;
    for(QString preset : presets)
    {
        d->presetParam->addItem(preset);
    }

    if(d->presetToLut.isEmpty())
    {
        d->presetToLut.insert("None", "Black & White");
        d->presetToLut.insert("VR Muscles&Bones", "Muscles & Bones");
        d->presetToLut.insert("Vascular I", "Stern");
        d->presetToLut.insert("Vascular II", "Red Vessels");
        d->presetToLut.insert("Vascular III", "Red Vessels");
        d->presetToLut.insert("Vascular IV", "Red Vessels");
        d->presetToLut.insert("Standard", "Muscles & Bones");
        d->presetToLut.insert("Glossy", "Bones");
    }

    connect(d->presetParam, SIGNAL(valueChanged(QString)), this, SLOT(setPreset(QString)));

    if(d->view->layer(data) > 0)
    {
        this->opacityParameter()->setValue(0.5);
    }
    else
    {
        this->opacityParameter()->setValue(1);
    }

    createSlicingParam();

    d->enableWindowLevelParameter = new medBoolParameterL("Windowing", this);
    d->enableWindowLevelParameter->setIcon(QIcon::fromTheme("contrast"));
    d->enableWindowLevelParameter->setToolTip (tr("Windowing"));
    connect(d->enableWindowLevelParameter, SIGNAL(valueChanged(bool)), this, SLOT(enableWindowLevel(bool)));

    d->enableInterpolation = new medBoolParameterL("Interpolate", this);
    d->enableInterpolation->setToolTip("Active interpolation\n shortcut is :\nkey 'n'");
    d->enableInterpolation->setValue(true);
    connect(d->enableInterpolation, SIGNAL(valueChanged(bool)), this, SLOT(interpolation(bool)));
    connect(d->view2d->qtSignalHandler, SIGNAL(interpolate(bool, int)), this, SLOT(updateInterpolateStatus(bool, int)));

    connect(d->view, SIGNAL(currentLayerChanged()), this, SLOT(updateImageViewInternalLayer()));

    if(getCurrentImageDataLayer() == 0)
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

void medVtkViewItkDataImageInteractor::initWindowLevelParameters(double *range)
{
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    double windowMin = 0;
    double windowMax = 2.0 * window;
    double halfWidth = 0.5 * window;
    double levelMin = range[0] - halfWidth;
    double levelMax = range[1] + halfWidth;

    d->intensityStep = (levelMax - levelMin) / 100;

    this->windowLevelParameter()->addVariant("Window", QVariant(window), QVariant(windowMin), QVariant(windowMax));
    this->windowLevelParameter()->addVariant("Level",  QVariant(level),  QVariant(levelMin),  QVariant(levelMax));

    d->minIntensityParameter = new medDoubleParameterL("Min Intensity", this);
    d->maxIntensityParameter = new medDoubleParameterL("Max Intensity", this);

    d->minIntensityParameter->setSingleStep(d->intensityStep);
    d->maxIntensityParameter->setSingleStep(d->intensityStep);

    if(d->isFloatImage)
    {
        int iDecimalCount = 2;
        if(d->intensityStep<1)
        {
            iDecimalCount = 1 + std::ceil(std::fabs(std::log10(static_cast<long double>(d->intensityStep))));
        }
        d->minIntensityParameter->setDecimals(iDecimalCount);
        d->maxIntensityParameter->setDecimals(iDecimalCount);
    }
    else
    {
        d->minIntensityParameter->setDecimals(0);
        d->maxIntensityParameter->setDecimals(0);
    }

    d->minIntensityParameter->setRange(levelMin, levelMax);
    d->maxIntensityParameter->setRange(levelMin, levelMax);
    d->minIntensityParameter->setValue(range[0]);
    d->maxIntensityParameter->setValue(range[1]);

    connect(d->minIntensityParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindowLevelFromMinMax()));
    connect(d->maxIntensityParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindowLevelFromMinMax()));
}

void medVtkViewItkDataImageInteractor::setOpacity(double opacity)
{
    unsigned int imageLayer = getCurrentImageDataLayer();

    d->view3d->SetOpacity (opacity, imageLayer);
    d->view2d->SetOpacity (opacity, imageLayer);

    update();
}

void medVtkViewItkDataImageInteractor::setVisibility(bool visible)
{
    unsigned int imageLayer = getCurrentImageDataLayer();

    if(visible)
    {
        d->view2d->SetVisibility(1, imageLayer);
        d->view3d->SetVisibility(1, imageLayer);
    }
    else
    {
        d->view2d->SetVisibility(0, imageLayer);
        d->view3d->SetVisibility(0, imageLayer);
    }

    update();
}

QString medVtkViewItkDataImageInteractor::lut() const
{
    return d->lutParam->value();
}

void medVtkViewItkDataImageInteractor::setLut(QString value)
{
    unsigned int imageLayer = getCurrentImageDataLayer();

    typedef vtkTransferFunctionPresets Presets;
    vtkColorTransferFunction * rgb   = vtkColorTransferFunction::New();
    vtkPiecewiseFunction     * alpha = vtkPiecewiseFunction::New();

    Presets::GetTransferFunction(value.toStdString(), rgb, alpha );

    vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
    d->view3d->SetTransferFunctions(rgb, alpha, imageLayer);
    d->view3d->SetLookupTable(lut, imageLayer);

    if (imageLayer == 0)
    {
        lut = vtkLookupTableManager::removeLUTAlphaChannel(lut);
    }
    d->view2d->SetLookupTable(lut, imageLayer);

    rgb->Delete();
    alpha->Delete();
    lut->Delete();

    update();
}

void medVtkViewItkDataImageInteractor::setPreset(QString preset)
{
    if(!d->presetToLut[preset].isEmpty())
      d->lutParam->setValue(d->presetToLut[preset]);

    QHash <QString, QVariant> wl;

    if ( preset == "None" )
    {
        double *range = d->view2d->GetScalarRange(getCurrentImageDataLayer());
        wl["Window"] = QVariant(range[1]-range[0]);
        wl["Level"] = QVariant(0.5*(range[1]+range[0]));
        setWindowLevel(wl);
    }
    else if ( preset == "VR Muscles&Bones" )
    {
        wl["Window"] = QVariant(337.0);
        wl["Level"] = QVariant(1237.0);
        setWindowLevel(wl);
    }
    else if ( preset == "Vascular I" )
    {
        wl["Window"] = QVariant(388.8);
        wl["Level"] = QVariant(362.9);
        setWindowLevel(wl);
    }
    else if ( preset == "Vascular II" )
    {
        wl["Window"] = QVariant(189.6);
        wl["Level"] = QVariant(262.3);
        setWindowLevel(wl);
    }
    else if ( preset == "Vascular III" )
    {
        wl["Window"] = QVariant(284.4);
        wl["Level"] = QVariant(341.7);
        setWindowLevel(wl);
    }
    else if ( preset == "Vascular IV" )
    {
        wl["Window"] = QVariant(272.5);
        wl["Level"] = QVariant(310.9);
        setWindowLevel(wl);
    }
    else if ( preset == "Standard" )
    {
        wl["Window"] = QVariant(243.7);
        wl["Level"] = QVariant(199.6);
        setWindowLevel(wl);
    }
    else if ( preset == "Glossy" )
    {
        wl["Window"] = QVariant(133.5);
        wl["Level"] = QVariant(163.4);
        setWindowLevel(wl);
    }

    update();
}

QString medVtkViewItkDataImageInteractor::preset() const
{
  return d->presetParam->value();
}

void medVtkViewItkDataImageInteractor::setMinIntensity(double min)
{
    d->minIntensityParameter->setValue(min);
}

void medVtkViewItkDataImageInteractor::setMaxIntensity(double max)
{
    d->maxIntensityParameter->setValue(max);
}

QWidget* medVtkViewItkDataImageInteractor::buildToolBarWidget()
{
    QWidget* toolbar = new QWidget();
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(5, 0, 0, 0);
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    toolbarLayout->addWidget(d->slicingParameter->getSlider());
    return toolbar;
}

QWidget* medVtkViewItkDataImageInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    QHBoxLayout *minLayout = new QHBoxLayout;
    QHBoxLayout *maxLayout = new QHBoxLayout;

    d->minIntensityParameter->getSlider()->setOrientation(Qt::Horizontal);
    minLayout->addWidget(d->minIntensityParameter->getSlider());
    minLayout->addWidget(d->minIntensityParameter->getSpinBox());

    d->maxIntensityParameter->getSlider()->setOrientation(Qt::Horizontal);
    maxLayout->addWidget(d->maxIntensityParameter->getSlider());
    maxLayout->addWidget(d->maxIntensityParameter->getSpinBox());

    layout->addRow(d->minIntensityParameter->getLabel(), minLayout);
    layout->addRow(d->maxIntensityParameter->getLabel(), maxLayout);
    layout->addRow(d->lutParam->getLabel(), d->lutParam->getComboBox());
    layout->addRow(d->presetParam->getLabel(), d->presetParam->getComboBox());
    layout->addRow(d->enableInterpolation->getLabel(), d->enableInterpolation->getWidget());

    return toolbox;
}

QWidget* medVtkViewItkDataImageInteractor::buildLayerWidget()
{
    this->opacityParameter()->getSlider()->setOrientation(Qt::Horizontal);
    return this->opacityParameter()->getSlider();
}

void medVtkViewItkDataImageInteractor::setWindowLevelFromMinMax()
{
    medDoubleParameterL *sender = dynamic_cast<medDoubleParameterL *>(this->sender());

    if(sender)
    {
        double minIntensityValue = d->minIntensityParameter->value();
        double maxIntensityValue = d->maxIntensityParameter->value();

        if( sender == d->minIntensityParameter && minIntensityValue >= maxIntensityValue )
        {
            // If the min parameter is equal or higher than the max parameter,
            // the max parameter is increased
            d->maxIntensityParameter->blockSignals(true);
            d->maxIntensityParameter->setValue(minIntensityValue + d->intensityStep);
            d->maxIntensityParameter->blockSignals(false);
        }
        else if( sender == d->maxIntensityParameter && maxIntensityValue <= minIntensityValue )
        {
            // If the max parameter is equal or lower than the min parameter,
            // the min parameter is increased
            d->minIntensityParameter->blockSignals(true);
            d->minIntensityParameter->setValue(maxIntensityValue - d->intensityStep);
            d->minIntensityParameter->blockSignals(false);
        }

        double minIntensityValueUpdated = d->minIntensityParameter->value();
        double maxIntensityValueUpdated = d->maxIntensityParameter->value();

        double level = 0.5 * (maxIntensityValueUpdated - minIntensityValueUpdated) + minIntensityValueUpdated;
        double window = maxIntensityValueUpdated - minIntensityValueUpdated;

        unsigned int imageLayer = getCurrentImageDataLayer();

        //--- block
        this->windowLevelParameter()->blockSignals(true);

        // Call function from vtkImageView shared by view2d and view3d
        d->view2d->SetColorWindowLevel(window, level, imageLayer);
        d->view3d->SetColorWindowLevel(window, level, imageLayer);

        this->windowLevelParameter()->blockSignals(false);
        //--- unblock
    }
}

void medVtkViewItkDataImageInteractor::updateInterpolateStatus(bool pi_bStatus, int pi_iLayer)
{
    if (d->imageData && (pi_iLayer == static_cast<int>(getCurrentImageDataLayer())))
    {
        d->enableInterpolation->setValue(pi_bStatus);
    }
}

void medVtkViewItkDataImageInteractor::setWindowLevel(QHash<QString, QVariant> values)
{
    if(values.size() != 2 )
    {
        qWarning() << "Window/Level parameters are incorrect";
        return;
    }

    double w = values["Window"].toDouble();
    double l = values["Level"].toDouble();
    if(w != w || l != l) // NaN values
    {
        return;
    }

    unsigned int imageLayer = getCurrentImageDataLayer();

    if (d->view2d->GetColorWindow(imageLayer) != w)
    {
        d->view2d->SetColorWindow(w, imageLayer);
    }

    if (d->view2d->GetColorLevel(imageLayer) != l)
    {
        d->view2d->SetColorLevel(l, imageLayer);
    }

    d->view3d->SetColorWindowLevel( w,  l,  imageLayer);

    //--- block
    d->minIntensityParameter->blockSignals(true);
    d->maxIntensityParameter->blockSignals(true);

    d->minIntensityParameter->setValue( l - 0.5*w );
    d->maxIntensityParameter->setValue( l + 0.5*w );

    d->minIntensityParameter->blockSignals(false);
    d->maxIntensityParameter->blockSignals(false);
    //--- unblock
}

void medVtkViewItkDataImageInteractor::moveToSlice(int slice)
{
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
     }
 }

void medVtkViewItkDataImageInteractor::update()
{
    if(d->view->is2D())
    {
        d->view2d->Render();
    }
    else
    {
        d->view3d->Render();
    }
}

void medVtkViewItkDataImageInteractor::updateWidgets()
{
    if (!d->view->is2D())
    {
        d->slicingParameter->getSlider()->setEnabled(false);
        d->enableInterpolation->hide();
    }
    else
    {
        d->enableInterpolation->show();

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
    unsigned int imageLayer = d->view->layer(d->imageData);

    if( imageLayer == d->view->currentLayer() )
    {
        d->view2d->SetCurrentLayer(imageLayer);
        d->view3d->SetCurrentLayer(imageLayer);
    }
}

void medVtkViewItkDataImageInteractor::createSlicingParam()
{
    d->slicingParameter = new medIntParameterL("Slicing", this);

    // slice orientation may differ from view orientation. Adapt slider range accordingly.
    int orientationId = d->view2d->GetSliceOrientation();
    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
    {
        d->slicingParameter->setRange(0, d->imageData->zDimension()-1);
    }
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
    {
        d->slicingParameter->setRange (0, d->imageData->yDimension()-1);
    }
    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
    {
        d->slicingParameter->setRange (0, d->imageData->xDimension()-1);
    }

    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)), Qt::UniqueConnection);
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()), Qt::UniqueConnection);
}

void medVtkViewItkDataImageInteractor::updateSlicingParam()
{
    if(d->view->is2D())
    {
        d->slicingParameter->blockSignals(true);
        d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
        d->slicingParameter->setValue(d->view2d->GetSlice());
        d->slicingParameter->blockSignals(false);
    }
}

void medVtkViewItkDataImageInteractor::enableWindowLevel(bool enable)
{
    if(enable)
    {
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeWindowLevel );
    }
}

void medVtkViewItkDataImageInteractor::interpolation(bool pi_bActive)
{
    d->view2d->SetInterpolate(pi_bActive, getCurrentImageDataLayer());
    d->view2d->Render();
}

unsigned int medVtkViewItkDataImageInteractor::getCurrentImageDataLayer()
{
   return d->view->layer(d->imageData);
}

void medVtkViewItkDataImageInteractor::restoreParameters(QHash<QString,QString> parameters)
{
    if(parameters.contains("Opacity"))
    {
        setOpacity(medDoubleParameterL::fromString(parameters["Opacity"]));
    }
    if(parameters.contains("Visibility"))
    {
        setVisibility(medBoolParameterL::fromString(parameters["Visibility"]));
    }
    if(parameters.contains("Preset"))
    {
        setPreset(medStringListParameterL::fromString(parameters["Preset"]));
    }
    if(parameters.contains("Lut"))
    {
        setLut(medStringListParameterL::fromString(parameters["Lut"]));
    }
    if(parameters.contains("Max Intensity"))
    {
        setMaxIntensity(medDoubleParameterL::fromString(parameters["Max Intensity"]));
    }
    if(parameters.contains("Min Intensity"))
    {
        setMinIntensity(medDoubleParameterL::fromString(parameters["Min Intensity"]));
    }
}

QString medVtkViewItkDataImageInteractor::name() const
{
    return "medVtkViewItkDataImageInteractor";
}
