/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewItkScalarImageInteractor.h>

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


class medVtkViewItkScalarImageInteractorPrivate
{
public:
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractImageData *imageData;

    medAbstractImageView *view;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medBoolParameter *enableWindowLevelParameter;
    medIntParameter *slicingParameter;

    medDoubleParameter *minIntensityParameter;
    medDoubleParameter *maxIntensityParameter;

    QMap <QString,QString> presetToLut;

    bool isFloatImage;
    double intensityStep;
};


medVtkViewItkScalarImageInteractor::medVtkViewItkScalarImageInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkScalarImageInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->imageData = NULL;

    d->lutParam = NULL;
    d->presetParam = NULL;
    d->slicingParameter = NULL;
    d->enableWindowLevelParameter = NULL;

    d->isFloatImage = false;
    d->intensityStep = 0;
}

medVtkViewItkScalarImageInteractor::~medVtkViewItkScalarImageInteractor()
{
    delete d;
    d = NULL;
}

QStringList medVtkViewItkScalarImageInteractor::dataHandled()
{
    QStringList d = QStringList() << "medItkChar3ImageData"
                                  << "medItkUChar3ImageData"
                                  << "medItkShort3ImageData"
                                  << "medItkUShort3ImageData"
                                  << "medItkInt3ImageData"
                                  << "medItkUInt3ImageData"
                                  << "medItkLong3ImageData"
                                  << "medItkULong3ImageData"
                                  << "medItkFloat3ImageData"
                                  << "medItkDouble3ImageData"
                                  << "medItkRgb3ImageData"
                                  << "medItkRgba3ImageData";
    return  d;
}

QStringList medVtkViewItkScalarImageInteractor::handled() const
{
    return medVtkViewItkScalarImageInteractor::dataHandled();
}

QString medVtkViewItkScalarImageInteractor::description() const
{
    return tr("Interactor for ITK images");
}


QString medVtkViewItkScalarImageInteractor::identifier() const
{
    return "medVtkViewItkScalarImageInteractor";
}

bool medVtkViewItkScalarImageInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkScalarImageInteractor>("medVtkViewItkScalarImageInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medVtkViewItkScalarImageInteractor::dataHandled());
}

QList<medAbstractParameter*> medVtkViewItkScalarImageInteractor::linkableParameters()
{
    QList<medAbstractParameter*> params;
    params.append(d->lutParam);
    params.append(d->presetParam);
    params.append(this->visibilityParameter());
    params.append(this->windowLevelParameter());
    params.append(d->minIntensityParameter);
    params.append(d->maxIntensityParameter);
    params.append(this->opacityParameter());

    return params;
}

QList<medBoolParameter*> medVtkViewItkScalarImageInteractor::mouseInteractionParameters()
{
    QList<medBoolParameter*> params;
    params.append(d->enableWindowLevelParameter);

    return params;
}

void medVtkViewItkScalarImageInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);
    d->imageData = dynamic_cast<medAbstractImageData *>(data);
    if(!d->imageData)
        return;

    if (!(SetViewInput<itk::Image<char,3> >("medItkChar3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned char,3> >("medItkUChar3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<short,3> >("medItkShort3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned short,3> >("medItkUShort3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<int,3> >("medItkInt3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned,3> >("medItkUInt3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<long,3> >("medItkLong3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<unsigned long,3> >("medItkULong3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<float,3> >("medItkFloat3ImageData", data , d->view->layer(data)) ||
          SetViewInput<itk::Image<double,3> >("medItkDouble3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::RGBPixel<unsigned char>,3> >("medItkRgb3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::RGBAPixel<unsigned char>,3> >("medItkRgba3ImageData", data, d->view->layer(data)) ||
          SetViewInput<itk::Image<itk::Vector<unsigned char,3>,3> >("medItkImageDataVector3", data, d->view->layer(data))))
    {
        qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
        return;
    }

    if( d->imageData->PixelType() == typeid(double) || d->imageData->PixelType() == typeid(float) )
        d->isFloatImage = true;

    d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
    initParameters(d->imageData);

    d->view2d->GetInput()->Update();
    double* range = d->view2d->GetInput(d->view->layer(d->imageData))->GetScalarRange();
    this->initWindowLevelParameters(range);
}

void medVtkViewItkScalarImageInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->imageData));
    d->view3d->RemoveLayer(d->view->layer(d->imageData));
}


template <typename IMAGE>
bool medVtkViewItkScalarImageInteractor::SetViewInput(const char* type, medAbstractData* data, int layer)
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

void medVtkViewItkScalarImageInteractor::initParameters(medAbstractImageData* data)
{
    d->imageData = data;

    d->lutParam = new medStringListParameter("Lut", this);
    QStringList luts = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni Inversed" << "Loni 2"
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
                                        << "Standard" << "Glossy" ;
    foreach(QString preset, presets)
        d->presetParam->addItem(preset);

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
        this->opacityParameter()->setValue(0.5);
    else
       this->opacityParameter()->setValue(1);

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

void medVtkViewItkScalarImageInteractor::initWindowLevelParameters(double *range)
{
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    double windowMin = 0;
    double windowMax = 2.0 * window;
    double halfWidth = 0.5 * window;
    double levelMin = range[0] - halfWidth;
    double levelMax = range[1] + halfWidth;

    this->windowLevelParameter()->addVariant("Window", QVariant(window), QVariant(windowMin), QVariant(windowMax));
    this->windowLevelParameter()->addVariant("Level", QVariant(level), QVariant(levelMin), QVariant(levelMax));

    d->minIntensityParameter = new medDoubleParameter("Min Intensity", this);
    connect(d->minIntensityParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindowLevelFromMinMax()));
    d->minIntensityParameter->setRange(levelMin, levelMax);

    d->maxIntensityParameter = new medDoubleParameter("Max Intensity", this);
    connect(d->maxIntensityParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindowLevelFromMinMax()));
    d->maxIntensityParameter->setRange(levelMin, levelMax);

    d->minIntensityParameter->setValue(range[0]);
    d->maxIntensityParameter->setValue(range[1]);

    if(d->isFloatImage)
    {
        d->intensityStep = qMin(0.1,(levelMax-levelMin) / 1000);
        d->minIntensityParameter->setSingleStep(d->intensityStep);
        d->minIntensityParameter->setDecimals(6);
        d->maxIntensityParameter->setSingleStep(d->intensityStep);
        d->maxIntensityParameter->setDecimals(6);
    }
    else
    {
        d->intensityStep= 1;
        d->minIntensityParameter->setSingleStep(d->intensityStep);
        d->minIntensityParameter->setDecimals(0);
        d->maxIntensityParameter->setSingleStep(d->intensityStep);
        d->maxIntensityParameter->setDecimals(0);
    }

    d->view->render();
}

void medVtkViewItkScalarImageInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, d->view->layer(d->imageData));
    d->view2d->SetOpacity (opacity, d->view->layer(d->imageData));

    update();
}

void medVtkViewItkScalarImageInteractor::setVisibility(bool visible)
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

QString medVtkViewItkScalarImageInteractor::lut() const
{
    return d->lutParam->value();
}

void medVtkViewItkScalarImageInteractor::setLut(QString value)
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

void medVtkViewItkScalarImageInteractor::setPreset(QString preset)
{
    if(!d->presetToLut[preset].isEmpty())
      d->lutParam->setValue(d->presetToLut[preset]);

    QHash <QString, QVariant> wl;

    if ( preset == "None" )
    {
        double *range = d->view2d->GetInput(d->view->layer(d->imageData))->GetScalarRange();
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

QString medVtkViewItkScalarImageInteractor::preset() const
{
  return d->presetParam->value();
}


QWidget* medVtkViewItkScalarImageInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QWidget* medVtkViewItkScalarImageInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);

    QHBoxLayout *minLayout = new QHBoxLayout;
    d->minIntensityParameter->getSlider()->setOrientation(Qt::Horizontal);
    minLayout->addWidget(d->minIntensityParameter->getSlider());
    minLayout->addWidget(d->minIntensityParameter->getSpinBox());

    QHBoxLayout *maxLayout = new QHBoxLayout;
    d->maxIntensityParameter->getSlider()->setOrientation(Qt::Horizontal);
    maxLayout->addWidget(d->maxIntensityParameter->getSlider());
    maxLayout->addWidget(d->maxIntensityParameter->getSpinBox());

    layout->addRow(d->minIntensityParameter->getLabel(), minLayout);
    layout->addRow(d->maxIntensityParameter->getLabel(), maxLayout);
    layout->addRow(d->lutParam->getLabel(), d->lutParam->getComboBox());
    layout->addRow(d->presetParam->getLabel(), d->presetParam->getComboBox());

    return toolbox;
}

QWidget* medVtkViewItkScalarImageInteractor::buildLayerWidget()
{
        this->opacityParameter()->getSlider()->setOrientation(Qt::Horizontal);
        return this->opacityParameter()->getSlider();
}

