/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageVtkViewInteractor.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <vtkTensorManager.h>
#include <vtkStructuredPoints.h>
#include <vtkImageFromBoundsSource.h>
#include <vtkPointSet.h>
#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderer.h>

#include <itkITKTensorsToVTKTensorsFilter.h>
#include <itkImage.h>
#include <itkTensor.h>

#include <medAbstractParameter.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medBoolGroupParameter.h>
#include <medColorListParameter.h>
#include <medAbstractImageView.h>
#include <medImageViewFactory.h>
#include <medVtkViewBackend.h>


typedef itk::Tensor<float, 3>    TensorTypeFloat;
typedef itk::Image<TensorTypeFloat, 3> TensorImageTypeFloat;
typedef TensorImageTypeFloat::Pointer TensorImagePointerFloat;

typedef itk::Tensor<double, 3>    TensorTypeDouble;
typedef itk::Image<TensorTypeDouble, 3> TensorImageTypeDouble;
typedef TensorImageTypeDouble::Pointer TensorImagePointerDouble;

class itkDataTensorImageVtkViewInteractorPrivate
{
public:
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractData *data;
    vtkTensorManager *manager;
    vtkRenderWindow *render;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;

    // the filters will convert from itk tensor image format to vtkStructuredPoint (format handled by the tensor manager)
    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::Pointer filterFloat;
    TensorImagePointerFloat      datasetFloat;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::Pointer filterDouble;
    TensorImagePointerDouble      datasetDouble;

    int minorScaling;
    int majorScalingExponent;

    double imageBounds[6];

    QList <medAbstractParameter*> parameters;

    QImage thumbnail;

    QWidget *toolbox;
};

itkDataTensorImageVtkViewInteractor::itkDataTensorImageVtkViewInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent),
    d(new itkDataTensorImageVtkViewInteractorPrivate)
{
    d->view = parent;

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->renderer2d = backend->renderer2D;
    d->renderer3d = backend->renderer3D;

    d->manager = vtkTensorManager::New();

    d->datasetFloat = NULL;
    d->filterFloat = NULL;

    d->datasetDouble = NULL;
    d->filterDouble = NULL;

    d->minorScaling = 1;
    d->majorScalingExponent = 0;

    // set default properties
    d->manager->SetGlyphShapeToLine();

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;

    // be careful not to forget setting the same renderer for the interactor and the view
    // otherwise a new renderer is created
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(), d->renderer3d);

    connect(d->view, SIGNAL(positionViewedChanged(QVector3D)), this, SLOT(changePosition(QVector3D)));

    d->toolbox = NULL;

}


itkDataTensorImageVtkViewInteractor::~itkDataTensorImageVtkViewInteractor()
{
    delete d;
    d = NULL;
}

