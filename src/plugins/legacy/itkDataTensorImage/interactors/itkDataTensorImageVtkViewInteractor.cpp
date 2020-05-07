/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageVtkViewInteractor.h>

#include <vtkTensorManager.h>
#include <vtkStructuredPoints.h>
#include <vtkPointSet.h>
#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

#include <itkITKTensorsToVTKTensorsFilter.h>
#include <itkImage.h>
#include <itkTensor.h>

#include <medAbstractData.h>
#include <medAbstractParameterL.h>
#include <medStringListParameterL.h>
#include <medIntParameterL.h>
#include <medBoolParameterL.h>
#include <medBoolGroupParameterL.h>
#include <medDoubleParameterL.h>
#include <medAbstractImageView.h>
#include <medViewFactory.h>
#include <medVtkViewBackend.h>

#include <QLabel>
#include <QSlider>
#include <QFormLayout>


typedef itk::Tensor<float, 3>    TensorTypeFloat;
typedef itk::Image<TensorTypeFloat, 3> TensorImageTypeFloat;
typedef TensorImageTypeFloat::Pointer TensorImagePointerFloat;

typedef itk::Tensor<double, 3>    TensorTypeDouble;
typedef itk::Image<TensorTypeDouble, 3> TensorImageTypeDouble;
typedef TensorImageTypeDouble::Pointer TensorImagePointerDouble;

class itkDataTensorImageVtkViewInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractData *data;
    vtkTensorManager *manager;
    vtkRenderWindow *render;
    vtkMatrix4x4 *orientationMatrix;

    // the filters will convert from itk tensor image format to vtkStructuredPoint (format handled by the tensor manager)
    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::Pointer filterFloat;
    TensorImagePointerFloat      datasetFloat;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::Pointer filterDouble;
    TensorImagePointerDouble      datasetDouble;

    int minorScaling;
    int majorScalingExponent;

    double imageBounds[6];

    QList <medAbstractParameterL*> parameters;

    medIntParameterL *slicingParameter;
    PropertySmartPointer actorProperty;

    template <typename TENSOR_IMAGE>
    void setVTKFilter(medAbstractData* d,typename itk::ITKTensorsToVTKTensorsFilter<TENSOR_IMAGE>::Pointer& filter)
    {
        TENSOR_IMAGE *dataset = static_cast<TENSOR_IMAGE *>(d->data());

        if (filter)
            filter->Delete();

        filter = itk::ITKTensorsToVTKTensorsFilter<TENSOR_IMAGE>::New();

        filter->SetInput(dataset);
        filter->UpdateOutputInformation();

        //  This line generates the vtkSHs, otherwise is not generated, even if the next filter
        //  in the pipeline is connected and Update() is called

        filter->Update();

        // we need to call this function because GetOutput() just returns the input
        vtkStructuredPoints *tensors = filter->GetVTKTensors();
        vtkMatrix4x4 *matrix = filter->GetDirectionMatrix();

        itk::ImageBase<3>::DirectionType directions = dataset->GetDirection();
        itk::ImageBase<3>::PointType origin = dataset->GetOrigin();
        orientationMatrix = vtkMatrix4x4::New();
        orientationMatrix->Identity();
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                orientationMatrix->SetElement (i, j, directions (i,j));
        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
            v_origin[i] = origin[i];
        v_origin[3] = 1.0;
        orientationMatrix->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
            orientationMatrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        double v_spacing[3];
        for (int i=0; i<3; i++)
            v_spacing[i] = dataset->GetSpacing()[i];

        manager->SetInput(tensors);
        manager->SetDirectionMatrix(matrix);

        manager->ResetPosition();

        manager->Update();

        data = d;

        if (view)
        {
            int* dim = manager->GetInput()->GetDimensions();
            view2d->SetInput(manager->GetTensorVisuManagerAxial()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
            view2d->SetInput(manager->GetTensorVisuManagerSagittal()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
            view2d->SetInput(manager->GetTensorVisuManagerCoronal()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
        }
    }
};

itkDataTensorImageVtkViewInteractor::itkDataTensorImageVtkViewInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent),
    d(new itkDataTensorImageVtkViewInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->orientationMatrix = 0;

    d->manager = vtkTensorManager::New();

    d->datasetFloat = nullptr;
    d->filterFloat = nullptr;

    d->datasetDouble = nullptr;
    d->filterDouble = nullptr;

    d->minorScaling = 1;
    d->majorScalingExponent = 3;

    // set default properties
    d->manager->SetGlyphShapeToLine();

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;

    // be careful not to forget setting the same renderer for the interactor and the view
    // otherwise a new renderer is created
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(), d->view3d->GetRenderer());

    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(changePosition(QVector3D)));

    d->slicingParameter = new medIntParameterL("Slicing", this);
}