void medVtkViewItkScalarImageInteractor::setWindowLevelFromMinMax()
{
    medDoubleParameter *sender = dynamic_cast<medDoubleParameter *>(this->sender());
    if(!sender)
        return;

    if( sender == d->minIntensityParameter && d->minIntensityParameter->value() >= d->maxIntensityParameter->value() )
    {
        d->maxIntensityParameter->blockSignals(true);
        d->maxIntensityParameter->setValue(d->minIntensityParameter->value() + d->intensityStep);
        d->maxIntensityParameter->blockSignals(false);
    }
    else if( sender == d->maxIntensityParameter && d->maxIntensityParameter->value() <= d->minIntensityParameter->value() )
    {
        d->minIntensityParameter->blockSignals(true);
        d->minIntensityParameter->setValue(d->maxIntensityParameter->value() - d->intensityStep);
        d->minIntensityParameter->blockSignals(false);
    }

    double level = 0.5 * (d->maxIntensityParameter->value() - d->minIntensityParameter->value()) + d->minIntensityParameter->value();
    double window = d->maxIntensityParameter->value() - d->minIntensityParameter->value();

    bool needsUpdate = false;

    this->windowLevelParameter()->blockSignals(true);

    if(d->view2d->GetColorWindow(d->view->layer(d->imageData)) != window)
    {
        d->view2d->SetColorWindow(window, d->view->layer(d->imageData));
        if (d->view->is2D())
            needsUpdate = true;
    }
    if(d->view3d->GetColorWindow(d->view->layer(d->imageData)) != window)
    {
        d->view3d->SetColorWindow(window, d->view->layer(d->imageData));
        if (!d->view->is2D())
            needsUpdate = true;
    }

    if(d->view2d->GetColorLevel(d->view->layer(d->imageData)) != level)
    {
        d->view2d->SetColorLevel(level, d->view->layer(d->imageData));
        if (d->view->is2D())
            needsUpdate = true;
    }
    if(d->view3d->GetColorLevel(d->view->layer(d->imageData)) != level)
    {
        d->view3d->SetColorLevel(level, d->view->layer(d->imageData));
        if (!d->view->is2D())
            needsUpdate = true;
    }

    this->windowLevelParameter()->blockSignals(false);

    if (needsUpdate)
        this->update();
}

void medVtkViewItkScalarImageInteractor::setWindowLevel(QHash<QString,QVariant> values)
{
    if(values.size() != 2 )
    {
        qWarning() << "Window/Level parameters are incorrect";
        return;
    }

    bool needUpdate = false;

    double w = values["Window"].toDouble();
    double l = values["Level"].toDouble();

    if (d->view2d->GetColorWindow(d->view->layer(d->imageData)) != w)
    {
        d->view2d->SetColorWindow(w, d->view->layer(d->imageData));

        if (d->view->is2D())
            needUpdate = true;
    }

    if (d->view3d->GetColorWindow(d->view->layer(d->imageData)) != w)
    {
        d->view3d->SetColorWindow(w, d->view->layer(d->imageData));

        if (!d->view->is2D())
            needUpdate = true;
    }

    if (d->view2d->GetColorLevel(d->view->layer(d->imageData)) != l)
    {
        d->view2d->SetColorLevel(l, d->view->layer(d->imageData));

        if (d->view->is2D())
            needUpdate = true;
    }

    if (d->view3d->GetColorLevel(d->view->layer(d->imageData)) != l)
    {
        d->view3d->SetColorLevel(l, d->view->layer(d->imageData));

        if (!d->view->is2D())
            needUpdate = true;
    }

    if(needUpdate)
        this->update();

    d->minIntensityParameter->blockSignals(true);
    d->maxIntensityParameter->blockSignals(true);

    d->minIntensityParameter->setValue( l - 0.5*w );
    d->maxIntensityParameter->setValue( l + 0.5*w );

    d->minIntensityParameter->blockSignals(false);
    d->maxIntensityParameter->blockSignals(false);
}

void medVtkViewItkScalarImageInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
     }
 }

void medVtkViewItkScalarImageInteractor::update()
{
    if(d->view->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}

void medVtkViewItkScalarImageInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medVtkViewItkScalarImageInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}


void medVtkViewItkScalarImageInteractor::updateImageViewInternalLayer()
{
    if( d->view->layer(d->imageData) != d->view->currentLayer() )
        return;

    d->view2d->SetCurrentLayer(d->view->layer(d->imageData));
    d->view3d->SetCurrentLayer(d->view->layer(d->imageData));
}

void medVtkViewItkScalarImageInteractor::updateSlicingParam()
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

void medVtkViewItkScalarImageInteractor::enableWIndowLevel(bool enable)
{
    if(enable)
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeWindowLevel );

}
