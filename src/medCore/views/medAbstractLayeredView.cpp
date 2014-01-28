#include "medAbstractLayeredView.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <medAbstractData.h>


class medAbstractLayeredViewPrivate
{
public:
    int currentLayer;
    QList < dtkSmartPointer <medAbstractData> > layersDataList;

};


medAbstractLayeredView::medAbstractLayeredView(QObject *parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->currentLayer = 0;
}

void medAbstractLayeredView::addLayer(medAbstractData *data)
{
    if(!data)
    {
        qWarning() << "Attempt to add a NULL data to the view: " << this;
        return;
    }

    if ( this->contains(data))
    {
        qDebug() << "Attempt to add twice the same data to the view: " << this;
        return;
    }


    QString dataType(data->identifier());

    foreach(dtkAbstractViewInteractor *interactor, this->interactors())
    {
        this->enableInteractor(interactor->identifier());
        if (interactor->enabled())
            interactor->setData(data);
    }

    if(!this->isDataTypeHandled(dataType))
    {
        qWarning()<<"Unable to find interactor to handle the data type: " << data->identifier();
        return;
    }

    this->addDataType(data->identifier());
    d->layersDataList.append(data);

    int layer = this->layersCount() - 1;
    this->setCurrentLayer(layer);

    emit dataAdded(data);
    emit dataAdded(data, layer);
}

bool medAbstractLayeredView::removeLayer(medAbstractData *data)
{
    int res = d->layersDataList.removeAll(data);
    return res > 0;
}

void medAbstractLayeredView::removeLayerAt(unsigned int layer)
{
    d->layersDataList.removeAt(layer);
}

void medAbstractLayeredView::insertLayer(unsigned int layer, medAbstractData *data)
{
    d->layersDataList.insert(layer, data);
}

void medAbstractLayeredView::moveLayer(unsigned int fromLayer, unsigned int toLayer)
{
    d->layersDataList.move(fromLayer, toLayer);
}

medAbstractData * medAbstractLayeredView::dataAtLayer(unsigned int layer) const
{
    if (!(layer < d->layersDataList.size()) && (layer > -1))
    {
        qWarning() << "Unable to retreive data at layer:"<<layer<<"from view: "<<this;
        return NULL;
    }

    return d->layersDataList[layer];
}

bool medAbstractLayeredView::contains(medAbstractData * data) const
{
    return d->layersDataList.contains(data);
}

unsigned int medAbstractLayeredView::layersCount() const
{
    return d->layersDataList.count();
}


void medAbstractLayeredView::setVisibility(bool visibility, int layer)
{
    emit visibilityChanged(visibility, layer);
}

bool medAbstractLayeredView::visibility(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return true;
}

void medAbstractLayeredView::setCurrentLayer(int layer)
{
    d->currentLayer = layer;
}

int medAbstractLayeredView::currentLayer(void) const
{
    return d->currentLayer;
}
