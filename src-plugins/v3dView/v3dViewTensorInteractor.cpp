/*=========================================================================

 MedInria

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

#include "v3dView.h"

#include <itkITKTensorsToVTKTensorsFilter.h>
#include <itkImage.h>
#include <itkTensor.h>

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
    dtkSmartPointer<v3dView>         view;
    vtkTensorManager                *manager;

    // the filters will convert from itk tensor image format to vtkStructuredPoint (format handled by the tensor manager)
    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::Pointer filterFloat;
    TensorImagePointerFloat      datasetFloat;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::Pointer filterDouble;
    TensorImagePointerDouble      datasetDouble;
};

v3dViewTensorInteractor::v3dViewTensorInteractor(): dtkAbstractViewInteractor(), d(new v3dViewTensorInteractorPrivate)
{
    d->manager = vtkTensorManager::New();

    d->datasetFloat = 0;
    d->filterFloat = 0;

    d->datasetDouble = 0;
    d->filterDouble = 0;

    this->addProperty("GlyphShape", QStringList() << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics");
    this->addProperty("FlipX", QStringList() << "true" << "false");
    this->addProperty("FlipY", QStringList() << "true" << "false");
    this->addProperty("FlipZ", QStringList() << "true" << "false");

    // set default properties
    d->manager->SetGlyphShapeToLine();
    this->setProperty("GlyphShape", "Lines");
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
    return QStringList () << "v3dView";
}

bool v3dViewTensorInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewTensorInteractor", QStringList() << "v3dView", createV3dViewTensorInteractor);
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
}

dtkAbstractData *v3dViewTensorInteractor::data()
{
    return d->data;
}

void v3dViewTensorInteractor::setView(dtkAbstractView *view)
{
    if (d->view) {
        disconnect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                   this,    SLOT(onPositionChanged(const QVector3D&,bool)));
        d->view = 0;
    }


    if (v3dView *v3dview = qobject_cast<v3dView*>(view) ) {
        d->view = v3dview;
        // be careful not to forget setting the same renderer for the interactor and the view
        // otherwise a new renderer is created
        d->manager->SetRenderWindowInteractor( d->view->interactor(), d->view->renderer3d() );

        connect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                this,    SLOT(onPositionChanged(const QVector3D&,bool)));
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

void v3dViewTensorInteractor::onPropertySet(const QString& key, const QString& value)
{
    if (key=="GlyphShape")
    {
        this->onGlyphShapePropertySet (value);
    }
    else if (key=="FlipX")
    {
        this->onFlipXPropertySet (value);
    }
    else if (key=="FlipY")
    {
        this->onFlipYPropertySet (value);
    }
    else if (key=="FlipZ")
    {
        this->onFlipZPropertySet (value);
    }
}

void v3dViewTensorInteractor::onGlyphShapePropertySet (const QString& value)
{
    if (value == "Lines")
    {
        d->manager->SetGlyphShapeToLine();
    }
    else if (value == "Disks")
    {
        d->manager->SetGlyphShapeToDisk();
    }
    else if (value == "Arrows")
    {
        d->manager->SetGlyphShapeToArrow();
    }
    else if (value == "Cubes")
    {
        d->manager->SetGlyphShapeToCube();
    }
    else if (value == "Cylinders")
    {
        d->manager->SetGlyphShapeToCylinder();
    }
    else if (value == "Ellipsoids")
    {
        d->manager->SetGlyphShapeToSphere();
    }
    else if (value == "Superquadrics")
    {
        d->manager->SetGlyphShapeToSuperquadric();
    }
}

void v3dViewTensorInteractor::onSampleRatePropertySet (int sampleRate)
{
    d->manager->SetSampleRate(sampleRate, sampleRate, sampleRate);
}

void v3dViewTensorInteractor::onEigenVectorPropertySet (int eigenVector)
{
    // we need to substract 1 because the manager receives an index
    d->manager->SetColorModeToEigenvector(eigenVector-1);
}

void v3dViewTensorInteractor::onGlyphResolutionPropertySet (int glyphResolution)
{
    d->manager->SetGlyphResolution(glyphResolution);
}

void v3dViewTensorInteractor::onReverseBackgroundColorPropertySet (bool isWhite)
{
    if (!d->view)
        return;

    if(isWhite)
        d->view->setBackgroundColor(1.0,1.0,1.0);
    else
        d->view->setBackgroundColor(0.0,0.0,0.0);
}

void v3dViewTensorInteractor::onScalingPropertySet (double scale)
{
    d->manager->SetGlyphScale((float)scale);
}

void v3dViewTensorInteractor::onHideShowAxialPropertySet(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);
}

void v3dViewTensorInteractor::onHideShowCoronalPropertySet(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);
}

void v3dViewTensorInteractor::onHideShowSagittalPropertySet(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);
}

void v3dViewTensorInteractor::onFlipXPropertySet (const QString& flipX)
{
    if (flipX == "true")
        d->manager->FlipX(true);
    else
        d->manager->FlipX(false);
}

void v3dViewTensorInteractor::onFlipYPropertySet (const QString& flipY)
{
    if (flipY == "true")
        d->manager->FlipY(true);
    else
        d->manager->FlipY(false);
}

void v3dViewTensorInteractor::onFlipZPropertySet (const QString& flipZ)
{
    if (flipZ == "true")
        d->manager->FlipZ(true);
    else
        d->manager->FlipZ(false);
}

void v3dViewTensorInteractor::onPositionChanged(const QVector3D& position, bool propagate)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewTensorInteractor()
{
    return new v3dViewTensorInteractor;
}
