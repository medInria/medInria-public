/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dViewTensorInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <vtkTensorManager.h>
#include <vtkStructuredPoints.h>

#include "medVtkView.h"

#include <itkITKTensorsToVTKTensorsFilter.h>
#include <itkImage.h>
#include <itkTensor.h>
#include <medParameter.h>

typedef itk::Tensor<float, 3>    TensorTypeFloat;
typedef itk::Image<TensorTypeFloat, 3> TensorImageTypeFloat;
typedef TensorImageTypeFloat::Pointer TensorImagePointerFloat;

typedef itk::Tensor<double, 3>    TensorTypeDouble;
typedef itk::Image<TensorTypeDouble, 3> TensorImageTypeDouble;
typedef TensorImageTypeDouble::Pointer TensorImagePointerDouble;

class v3dViewTensorInteractorPrivate
{
public:
    dtkSmartPointer<dtkAbstractData> data;
    v3dView*                        view;
    vtkTensorManager                *manager;

    // the filters will convert from itk tensor image format to vtkStructuredPoint (format handled by the tensor manager)
    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::Pointer filterFloat;
    TensorImagePointerFloat      datasetFloat;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::Pointer filterDouble;
    TensorImagePointerDouble      datasetDouble;

    int minorScaling;
    int majorScalingExponent;
};

v3dViewTensorInteractor::v3dViewTensorInteractor(): medAbstractVtkViewInteractor(), d(new v3dViewTensorInteractorPrivate)
{
    d->manager = vtkTensorManager::New();

    d->datasetFloat = 0;
    d->filterFloat = 0;

    d->datasetDouble = 0;
    d->filterDouble = 0;

    d->data = 0;
    d->view = 0;

    d->minorScaling = 1;
    d->majorScalingExponent = 0;

    // set default properties
    d->manager->SetGlyphShapeToLine();
}


v3dViewTensorInteractor::~v3dViewTensorInteractor()
{
    this->disable();
    d->manager->Delete();

    delete d;
    d = 0;
}

QString v3dViewTensorInteractor::description() const
{
    return tr("Interactor helping display Tensors");
}

QString v3dViewTensorInteractor::identifier() const
{
    return "v3dViewTensorInteractor";
}

QStringList v3dViewTensorInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
}

bool v3dViewTensorInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType.startsWith("itkDataTensorImage"))
        return true;
    
    return false;
}

bool v3dViewTensorInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewTensorInteractor",
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createV3dViewTensorInteractor);
}

