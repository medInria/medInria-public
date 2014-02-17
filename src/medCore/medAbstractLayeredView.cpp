#include "medAbstractLayeredView.h"

#include <dtkCore/dtkAbstractData.h>
#include <medAbstractData.h>


class medAbstractLayeredViewPrivate
{
public:
    int currentLayer;
    QList < dtkSmartPointer <medAbstractData> > layersDataList;

};


medAbstractLayeredView::medAbstractLayeredView(medAbstractView * parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->currentLayer = 0;
}

void medAbstractLayeredView::addLayer(medAbstractData *data)
{
    dtkAbstractView::setData(data);

    if(data)
      addDataType(data->identifier());

    return d->layersDataList.append(data);
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

medAbstractData * medAbstractLayeredView::layerData(unsigned int layer)
{
    if ((layer < d->layersDataList.size()) && (layer >= 0))
        return d->layersDataList[layer];

    return NULL;
}

bool medAbstractLayeredView::contains(medAbstractData * data)
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
