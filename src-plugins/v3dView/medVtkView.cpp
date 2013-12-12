#include <medVtkView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

medVtkView::medVtkView()
{

}

medVtkView::~medVtkView()
{
    
}

QString medVtkView::s_identifier()
{
    return "medVtkView";
}

QString medVtkView::identifier() const
{
    return medVtkView::s_identifier();
}

bool medVtkView::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewType ( medVtkView::s_identifier(), createMedVtkView );
}

dtkAbstractView *createMedVtkView()
{
    return new medVtkView;
}

void medVtkView::setData (dtkAbstractData *data, int layer)
{
    if(!data)
        return;

    if (layer < 0)
        layer = 0;
    
    qDebug() << "Adding layer" << layer << data->identifier();
    
    if (layer < layersDataList.size())
        layersDataList[layer] = data;
    else
        layersDataList.append(data);
        
    v3dView::setData(data,layer);
}

dtkAbstractData * medVtkView::layerData(int layer)
{
    if ((layer < layersDataList.size()) && (layer >= 0))
        return layersDataList[layer];

    return NULL;
}

unsigned int medVtkView::numberOfLayers()
{
    return layersDataList.size();
}