void v3dViewTensorInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    // up to the moment 2 itk tensor image formats are supported
    // we need to convert them to vtkStructuredPoints so it's understood by the tensor manager
    if (identifier.compare("itkDataTensorImageFloat3") == 0) {
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

            if (d->view) {
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerAxial()->GetActor());
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerSagittal()->GetActor());
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerCoronal()->GetActor());
            }

            d->data = data;
        }
    }
    else if (identifier.compare("itkDataTensorImageDouble3") == 0) {
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

            if (d->view) {
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerAxial()->GetActor());
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerSagittal()->GetActor());
                d->view->renderer2d()->AddActor (d->manager->GetTensorVisuManagerCoronal()->GetActor());
            }

            d->data = data;
        }
    } else {
        qDebug() << "Unrecognized tensor data type: " << identifier;
    }

    medListParameter *shapeParam = new medListParameter("Shape", data);
    QStringList shapes;
    shapes << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics";
    shapeParam->setValues(shapes);

    medIntParameter *sampleRateParam = new medIntParameter("Sample Rate", data);
    sampleRateParam->setMinimum(1);
    sampleRateParam->setMaximum(10);
    sampleRateParam->setValue(2);

    medBooleanParameter *flipXParam = new medBooleanParameter("FlipX", data);
    medBooleanParameter *flipYParam = new medBooleanParameter("FlipY", data);
    medBooleanParameter *flipZParam = new medBooleanParameter("FlipZ", data);

    medListParameter *colorParam = new medListParameter("Color", data);
    QStringList colors;
    colors << "v1" << "v2" << "v3";
    colorParam->setValues(colors);

    medIntParameter *resolutionParam = new medIntParameter("Resolution", data);
    resolutionParam->setMinimum(2);
    resolutionParam->setMaximum(20);
    resolutionParam->setValue(6);

    medIntParameter *scaleParam = new medIntParameter("Scale", data);
    scaleParam->setMinimum(1);
    scaleParam->setMaximum(9);
    scaleParam->setValue(1);

    medIntParameter *multiplierParam = new medIntParameter("x10^", data);
    multiplierParam->setMinimum(-10);
    multiplierParam->setMaximum(10);
    multiplierParam->setValue(0);

    medBooleanParameter *showsAxialParam = new medBooleanParameter("Show axial", data);
    medBooleanParameter *showSagitalParam = new medBooleanParameter("Show sagital", data);
    medBooleanParameter *showCoronalParam = new medBooleanParameter("Show coronal", data);

    connect(shapeParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphShape(int)));
    connect(sampleRateParam, SIGNAL(valueChanged(int)), this, SLOT(setSampleRate(int)));
    connect(flipXParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipX(bool)));
    connect(flipYParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipY(bool)));
    connect(flipZParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipZ(bool)));
   // connect(colorParam, SIGNAL(), this, SLOT());
    connect(resolutionParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphResolution(int)));
    connect(scaleParam, SIGNAL(valueChanged(int)), this, SLOT(setMinorScaling(int)));
    connect(multiplierParam, SIGNAL(valueChanged(int)), this, SLOT(setMajorScaling(int)));
    connect(showsAxialParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowAxial(bool)));
    connect(showSagitalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowSagittal(bool)));
    connect(showCoronalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowCoronal(bool)));

    parameters.insert(data, shapeParam);
    parameters.insert(data, sampleRateParam);
    parameters.insert(data, flipXParam);
    parameters.insert(data, flipYParam);
    parameters.insert(data, flipZParam);
    parameters.insert(data, colorParam);
    parameters.insert(data, resolutionParam);
    parameters.insert(data, scaleParam);
    parameters.insert(data, multiplierParam);
    parameters.insert(data, showsAxialParam);
    parameters.insert(data, showSagitalParam);
    parameters.insert(data, showCoronalParam);
}


void v3dViewTensorInteractor::removeData(medAbstractData *data)
{
    if (d->view)
    {
        d->view->renderer2d()->RemoveActor (d->manager->GetTensorVisuManagerAxial()->GetActor());
        d->view->renderer2d()->RemoveActor (d->manager->GetTensorVisuManagerSagittal()->GetActor());
        d->view->renderer2d()->RemoveActor (d->manager->GetTensorVisuManagerCoronal()->GetActor());

        d->view->removeLayer(data);
        d->view->update();
    }
}

dtkAbstractData *v3dViewTensorInteractor::data()
{
    return d->data;
}

void v3dViewTensorInteractor::setView(dtkAbstractView *view)
{
    if (d->view) {
        disconnect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                   this,    SLOT(changePosition(const QVector3D&,bool)));
        d->view = 0;
    }


    if (v3dView *v3dview = qobject_cast<v3dView*>(view) ) {
        d->view = v3dview;
        // be careful not to forget setting the same renderer for the interactor and the view
        // otherwise a new renderer is created
        d->manager->SetRenderWindowInteractor( d->view->interactor(), d->view->renderer3d() );

        connect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                this,    SLOT(changePosition(const QVector3D&,bool)));
    }
}

dtkAbstractView *v3dViewTensorInteractor::view()
{
    return d->view;
}

