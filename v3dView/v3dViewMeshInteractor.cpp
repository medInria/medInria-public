#include "v3dViewMeshInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkBoundingBox.h>
#include <vtkPolyDataManager.h>
#include "v3dView.h"
#include <vtkDatasetToImageGenerator.h>
#include <vtkImageData.h>
#include <QInputDialog>
#include <QColorDialog>
#include <vtkImageActor.h>

class v3dViewMeshInteractorPrivate
{
public:
	typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
	typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

	dtkAbstractData  *data;
	v3dView          *view;
	ActorSmartPointer     actor2d;
	ActorSmartPointer     actor3d;
	PropertySmartPointer  actorProperty;


};

v3dViewMeshInteractor::v3dViewMeshInteractor(): dtkAbstractViewInteractor(), d(new v3dViewMeshInteractorPrivate)
{
	d->data = NULL;
	d->view = NULL;


	qDebug()<<"v3dViewMeshInteractor";
	// addProperty here
	this->addProperty("Visibility", QStringList() << "true" << "false");
	this->addProperty("ShowEdges", QStringList() << "true" << "false");
	this->addProperty("RenderingMode", QStringList() << "surface" << "wireframe" << "points" );

}

v3dViewMeshInteractor::~v3dViewMeshInteractor()
{
	this->disable();

	delete d;
	d = 0;
}

QString v3dViewMeshInteractor::description(void) const
{
	return "v3dViewMeshInteractor";
}

QStringList v3dViewMeshInteractor::handled(void) const
{
	return QStringList () << "v3dView";
}

bool v3dViewMeshInteractor::registered(void)
{
	return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewMeshInteractor", QStringList() << "v3dView", createV3dViewMeshInteractor);
}

void v3dViewMeshInteractor::setData(dtkAbstractData *data)
{

	qDebug()<<"v3dViewMeshInteractor::setData";
	if (vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(data->data()))) {
		if(!d->view->data())
		{
			vtkDatasetToImageGenerator* imagegenerator = vtkDatasetToImageGenerator::New();
			imagegenerator->SetInput (pointSet);
			vtkImageData * image = imagegenerator->GetOutput();

            qDebug()<<"Number of Layers : "<<d->view->view2d()->GetNumberOfLayers();
            d->view->view2d()->SetInput(image);
			vtkImageActor *actor = d->view->view2d()->GetImageActor();
			actor->SetOpacity(0.0);	

			//typedef itk::Vector< unsigned char, 3 >    PixelType;
   //         typedef itk::Image< PixelType, 3 >    ImageType;
   //         ImageType::Pointer image = ImageType::New();

   //         ImageType::IndexType start;
   //         ImageType::SizeType  size;
   //         double spacing[ ImageType::ImageDimension ];
   //         spacing[0] = 0.626; // spacing in mm along X
   //         spacing[1] = 0.626; // spacing in mm along Y
   //         spacing[2] = 0.813; // spacing in mm along Z
   //         image->SetSpacing( spacing );

   //         size[0]  = 128;  // size along X
   //         size[1]  = 128;  // size along Y
   //         size[2]  = 128;  // size along Z

   //         start[0] =   0;  // first index on X
   //         start[1] =   0;  // first index on Y
   //         start[2] =   0;  // first index on Z

   //         double bounds[6];
   //         pointSet->GetBounds(bounds);

   //         ImageType::RegionType region;
   //         region.SetSize( size );
   //         region.SetIndex( start );

   //         // Pixel data is allocated
   //         image->SetRegions( region );
   //         image->Allocate();

   //         d->view->view2d()->SetITKInput(image);
		 //   d->view->view2d()->GetImageActor()->SetOpacity(0.0);
		}
		Q_UNUSED( pointSet );
		d->data = data;

		updatePipeline();

	}
}


void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
	qDebug()<<" v3dViewMeshInteractor::setView";
	if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
		d->view = v3dview;


	}
}

bool v3dViewMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
	if ( data->description() == "vtkDataMesh" ) {

		this->enable ();
		return true;
	}
	return false;
}

void v3dViewMeshInteractor::enable(void)
{
	dtkWarning() << "enabling v3dViewMeshInteractor" ;

	if (this->enabled())
		return;

	updatePipeline ();

	dtkAbstractViewInteractor::enable();
}


void v3dViewMeshInteractor::disable(void)
{
	if (!this->enabled())
		return;

	if (d->view) {

		// TODO        d->view->view3D ()->RemoveDataset ();
		// TODO        d->view->view3D ()->RemoveDataset ();
		// TODO        d->view->view3D ()->RemoveDataset ();
		// TODO        d->view->view3D ()->RemoveDataset ();
	}
	dtkAbstractViewInteractor::disable();
}

void v3dViewMeshInteractor::onPropertySet(const QString& key, const QString& value)
{
	if (key=="Visibility")
		this->onVisibilityPropertySet (value);

	if (key=="ShowEdges")
		this->onEdgeVisibilityPropertySet (value);

	if (key=="RenderingMode")
		this->onRenderingModePropertySet (value);

	if (d->view)
		d->view->update();
}


void v3dViewMeshInteractor::onVisibilityPropertySet (const QString& value)
{
	if (value=="true") {
		d->actor2d->SetVisibility(1);
		d->actor3d->SetVisibility(1);
	} else {
		d->actor2d->SetVisibility(0);
		d->actor3d->SetVisibility(0);
	}
}

void v3dViewMeshInteractor::onEdgeVisibilityPropertySet (const QString& value)
{
	if (value=="true")
		d->actorProperty->SetEdgeVisibility (1);
	else
		d->actorProperty->SetEdgeVisibility (0);
}

void v3dViewMeshInteractor::onRenderingModePropertySet (const QString& value)
{
	if (value=="wireframe") {
		d->actorProperty->SetRepresentationToWireframe ();
	} else if (value=="surface") {
		d->actorProperty->SetRepresentationToSurface ();
	} else if (value=="points") {
		d->actorProperty->SetRepresentationToPoints ();
	}
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void)
{
	return new v3dViewMeshInteractor;
}

void v3dViewMeshInteractor::updatePipeline (void)
{










	if (d->view && d->data) {
		if(vtkPointSet *pointset = dynamic_cast<vtkPointSet*>((vtkObject *)(d->data->data()))) {


			d->actor2d = d->view->view2d ()->AddDataSet (pointset);
			d->actor3d = d->view->view3d ()->AddDataSet(pointset);
			//pointset->Print(std::cout);

			d->actorProperty = v3dViewMeshInteractorPrivate::PropertySmartPointer::New ();

			d->actorProperty->SetColor ( 0,1,0 );
			d->actor2d->SetProperty ( d->actorProperty );
			d->actor3d->SetProperty ( d->actorProperty );

			d->actor2d->GetMapper ()->ScalarVisibilityOff ();
			d->actor3d->GetMapper ()->ScalarVisibilityOff ();




		}
	}

}

