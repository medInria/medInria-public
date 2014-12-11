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

    medAbstractImageView *view;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medBoolParameter *enableWindowLevelParameter;
    medIntParameter *slicingParameter;

    medDoubleParameter *windowParameter;
    medDoubleParameter *levelParameter;

    QMap <QString,QString> presetToLut;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
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
          SetViewInput<itk::Image<itk::Vector<unsigned char,3>,3> >("itkDataImageVectorUChar3", data, d->view->layer(data))))
    {
        qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
        return;
    }

    d->view2d->GetImageActor(d->view2d->GetCurrentLayer())->GetProperty()->SetInterpolationTypeToCubic();
    initParameters(d->imageData);

    d->view2d->GetInput()->Update();
    double* range = d->view2d->GetInput(d->view->layer(d->imageData))->GetScalarRange();
    this->initWindowLevelParameters(range);
}

void medVtkViewItkDataImageInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->imageData));
    d->view3d->RemoveLayer(d->view->layer(d->imageData));
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

void medVtkViewItkDataImageInteractor::initWindowLevelParameters(double *range)
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

    d->windowParameter = new medDoubleParameter("Window", this);
    connect(d->windowParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindow(double)));
    d->windowParameter->setRange(windowMin, windowMax);
    d->windowParameter->setSingleStep(qMin(0.1,(windowMax-windowMin) / 1000));
    d->windowParameter->setValue(window);

    d->levelParameter = new medDoubleParameter("Level", this);
    connect(d->levelParameter, SIGNAL(valueChanged(double)), this, SLOT(setLevel(double)));
    d->levelParameter->setRange(levelMin, levelMax);
    d->levelParameter->setSingleStep(qMin(0.1,(levelMax-levelMin) / 1000));
    d->levelParameter->setValue(level);

    d->view->render();
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
    d->lutParam->setValue(d->presetToLut[preset]);

    if ( preset == "None" )
    {
        double *range = d->view2d->GetInput(d->view->layer(d->imageData))->GetScalarRange();
        d->windowParameter->setValue(range[1]-range[0]);
        d->levelParameter->setValue(0.5*(range[1]+range[0]));
    }
    else if ( preset == "VR Muscles&Bones" )
    {
        d->windowParameter->setValue(337.0);
        d->levelParameter->setValue(1237.0);
    }
    else if ( preset == "Vascular I" )
    {
        d->windowParameter->setValue(388.8);
        d->levelParameter->setValue(362.9);
    }
    else if ( preset == "Vascular II" )
    {
        d->windowParameter->setValue(189.6);
        d->levelParameter->setValue(262.3);
    }
    else if ( preset == "Vascular III" )
    {
        d->windowParameter->setValue(284.4);
        d->levelParameter->setValue(341.7);
    }
    else if ( preset == "Vascular IV" )
    {
        d->windowParameter->setValue(272.5);
        d->levelParameter->setValue(310.9);
    }
    else if ( preset == "Standard" )
    {
        d->windowParameter->setValue(243.7);
        d->levelParameter->setValue(199.6);
    }
    else if ( preset == "Glossy" )
    {
        d->windowParameter->setValue(133.5);
        d->levelParameter->setValue(163.4);
    }

    update();
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
    bool needsUpdate = false;

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

    if (needsUpdate)
        this->update();
}

void medVtkViewItkDataImageInteractor::setLevel(double level)
{
    bool needsUpdate = false;
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

    if (needsUpdate)
        this->update();
}

void medVtkViewItkDataImageInteractor::setWindowLevel(QHash<QString,QVariant> values)
{
    if(values.size() != 2 )
    {
        qWarning() << "Window/Level parameters are incorrect";
        return;
    }

    bool needUpdate = false;
    if (d->view2d->GetColorWindow(d->view->layer(d->imageData)) != values["Window"])
    {
        double w = values["Window"].toDouble();
        d->view2d->SetColorWindow(w, d->view->layer(d->imageData));

        if (d->view->is2D())
            needUpdate = true;
    }

    if (d->view3d->GetColorWindow(d->view->layer(d->imageData)) != values["Window"])
    {
        double w = values["Window"].toDouble();
        d->view3d->SetColorWindow(w, d->view->layer(d->imageData));

        if (!d->view->is2D())
            needUpdate = true;
    }

    if (d->view2d->GetColorLevel(d->view->layer(d->imageData)) != values["Level"])
    {
        double l = values["Level"].toDouble();
        d->view2d->SetColorLevel(l, d->view->layer(d->imageData));

        if (d->view->is2D())
            needUpdate = true;
    }

    if (d->view3d->GetColorLevel(d->view->layer(d->imageData)) != values["Level"])
    {
        double l = values["Level"].toDouble();
        d->view3d->SetColorLevel(l, d->view->layer(d->imageData));

        if (!d->view->is2D())
            needUpdate = true;
    }

    if(needUpdate)
        this->update();

    d->levelParameter->setValue(values["Level"].toDouble());
    d->windowParameter->setValue(values["Window"].toDouble());
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