void v3dViewTensorInteractor::enable()
{
    if (this->enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewTensorInteractor::disable()
{
    if (!this->enabled())
        return;

    dtkAbstractViewInteractor::disable();
}

void v3dViewTensorInteractor::setOpacity(dtkAbstractData * /*data*/, double /*opacity*/)
{
    //TODO
}

double v3dViewTensorInteractor::opacity(dtkAbstractData * /*data*/) const
{
    //TODO
    return 100;
}

void v3dViewTensorInteractor::setVisible(dtkAbstractData * /*data*/, bool /*visible*/)
{
    //TODO
}

bool v3dViewTensorInteractor::isVisible(dtkAbstractData * /*data*/) const
{
    //TODO
    return true;
}

void v3dViewTensorInteractor::setGlyphShape(int glyphShape)
{
    switch(glyphShape)
    {
        case v3dViewTensorInteractor::Lines:
            d->manager->SetGlyphShapeToLine();
            break;
        case v3dViewTensorInteractor::Disks:
            d->manager->SetGlyphShapeToDisk();
            break;
        case v3dViewTensorInteractor::Arrows:
            d->manager->SetGlyphShapeToArrow();
            break;
        case v3dViewTensorInteractor::Cubes:
            d->manager->SetGlyphShapeToCube();
            break;
        case v3dViewTensorInteractor::Cylinders:
            d->manager->SetGlyphShapeToCylinder();
            break;
        case v3dViewTensorInteractor::Ellipsoids:
            d->manager->SetGlyphShapeToSphere();
            break;
        case v3dViewTensorInteractor::Superquadrics:
            d->manager->SetGlyphShapeToSuperquadric();
            break;
        default:
            qDebug() << "Unknown glyph type";
    }

    d->view->update();
}

void v3dViewTensorInteractor::setSampleRate(int sampleRate)
{
    d->manager->SetSampleRate(sampleRate, sampleRate, sampleRate);
    d->view->update();
}

void v3dViewTensorInteractor::setEigenVector(int eigenVector)
{
    // we need to substract 1 because the manager receives an index
    d->manager->SetColorModeToEigenvector(eigenVector-1);
    d->view->update();
}

void v3dViewTensorInteractor::setGlyphResolution(int glyphResolution)
{
    d->manager->SetGlyphResolution(glyphResolution);
    d->view->update();
}

void v3dViewTensorInteractor::setReverseBackgroundColor(bool isWhite)
{
    if (!d->view)
        return;

    if(isWhite)
        d->view->setBackgroundColor(1.0,1.0,1.0);
    else
        d->view->setBackgroundColor(0.0,0.0,0.0);

    d->view->update();
}

void v3dViewTensorInteractor::setScale(double scale)
{
    d->manager->SetGlyphScale((float)scale);
    d->view->update();
}

void v3dViewTensorInteractor::setMajorScaling(int majorScalingExponent)
{
     d->majorScalingExponent = majorScalingExponent;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void v3dViewTensorInteractor::setMinorScaling(int minorScaling)
{
     d->minorScaling = minorScaling;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void v3dViewTensorInteractor::setScale(int minorScale, int majorScaleExponent)
{
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    setScale(scale);
}

void v3dViewTensorInteractor::setShowAxial(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);

    d->view->update();
}

void v3dViewTensorInteractor::setShowCoronal(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);

    d->view->update();
}

void v3dViewTensorInteractor::setShowSagittal(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);

    d->view->update();
}

void v3dViewTensorInteractor::setFlipX(bool flip)
{
    d->manager->FlipX(flip);
    d->view->update();
}

void v3dViewTensorInteractor::setFlipY(bool flip)
{
    d->manager->FlipY(flip);
    d->view->update();
}

void v3dViewTensorInteractor::setFlipZ(bool flip)
{
    d->manager->FlipZ(flip);
    d->view->update();
}

void v3dViewTensorInteractor::changePosition(const QVector3D& position, bool propagate)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());
    d->view->update();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewTensorInteractor()
{
    return new v3dViewTensorInteractor;
}