itkDataTensorImageVtkViewInteractor::~itkDataTensorImageVtkViewInteractor()
{
    d->manager->Delete();
    d->orientationMatrix->Delete();
    delete d;
    d = nullptr;
}

void itkDataTensorImageVtkViewInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->data));
    d->manager->Delete();
}

QString itkDataTensorImageVtkViewInteractor::description() const
{
    return tr("Interactor helping display Tensors");
}

QString itkDataTensorImageVtkViewInteractor::identifier() const
{
    return "itkDataTensorImageVtkViewInteractor";
}

QStringList itkDataTensorImageVtkViewInteractor::handled() const
{
    return itkDataTensorImageVtkViewInteractor::dataHandled();
}

QStringList itkDataTensorImageVtkViewInteractor::dataHandled()
{
    QStringList datahandled = QStringList() << "itkDataTensorImageDouble3" << "itkDataTensorImageFloat3";
    return  datahandled;
}


bool itkDataTensorImageVtkViewInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<itkDataTensorImageVtkViewInteractor>("itkDataTensorImageVtkViewInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     itkDataTensorImageVtkViewInteractor::dataHandled());
    return true;
}

void itkDataTensorImageVtkViewInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

    if (!data)
        return;

    QString identifier = data->identifier();

    // up to the moment 2 itk tensor image formats are supported
    // we need to convert them to vtkStructuredPoints so it's understood by the tensor manager
    if (identifier == "itkDataTensorImageFloat3")
        d->setVTKFilter <TensorImageTypeFloat>(data,d->filterFloat);
    else if (identifier == "itkDataTensorImageDouble3")
        d->setVTKFilter <TensorImageTypeDouble>(data,d->filterDouble);
    else
    {
        dtkDebug() << "Unrecognized tensor data type: " << identifier;
        return;
    }

    d->actorProperty = itkDataTensorImageVtkViewInteractorPrivate::PropertySmartPointer::New();
    d->manager->GetTensorVisuManagerAxial()->GetActor()->SetProperty( d->actorProperty );
    d->manager->GetTensorVisuManagerSagittal()->GetActor()->SetProperty( d->actorProperty );
    d->manager->GetTensorVisuManagerCoronal()->GetActor()->SetProperty( d->actorProperty );

    medStringListParameterL *shapeParam = new medStringListParameterL("Shape", data);
    d->parameters << shapeParam;

    shapeParam->addItem("Lines");
    shapeParam->addItem("Disks");
    shapeParam->addItem("Arrows");
    shapeParam->addItem("Cubes");
    shapeParam->addItem("Cylinders");
    shapeParam->addItem("Ellipsoids");
    shapeParam->addItem("Superquadrics");

    medIntParameterL *sampleRateParam = new medIntParameterL("Sample Rate", data);
    d->parameters << sampleRateParam;
    sampleRateParam->setRange(1, 10);
    sampleRateParam->setValue(2);

    medBoolParameterL *flipXParam = new medBoolParameterL("FlipX", this);
    medBoolParameterL *flipYParam = new medBoolParameterL("FlipY", this);
    medBoolParameterL *flipZParam = new medBoolParameterL("FlipZ", this);
    d->parameters << flipXParam << flipYParam << flipZParam;

    medStringListParameterL *eigenParam = new medStringListParameterL("Color with", this);
    eigenParam->addItem("v1");
    eigenParam->addItem("v2");
    eigenParam->addItem("v3");
    eigenParam->setValue("v1");
    d->parameters << eigenParam;

    medIntParameterL *resolutionParam = new medIntParameterL("Resolution", this);
    resolutionParam->setRange(2, 20);
    resolutionParam->setValue(6);
    d->parameters << resolutionParam;

    medIntParameterL *scaleParam = new medIntParameterL("Scale", this);
    scaleParam->setRange(1, 9);
    scaleParam->setValue(1);
    d->parameters << scaleParam;

    medIntParameterL *multiplierParam = new medIntParameterL("x10^", this);
    multiplierParam->setRange(-10, 10);
    multiplierParam->setValue(3);
    d->parameters << multiplierParam;

    d->parameters << visibilityParameter();
    d->parameters << opacityParameter();

    connect(shapeParam, SIGNAL(valueChanged(QString)), this, SLOT(setGlyphShape(QString)));
    connect(sampleRateParam, SIGNAL(valueChanged(int)), this, SLOT(setSampleRate(int)));
    connect(flipXParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipX(bool)));
    connect(flipYParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipY(bool)));
    connect(flipZParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipZ(bool)));
    connect(eigenParam, SIGNAL(valueChanged(QString)), this, SLOT(setEigenVector(QString)));
    connect(resolutionParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphResolution(int)));
    connect(scaleParam, SIGNAL(valueChanged(int)), this, SLOT(setMinorScaling(int)));
    connect(multiplierParam, SIGNAL(valueChanged(int)), this, SLOT(setMajorScaling(int)));

    if(d->view->layer(d->data) == 0)
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

    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()));
    this->updateWidgets();
}


