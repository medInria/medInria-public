/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMesh4DInteractor.h"

#include <vtkMetaDataSetSequence.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medViewFactory.h>
#include <medTimeLineParameterL.h>


class vtkDataMesh4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    medAbstractData *data;
    medAbstractImageView *view;

    vtkMetaDataSetSequence *sequence;
};


vtkDataMesh4DInteractor::vtkDataMesh4DInteractor(medAbstractView* parent): vtkDataMeshInteractor(parent),
    d(new vtkDataMesh4DInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);
    d->data = NULL;
}

vtkDataMesh4DInteractor::~vtkDataMesh4DInteractor()
{

}

QString vtkDataMesh4DInteractor::description() const
{
    return tr("Interactor displaying 4D Meshes");
}

QString vtkDataMesh4DInteractor::identifier() const
{
    return "vtkDataMesh4DInteractor";
}

QStringList vtkDataMesh4DInteractor::handled() const
{
    return vtkDataMesh4DInteractor::dataHandled();
}

QStringList vtkDataMesh4DInteractor::dataHandled()
{
    QStringList d = QStringList() << "vtkDataMesh4D";
    return  d;
}

bool vtkDataMesh4DInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<vtkDataMesh4DInteractor>("vtkDataMesh4DInteractor",
                                                                          QStringList () << "medVtkView" <<
                                                                          vtkDataMesh4DInteractor::dataHandled());
}

void vtkDataMesh4DInteractor::setInputData(medAbstractData *data)
{
    vtkDataMeshInteractor::setInputData(data);

    if (data->identifier() == "vtkDataMesh4D" )
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
            d->data->setMetaData("SequenceDuration", QString::number(d->sequence->GetMaxTime()));
            d->data->setMetaData("SequenceFrameRate", QString::number((double)d->sequence->GetNumberOfMetaDataSets() /
                                                                           (double)d->sequence->GetMaxTime()));
            break;
        default:
            break;
        }
    }
}

void vtkDataMesh4DInteractor::setCurrentTime (double time)
{
    if(d->sequence->GetTime() == time)
        return;

    d->sequence->UpdateToTime(time);
}

