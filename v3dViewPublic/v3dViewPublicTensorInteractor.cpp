#include "v3dViewPublicTensorInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkTensorManager.h>
#include <vtkStructuredPoints.h>

#include "v3dViewPublic.h"

#include <itkITKTensorsToVTKTensorsFilter.h>
#include <itkImage.h>
#include <itkTensor.h>

typedef itk::Tensor<float, 3>    TensorTypeFloat;
typedef itk::Image<TensorTypeFloat, 3> TensorImageTypeFloat;
typedef TensorImageTypeFloat::Pointer TensorImagePointerFloat;

typedef itk::Tensor<double, 3>    TensorTypeDouble;
typedef itk::Image<TensorTypeDouble, 3> TensorImageTypeDouble;
typedef TensorImageTypeDouble::Pointer TensorImagePointerDouble;

class v3dViewPublicTensorInteractorPrivate
{
public:
    dtkAbstractData        *data;
    v3dViewPublic          *view;
    vtkTensorManager       *manager;
	vtkStructuredPoints    *tensors;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::Pointer filterFloat;
    TensorImagePointerFloat      datasetFloat;

    itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::Pointer filterDouble;
    TensorImagePointerDouble      datasetDouble;
};

v3dViewPublicTensorInteractor::v3dViewPublicTensorInteractor(): dtkAbstractViewInteractor(), d(new v3dViewPublicTensorInteractorPrivate)
{
    d->data    = 0;
    d->view    = 0;
    d->manager = vtkTensorManager::New();
	d->tensors = vtkStructuredPoints::New();

	d->datasetFloat = 0;
	d->filterFloat = 0;

	d->datasetDouble = 0;
	d->filterDouble = 0;

	// set default glyph shape
	d->manager->SetGlyphShapeToCube();
}

v3dViewPublicTensorInteractor::~v3dViewPublicTensorInteractor()
{
    this->disable();
    d->manager->Delete();

    delete d;
    d = 0;
}

QString v3dViewPublicTensorInteractor::description(void) const
{
    return "v3dViewTensorInteractor";
}

QStringList v3dViewPublicTensorInteractor::handled(void) const
{
    return QStringList () << "v3dViewPublic";
}

bool v3dViewPublicTensorInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewTensorInteractor", QStringList() << "v3dViewPublic", createV3dViewPublicTensorInteractor);
}

void v3dViewPublicTensorInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    QString description = data->description();

    if (description.compare("itkDataTensorImageFloat3") == 0) {
		if (TensorImageTypeFloat *dataset = static_cast<TensorImageTypeFloat *>(data->data())) {

			d->datasetFloat = dataset;

			d->filterFloat = itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeFloat>::New();

			d->filterFloat->SetInput(dataset);

			// this line generates the vtkTensors, otherwise is not generated, even if the next filter
			// in the pipeline is connected and Update() is called
			d->filterFloat->Update();

			// we need to call this function because GetOutput() just returns the input
			d->tensors = d->filterFloat->GetVTKTensors();

			d->manager->SetInput(d->tensors);

			// TODO this should not be here once the toolbox is coded
			d->manager->ResetPosition();

			d->manager->Update();

			d->data = data;
		}
    } else if (description.compare("itkDataTensorImageDouble3") == 0) {
		if (TensorImageTypeDouble *dataset = static_cast<TensorImageTypeDouble *>(data->data())) {

			d->datasetDouble = dataset;

			d->filterDouble = itk::ITKTensorsToVTKTensorsFilter<TensorImageTypeDouble>::New();

			d->filterDouble->SetInput(dataset);

			// this line generates the vtkTensors, otherwise is not generated, even if the next filter
			// in the pipeline is connected and Update() is called
			d->filterDouble->Update();

			// we need to call this function because GetOutput() just returns the input
			d->tensors = d->filterDouble->GetVTKTensors();

			d->manager->SetInput(d->tensors);

			// TODO this should not be here once the toolbox is coded
			d->manager->ResetPosition();

			d->manager->Update();

			d->data = data;
		}
    } else {
    	qDebug() << "Unrecognized tensor data type: " << description;
    }
}

dtkAbstractData *v3dViewPublicTensorInteractor::data (void)
{
    return d->data;
}

void v3dViewPublicTensorInteractor::setView(dtkAbstractView *view)
{
    if (v3dViewPublic *v3dview = dynamic_cast<v3dViewPublic*>(view) ) {
        d->view = v3dview;
        d->manager->SetRenderWindowInteractor( d->view->interactor3D(), d->view->renderer3D() );
    }
}

dtkAbstractView *v3dViewPublicTensorInteractor::view (void)
{
    return d->view;
}

void v3dViewPublicTensorInteractor::enable(void)
{
    if (this->enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewPublicTensorInteractor::disable(void)
{
    if (!this->enabled())
        return;

    dtkAbstractViewInteractor::disable();
}

void v3dViewPublicTensorInteractor::onPropertySet(const QString& key, const QString& value)
{

}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewPublicTensorInteractor(void)
{
    return new v3dViewPublicTensorInteractor;
}