void itkDataTensorImageVtkViewInteractor::setWindowLevel(QHash<QString,QVariant>)
{
    //TODO
}

void itkDataTensorImageVtkViewInteractor::setOpacity(double opacity)
{
    d->actorProperty->SetOpacity(opacity);

    d->view->render();
}


void itkDataTensorImageVtkViewInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetTensorVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetTensorVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetTensorVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setGlyphShape(QString glyphShape)
{
    if(glyphShape == "Lines")
         d->manager->SetGlyphShapeToLine();
    else if(glyphShape == "Disks")
        d->manager->SetGlyphShapeToDisk();
    else if(glyphShape == "Arrows")
        d->manager->SetGlyphShapeToArrow();
    else if(glyphShape == "Cubes")
        d->manager->SetGlyphShapeToCube();
    else if(glyphShape == "Cylinders")
        d->manager->SetGlyphShapeToCylinder();
    else if(glyphShape == "Ellipsoids")
        d->manager->SetGlyphShapeToSphere();
    else if(glyphShape == "Superquadrics")
        d->manager->SetGlyphShapeToSuperquadric();

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setSampleRate(int sampleRate)
{
    d->manager->SetSampleRate(sampleRate, sampleRate, sampleRate);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::setEigenVector(int eigenVector)
{
    // we need to substract 1 because the manager receives an index
    d->manager->SetColorModeToEigenvector(eigenVector-1);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::setEigenVector(QString colorMode)
{
    if(colorMode == "v1")
        this->setEigenVector(1);
    else if (colorMode == "v2")
        this->setEigenVector(2);
    else if(colorMode == "v3")
        this->setEigenVector(3);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setGlyphResolution(int glyphResolution)
{
    d->manager->SetGlyphResolution(glyphResolution);
    this->update();
}


void itkDataTensorImageVtkViewInteractor::setScale(double scale)
{
    d->manager->SetGlyphScale((float)scale);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::setMajorScaling(int majorScalingExponent)
{
     d->majorScalingExponent = majorScalingExponent;
     setScale(d->minorScaling, d->majorScalingExponent);

     this->update();
}

void itkDataTensorImageVtkViewInteractor::setMinorScaling(int minorScaling)
{
     d->minorScaling = minorScaling;
     setScale(d->minorScaling, d->majorScalingExponent);

     this->update();
}

void itkDataTensorImageVtkViewInteractor::setScale(int minorScale, int majorScaleExponent)
{
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    setScale(scale);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setShowAxial(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setShowCoronal(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setShowSagittal(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);

    this->update();
}

void itkDataTensorImageVtkViewInteractor::setFlipX(bool flip)
{
    d->manager->FlipX(flip);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::setFlipY(bool flip)
{
    d->manager->FlipY(flip);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::setFlipZ(bool flip)
{
    d->manager->FlipZ(flip);
    this->update();
}

void itkDataTensorImageVtkViewInteractor::changePosition(const QVector3D& position)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());

    this->update();
}


void itkDataTensorImageVtkViewInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}

void itkDataTensorImageVtkViewInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

QWidget* itkDataTensorImageVtkViewInteractor::buildLayerWidget()
{
    QSlider *slider = opacityParameter()->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}

QWidget* itkDataTensorImageVtkViewInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    for(medAbstractParameterL *parameter : d->parameters)
    {
        if ((parameter->name() == "Opacity")||(parameter->name() == "Visibility"))
        {
            continue;
        }
        layout->addRow(parameter->getLabel(), parameter->getWidget());
    }

    return toolbox;
}

QWidget* itkDataTensorImageVtkViewInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QList<medAbstractParameterL*> itkDataTensorImageVtkViewInteractor::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameterL*> itkDataTensorImageVtkViewInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameterL*>();
}

void itkDataTensorImageVtkViewInteractor::update()
{
    d->view->render();
}

void itkDataTensorImageVtkViewInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void itkDataTensorImageVtkViewInteractor::updateSlicingParam()
{
    if(!d->view->is2D())
        return;

    //TODO Should be set according to the real number of slice of this data and
    // not according to vtkInria (ie. first layer droped) - RDE
    // slice orientation may differ from view orientation. Adapt slider range accordingly.
//    int orientationId = d->view2d->GetSliceOrientation();
//    int *dim = d->manager->GetInput()->GetDimensions();

//    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
//        d->slicingParameter->setRange(0, dim[2] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
//        d->slicingParameter->setRange (0, dim[1] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
//        d->slicingParameter->setRange (0, dim[0] - 1);

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}