void itkDataTensorImageVtkViewInteractor::removeData()
{
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
    medImageViewFactory *factory = medImageViewFactory::instance();
    factory->registerInteractor<itkDataTensorImageVtkViewInteractor>("itkDataTensorImageVtkViewInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     itkDataTensorImageVtkViewInteractor::dataHandled());
    return true;
}

void itkDataTensorImageVtkViewInteractor::setData(medAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    // up to the moment 2 itk tensor image formats are supported
    // we need to convert them to vtkStructuredPoints so it's understood by the tensor manager
    if (identifier.compare("itkDataTensorImageFloat3") == 0)
    {
        if (TensorImageTypeFloat *dataset = static_cast<TensorImageTypeFloat *>(data->data())) {

            d->datasetFloat = dataset;

            d->filterFloat = itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::New();

            d->filterFloat->SetInput(dataset);

            // this line generates the vtkTensors, otherwise is not generated, even if the next filter
            // in the pipeline is connected and Update() is called
            d->filterFloat->Update();

            // we need to call this function because GetOutput() just returns the input
            vtkStructuredPoints *tensors = d->filterFloat->GetVTKTensors();
            vtkMatrix4x4 *matrix = d->filterFloat->GetDirectionMatrix();

            d->manager->SetInput(tensors);
            d->manager->SetDirectionMatrix(matrix);

            // TODO this should not be here once the toolbox is coded
            d->manager->ResetPosition();

            d->manager->Update();

            if(d->view->layersCount() == 1)
            {
                computeBounds();
            }

            if (d->view) {
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerAxial()->GetActor());
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerSagittal()->GetActor());
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerCoronal()->GetActor());
            }

            d->data = data;
        }
    }
    else if (identifier.compare("itkDataTensorImageDouble3") == 0)
    {
        if (TensorImageTypeDouble *dataset = static_cast<TensorImageTypeDouble *>(data->data())) {

            d->datasetDouble = dataset;

            d->filterDouble = itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::New();

            d->filterDouble->SetInput(dataset);

            // this line generates the vtkTensors, otherwise is not generated, even if the next filter
            // in the pipeline is connected and Update() is called
            d->filterDouble->Update();

            // we need to call this function because GetOutput() just returns the input
            vtkStructuredPoints* tensors = d->filterDouble->GetVTKTensors();
            vtkMatrix4x4 *matrix = d->filterDouble->GetDirectionMatrix();

            d->manager->SetInput(tensors);
            d->manager->SetDirectionMatrix(matrix);

            // TODO this should not be here once the toolbox is coded
            d->manager->ResetPosition();

            d->manager->Update();

            if(d->view->layersCount() == 1)
            {
                computeBounds();
            }

            if (d->view) {
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerAxial()->GetActor());
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerSagittal()->GetActor());
                d->renderer2d->AddActor (d->manager->GetTensorVisuManagerCoronal()->GetActor());
            }

            d->data = data;
        }
    } else
    {
        qDebug() << "Unrecognized tensor data type: " << identifier;
    }

    medStringListParameter *shapeParam = new medStringListParameter("Shape", data);
    d->parameters << shapeParam;
    QStringList shapes;
    shapes << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics";
    shapeParam->addItems(shapes);

    medIntParameter *sampleRateParam = new medIntParameter("Sample Rate", data);
    d->parameters << sampleRateParam;
    sampleRateParam->setRange(1, 10);
    sampleRateParam->setValue(2);

    medBoolParameter *flipXParam = new medBoolParameter("FlipX", data);
    medBoolParameter *flipYParam = new medBoolParameter("FlipY", data);
    medBoolParameter *flipZParam = new medBoolParameter("FlipZ", data);
    d->parameters << flipXParam << flipYParam << flipZParam;

    medStringListParameter *eigenParam = new medStringListParameter("Color with", data);
    QStringList colorMode = QStringList() << "v1" << "v2" << "v3";
    eigenParam->addItems(colorMode);
    eigenParam->setValue("v1");
    d->parameters << eigenParam;

    medIntParameter *resolutionParam = new medIntParameter("Resolution", data);
    resolutionParam->setRange(2, 20);
    resolutionParam->setValue(6);
    d->parameters << resolutionParam;

    medIntParameter *scaleParam = new medIntParameter("Scale", data);
    scaleParam->setRange(1, 9);
    scaleParam->setValue(1);
    d->parameters << scaleParam;

    medIntParameter *multiplierParam = new medIntParameter("x10^", data);
    multiplierParam->setRange(-10, 10);
    multiplierParam->setValue(0);
    d->parameters << multiplierParam;


    connect(shapeParam, SIGNAL(valueChanged(QString)), this, SLOT(setGlyphShape(QString)));
    connect(sampleRateParam, SIGNAL(valueChanged(int)), this, SLOT(setSampleRate(int)));
    connect(flipXParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipX(bool)));
    connect(flipYParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipY(bool)));
    connect(flipZParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipZ(bool)));
    connect(eigenParam, SIGNAL(valueChanged(QString)), this, SLOT(setEigenVector(QString)));
    connect(resolutionParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphResolution(int)));
    connect(scaleParam, SIGNAL(valueChanged(int)), this, SLOT(setMinorScaling(int)));
    connect(multiplierParam, SIGNAL(valueChanged(int)), this, SLOT(setMajorScaling(int)));
}


