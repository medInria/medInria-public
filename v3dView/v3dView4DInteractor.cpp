#include "v3dView4DInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>

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

#include <vtkCollection.h>
#include <vtkMetaDataSetSequence.h>

#include "v3dView.h"
#include <QInputDialog>
#include <QColorDialog>

class v3dView4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    dtkAbstractData  *data;
    v3dView          *view;
};

v3dView4DInteractor::v3dView4DInteractor(): dtkAbstractViewInteractor(), d(new v3dView4DInteractorPrivate)
{
    d->data = NULL;
    d->view = NULL;
    this->currentTime = 0.0;
}

v3dView4DInteractor::~v3dView4DInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dView4DInteractor::description(void) const
{
    return "v3dView4DInteractor";
}

QStringList v3dView4DInteractor::handled(void) const
{
    return QStringList () << "v3dView";
}

bool v3dView4DInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dView4DInteractor", QStringList() << "v3dView", createV3dView4DInteractor);
}

void v3dView4DInteractor::setData(dtkAbstractData *data)
{
  this->appendData (data);
}

void v3dView4DInteractor::appendData(dtkAbstractData *data)
{
    if (vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()))) {

      this->sequenceList->AddItem (sequence);
      
      //Q_UNUSED( sequence );
      //d->data = data;

      updatePipeline ();
    }
}

void v3dView4DInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}

bool v3dView4DInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->description() == "vtkData4D" ) {

        this->enable ();
        return true;
    }
    return false;
}

void v3dView4DInteractor::enable(void)
{
  dtkWarning() << "enabling v3dView4DInteractor" ;
  if (this->enabled())
    return;
  updatePipeline ();
  dtkAbstractViewInteractor::enable();
  dtkWarning() << "enabled " ;

}


void v3dView4DInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view)
    {
      for (int i=0; i<sequenceList->GetNumberOfItems(); i++)
      {
	vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(sequenceList->GetItemAsObject(i));
	if (!sequence)
	  continue;
	
	// d->view->view2d ()->RemoveDataset (sequence->GetDataSet());
	// d->view->view3d ()->RemoveDataset (sequence->GetDataSet());
      }
    }
    dtkAbstractViewInteractor::disable();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dView4DInteractor(void)
{
    return new v3dView4DInteractor;
}

void v3dView4DInteractor::updatePipeline (void)
{
  dtkWarning() << "v3dView4DInteractor::updatePipeline()";
  
  for (int i=0; i<sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;

    dtkWarning() << "sequence ()" << i;
    switch (sequence->GetType())
    {
	case vtkMetaDataSet::VTK_META_IMAGE_DATA:
	  d->view->view2d()->SetInput (vtkImageData::SafeDownCast (sequence->GetDataSet()));
	  d->view->view3d()->SetInput (vtkImageData::SafeDownCast (sequence->GetDataSet()));
	  dtkWarning() << "sequence ()" << i << " is an image";
	  break;
	case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	  d->view->view2d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));
	  d->view->view3d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));
	  dtkWarning() << "sequence ()" << i << " is a mesh and has been added";
	  break;  
	default:
	  break;
    }
  }
}

void v3dView4DInteractor::setCurrentTime (double time)
{
  if (!d->view)
    return;
  
  // double range[2] = {0,0};
  // this->getSequencesTimeRange(range);

  // time = std::min (range[1], time);
  // time = std::max (range[0], time);

  this->currentTime = time;

  for (int i=0; i<this->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(sequenceList->GetItemAsObject (i));
    sequence->UpdateToTime (time);
  }

  d->view->view2d()->Modified();
  d->view->view3d()->Modified();
  d->view->view2d()->Render();
  d->view->view3d()->Render();
}

