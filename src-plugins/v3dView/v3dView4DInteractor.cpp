/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dView4DInteractor.h"

#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCollection.h>
#include <vtkProperty.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkMetaDataSetSequence.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkLog/dtkLog.h>

#include <medMessageController.h>

#include "v3dView.h"

template <typename TYPE>
bool AppendImageSequence(dtkAbstractData* data,v3dView* view,vtkCollection* sequenceList,QList<dtkAbstractData*> dataList) {
    if (itk::Image<TYPE,4>* image = dynamic_cast<itk::Image<TYPE,4>*>(static_cast<itk::Object*>(data->data()))) {
        unsigned int layer = view->layerCount();
        if (layer==1 && !view->view2d()->GetInput())
            layer = 0;
        vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();
        sequence->SetITKDataSet<TYPE>(image);
        sequenceList->AddItem(sequence);
        vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet(0U));
        vtkImageData*     vtkimage  = vtkImageData::SafeDownCast(sequence->GetDataSet());
        view->view2d()->SetInput(vtkimage,metaimage->GetOrientationMatrix(),layer);
        view->view3d()->SetInput(vtkimage,metaimage->GetOrientationMatrix(),layer);
        sequence->Delete();
        dataList.push_back(data);
        return true;
    }
    return false;
}

class v3dView4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    v3dView          *view;
    vtkCollection* sequenceList;
    QList<dtkAbstractData*> dataList;
};

v3dView4DInteractor::v3dView4DInteractor(): med4DAbstractViewInteractor(), d(new v3dView4DInteractorPrivate)
{
    d->view = NULL;
    this->currentTime = 0.0;
    d->sequenceList = vtkCollection::New();
}

v3dView4DInteractor::~v3dView4DInteractor()
{
  this->disable();
  d->sequenceList->Delete();

  delete d;
  d = 0;
}

QString v3dView4DInteractor::description() const
{
    return tr("Interactor displaying 4d images (temporal sequence)");
}


QString v3dView4DInteractor::identifier() const
{
    return "v3dView4DInteractor";
}

QStringList v3dView4DInteractor::handled() const
{
  return QStringList () << v3dView::s_identifier();
}

bool v3dView4DInteractor::registered()
{
  return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dView4DInteractor", QStringList() << v3dView::s_identifier(), createV3dView4DInteractor);
}

void v3dView4DInteractor::setData(dtkAbstractData *data)
{
  //  this->appendData (data);
}

void v3dView4DInteractor::appendData(dtkAbstractData *data)
{
  if (d->dataList.contains (data))
    return;
  if (data->identifier() == "vtkDataMesh4D" )
  {
    vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()));
    //vtkProperty *prop = vtkProperty::New();
    if (!sequence || d->sequenceList->IsItemPresent (sequence))
      return;

    switch (sequence->GetType())
    {
	case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	case vtkMetaDataSet::VTK_META_VOLUME_MESH:

      //prop->SetColor(1,0,1);

      //d->view->view2d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));//->SetProperty(prop);
	  //d->view->view3d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));//->SetProperty(prop);
	  d->sequenceList->AddItem (sequence);
	  d->dataList.push_back (data);

	  break;
	default:
	  break;
    }
  }
  else if (data->identifier().contains("Image") && data->identifier().contains ("4")) {

    if (!(AppendImageSequence<char>(data,d->view,d->sequenceList,d->dataList)           ||
          AppendImageSequence<unsigned char>(data,d->view,d->sequenceList,d->dataList)  ||
          AppendImageSequence<short>(data,d->view,d->sequenceList,d->dataList)          ||
          AppendImageSequence<unsigned short>(data,d->view,d->sequenceList,d->dataList) ||
          AppendImageSequence<float>(data,d->view,d->sequenceList,d->dataList)          ||
          AppendImageSequence<double>(data,d->view,d->sequenceList,d->dataList))) {
        // emit showError(this, tr ("cannot append data to 4D interactor (unhandled type: ") + data->identifier(), 5000);
    }
  }
  else
  {
    // emit showError(this, tr ("cannot append data to 4D interactor (unhandled type: ") + data->identifier(), 5000);
  }

}


void v3dView4DInteractor::setView(dtkAbstractView *view)
{
  if (v3dView *v3dview = dynamic_cast<v3dView*>(view) )
  {

      d->view = v3dview;
      connect (view, SIGNAL (dataAdded (dtkAbstractData*)), this, SLOT (onDataAdded (dtkAbstractData*)));
  }
}

void v3dView4DInteractor::onDataAdded(dtkAbstractData *data)
{
  this->appendData(data);
}

bool v3dView4DInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{

  if ( ( data->identifier() == "vtkDataMesh4D" ) ||
       ( data->identifier().contains ("Image") &&
	 data->identifier().contains ("4") ) )
  {
    this->appendData(data);
    this->enable ();
    return true;
  }

  return false;
}

void v3dView4DInteractor::enable()
{
  if (this->enabled())
    return;

  med4DAbstractViewInteractor::enable();
}

void v3dView4DInteractor::disable()
{
    if (!this->enabled())
        return;

    if (d->view)
    {
      for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
      {
	vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject(i));
	if (!sequence)
	  continue;

	// d->view->view2d ()->RemoveDataset (sequence->GetDataSet());
	// d->view->view3d ()->RemoveDataset (sequence->GetDataSet());
      }
    }

    med4DAbstractViewInteractor::disable();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dView4DInteractor()
{
  return new v3dView4DInteractor;
}

void v3dView4DInteractor::setCurrentTime (double time)
{
  if (!d->view)
    return;

  double range[2] = {0,0};
  this->sequencesRange(range);

  time = std::min (range[1], time);
  time = std::max (range[0], time);

  this->currentTime = time;

  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    sequence->UpdateToTime (time);
  }

  d->view->currentView()->Modified();
  d->view->currentView()->Render();
}

void v3dView4DInteractor::sequencesRange (double* range)
{
  if (!d->sequenceList->GetNumberOfItems())
  {
    range[0] = 0;
    range[1] = 1.0;
    return;
  }

  double mintime = 3000;
  double maxtime = -3000;

  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    mintime = std::min (mintime, sequence->GetMinTime());
    maxtime = std::max (maxtime, sequence->GetMaxTime());
  }

  range[0] = mintime;
  range[1] = maxtime;
}

double v3dView4DInteractor::sequencesMinTimeStep()
{
  if (!d->sequenceList->GetNumberOfItems())
  {
    return 0.01;
  }

  double step = 3000;

  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    double mintime = sequence->GetMinTime();
    double maxtime = sequence->GetMaxTime();
    double number = sequence->GetNumberOfMetaDataSets();

    step = std::min ( step, (maxtime - mintime)/(number - 1.0) );
  }

  return step;
}