void itkDataTensorImageVtkViewInteractor::setWindowLevel(double &window, double &level)
{
    //TODO
}

void itkDataTensorImageVtkViewInteractor::windowLevel(double &window, double &level)
{
    //TODO
}

void itkDataTensorImageVtkViewInteractor::setOpacity(double opacity)
{
    //TODO
}

double itkDataTensorImageVtkViewInteractor::opacity() const
{
    //TODO
    return 100;
}

void itkDataTensorImageVtkViewInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetTensorVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetTensorVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetTensorVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
}

bool itkDataTensorImageVtkViewInteractor::visibility() const
{
    return (d->manager->GetTensorVisuManagerAxial()->GetActor()->GetVisibility() == 1);
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

QImage itkDataTensorImageVtkViewInteractor::generateThumbnail(const QSize &size)
{
    int w(size.width()), h(size.height());

    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();

    d->render->SetOffScreenRendering(1);

    d->view->viewWidget()->resize(w,h);
    d->render->vtkRenderWindow::SetSize(w,h);
    d->view2d->Reset();
    d->view2d->Render();

    QGLWidget *glWidget = dynamic_cast<QGLWidget *>(d->view->viewWidget());
    d->thumbnail = glWidget->grabFrameBuffer();

    d->render->SetOffScreenRendering(0);

    return d->thumbnail;
}


void itkDataTensorImageVtkViewInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    //TODO
}

void itkDataTensorImageVtkViewInteractor::moveToSlice(int slice)
{
    //TODO
}

QWidget* itkDataTensorImageVtkViewInteractor::layerWidget()
{
    return NULL;
}

QWidget* itkDataTensorImageVtkViewInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        foreach(medAbstractParameter *parameter, d->parameters)
            layout->addRow(parameter->getLabel(), parameter->getWidget());
    }
    return d->toolbox;
}

QWidget* itkDataTensorImageVtkViewInteractor::toolBarWidget()
{
    return NULL;
}

QList<medAbstractParameter*> itkDataTensorImageVtkViewInteractor::parameters()
{
    return d->parameters;
}

void itkDataTensorImageVtkViewInteractor::update()
{
    d->render->Render();
}

void itkDataTensorImageVtkViewInteractor::computeBounds()
{
    d->manager->GetTensorVisuManagerAxial()->GetActor()->GetBounds(d->imageBounds);

    updateBounds(d->manager->GetTensorVisuManagerSagittal()->GetActor()->GetBounds());
    updateBounds(d->manager->GetTensorVisuManagerCoronal()->GetActor()->GetBounds());

    // these bounds are used by vtkImageFromBoundsSource to generate a background image in case there is none
    // vtkImageFromBoundsSource output image size is actually [boundsXMax-boundXMin]...,
    // so we need to increase bounds by +1 to have the correct image size
    d->imageBounds[0] = round(d->imageBounds[0]);
    d->imageBounds[1] = round(d->imageBounds[1])+1;
    d->imageBounds[2] = round(d->imageBounds[2]);
    d->imageBounds[3] = round(d->imageBounds[3])+1;
    d->imageBounds[4] = round(d->imageBounds[4]);
    d->imageBounds[5] = round(d->imageBounds[5])+1;

    d->view2d->updateBounds(d->imageBounds, d->manager->GetInput()->GetDimensions());
}

void itkDataTensorImageVtkViewInteractor::updateBounds(const double bounds[])
{
    for (int i=0; i<6; i=i+2)
        if (bounds[i] < d->imageBounds[i])
            d->imageBounds[i]=bounds[i];

    for (int i=1; i<6; i=i+2)
        if (bounds[i] > d->imageBounds[i])
            d->imageBounds[i]=bounds[i];

}
