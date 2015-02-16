/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkViewVtk4DMeshInteractor.h"

#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medTimeLineParameter.h>


class medVtkViewVtk4DMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractData *data;
    medAbstractImageView *view;

    vtkMetaDataSetSequence *sequence;
};


medVtkViewVtk4DMeshInteractor::medVtkViewVtk4DMeshInteractor(medAbstractView* parent): medVtkViewVtkMeshInteractor(parent),
    d(new medVtkViewVtk4DMeshInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);
    d->data = NULL;
}

medVtkViewVtk4DMeshInteractor::~medVtkViewVtk4DMeshInteractor()
{

}

QString medVtkViewVtk4DMeshInteractor::description() const
{
    return tr("Interactor displaying 4D Meshes");
}

QString medVtkViewVtk4DMeshInteractor::identifier() const
{
    return "medVtkViewVtk4DMeshInteractor";
}

QStringList medVtkViewVtk4DMeshInteractor::handled() const
{
    return medVtkViewVtk4DMeshInteractor::dataHandled();
}

QStringList medVtkViewVtk4DMeshInteractor::dataHandled()
{
    QStringList d = QStringList() << "medVtk4DMeshData";
    return  d;
}

bool medVtkViewVtk4DMeshInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewVtk4DMeshInteractor>("medVtkViewVtk4DMeshInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          medVtkViewVtk4DMeshInteractor::dataHandled());
}

void medVtkViewVtk4DMeshInteractor::setInputData(medAbstractData *data)
{
    medVtkViewVtkMeshInteractor::setInputData(data);

    if (data->identifier() == "medVtk4DMeshData" )
    {
        d->data = data;

        vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()));
        //vtkProperty *prop = vtkProperty::New();
        if ( !sequence )
            return;

        switch (sequence->GetType())
        {
        case vtkMetaDataSet::VTK_META_SURFACE_MESH:
        case vtkMetaDataSet::VTK_META_VOLUME_MESH:
            d->sequence = sequence;
            d->data->addMetaData("SequenceDuration", QString::number(d->sequence->GetMaxTime()));
            d->data->addMetaData("SequenceFrameRate", QString::number((double)d->sequence->GetNumberOfMetaDataSets() /
                                                                           (double)d->sequence->GetMaxTime()));
            break;
        default:
            break;
        }
    }
}

void medVtkViewVtk4DMeshInteractor::setCurrentTime (double time)
{
    if(d->sequence->GetTime() == time)
        return;

    d->sequence->UpdateToTime(time);
}

