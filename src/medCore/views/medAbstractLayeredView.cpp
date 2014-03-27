#include <medAbstractLayeredView.h>

#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractLayeredViewInteractor.h>
#include <medLayeredViewFactory.h>


class medAbstractLayeredViewPrivate
{
public:
    QList <dtkSmartPointer<medAbstractData> > layersDataList;
    QHash<dtkSmartPointer<medAbstractData>,  medAbstractLayeredViewInteractor*> primaryInteractorsHash;
    QHash<dtkSmartPointer<medAbstractData>,  QList<medAbstractInteractor*> > extraInteractorsHash;

    medAbstractLayeredViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;
    unsigned int currentLayer;

};

medAbstractLayeredView::medAbstractLayeredView(QObject *parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->primaryNavigator = NULL;
    d->currentLayer = 0;
}

medAbstractLayeredView::~medAbstractLayeredView()
{
    delete d;
}

void medAbstractLayeredView::removeInteractors(medAbstractData *data)
{
    medAbstractLayeredViewInteractor* pInteractor = d->primaryInteractorsHash.take(data);
    pInteractor->removeData();
    delete pInteractor;

    QList<medAbstractInteractor*> extraInt =  d->extraInteractorsHash.take(data);
    foreach(medAbstractInteractor* extra, extraInt)
    {
        delete extra;
    }
    extraInt.clear();
}

bool medAbstractLayeredView::initialiseInteractors(medAbstractData *data)
{
    // primary

    medLayeredViewFactory* factory = medLayeredViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
        return false;
    }
    else
    {
        medAbstractLayeredViewInteractor* interactor = factory->createInteractor(primaryInt.first(), this);
        interactor->setData(data);
        d->primaryInteractorsHash.insert(data, interactor);
        connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraInt = factory->additionalInteractorsAbleToHandle(this->identifier(), data->identifier());
    if(!extraInt.isEmpty())
    {
        QList<medAbstractInteractor*> extraIntList;
        foreach (QString i, extraInt)
        {
            medAbstractInteractor* interactor = factory->createAdditionalInteractor(i, this);
            interactor->setData(data);
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
            extraIntList << interactor;
        }
        d->extraInteractorsHash.insert(data, extraIntList);
    }

    return true;
}

bool medAbstractLayeredView::initialiseNavigators()
{
    // primary
    medLayeredViewFactory* factory = medLayeredViewFactory::instance();
    QStringList primaryNav = factory->navigatorsAbleToHandle(this->identifier());
    if(primaryNav.isEmpty())
    {
        qWarning() << "Unable to find any primary navigator for: " << this->identifier();
        return false;

    }
    else
    {
        d->primaryNavigator = factory->createNavigator(primaryNav.first(), this);
        connect(this, SIGNAL(orientationChanged()), d->primaryNavigator, SLOT(updateWidgets()));
        connect(this, SIGNAL(currentLayerChanged()), d->primaryNavigator, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraNav = factory->additionalNavigatorsAbleToHandle(this->identifier());
    if(!extraNav.isEmpty())
    {
        foreach (QString n, extraNav)
        {
            medAbstractNavigator* nav = factory->createAdditionalNavigator(n, this);
            connect(this, SIGNAL(orientationChanged()), nav, SLOT(updateWidgets()));
            connect(this, SIGNAL(currentLayerChanged()), nav, SLOT(updateWidgets()));
            d->extraNavigators << nav;
        }
    }
    return true;
}

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(medAbstractData* data)
{
    if(d->primaryInteractorsHash.isEmpty())
        return NULL;

    return d->primaryInteractorsHash.value(data);
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(medAbstractData* data)
{
    return d->extraInteractorsHash.value(data);
}

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(unsigned int layer)
{
    return d->primaryInteractorsHash.value(this->layerData(layer));
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(unsigned int layer)
{
    return d->extraInteractorsHash.value(this->layerData(layer));
}

medAbstractLayeredViewNavigator* medAbstractLayeredView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractLayeredView::extraNavigators()
{
    return d->extraNavigators;
}

void medAbstractLayeredView::addLayer(medAbstractData *data)
{
    this->insertLayer(d->layersDataList.count(), data);
}

QList<dtkSmartPointer<medAbstractData> > medAbstractLayeredView::dataList() const
{
    return d->layersDataList;
}

unsigned int medAbstractLayeredView::layer(medAbstractData * data)
{
    return d->layersDataList.indexOf(data);
}

void medAbstractLayeredView::removeData(medAbstractData *data)
{
    if(!d->layersDataList.contains(data))
        return;

    this->removeInteractors(data);
    int layer = this->layer(data);
    d->layersDataList.removeAll(data);


    if(d->layersDataList.count() != 0  && layer == d->layersDataList.count())
    {
        this->setCurrentLayer(layer -1);
        emit layerRemoved(layer);
    }
    else if(d->layersDataList.count() != 0)
    {
        this->setCurrentLayer(layer);
        emit layerRemoved(layer);
    }
    else
        this->~medAbstractLayeredView();


}

void medAbstractLayeredView::removeLayer(unsigned int layer)
{
    this->removeData(this->layerData(layer));
}

void medAbstractLayeredView::removeLayer()
{
    this->removeData(this->layerData(d->currentLayer));
}

void medAbstractLayeredView::insertLayer(unsigned int layer, medAbstractData *data)
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

    d->layersDataList.insert(layer, data);

    bool initSuccess = this->initialiseInteractors(data);
    if(!initSuccess)
    {
        d->layersDataList.removeOne(data);
        return;
    }

    this->setCurrentLayer(layer);
    if(this->layersCount() == 1)
        this->reset();

    emit layerAdded(layer);
}

void medAbstractLayeredView::moveLayer(unsigned int fromLayer, unsigned int toLayer)
{
    d->layersDataList.move(fromLayer, toLayer);
    //TODO I'm not sure we really want this. find out which signal emit. - RDE.
}

medAbstractData * medAbstractLayeredView::layerData(unsigned int layer) const
{
    if (layer >= (unsigned int)d->layersDataList.size())
    {
        qWarning() << "Unable to retreive data at layer:" <<layer << "from: "<< this->description();
        return NULL;
    }
    return d->layersDataList[layer];
}

bool medAbstractLayeredView::contains(medAbstractData * data) const
{
    return d->layersDataList.contains(data);
}

bool medAbstractLayeredView::contains(QString identifier) const
{
    foreach(medAbstractData *data, d->layersDataList)
    {
        if(data->identifier() == identifier)
            return true;
    }

    return false;
}

unsigned int medAbstractLayeredView::layersCount() const
{
    return d->layersDataList.count();
}


void medAbstractLayeredView::setVisibility(bool visibility, unsigned int layer)
{
    medAbstractLayeredViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->setVisibility(visibility);
    emit visibilityChanged(visibility, layer);
}

void medAbstractLayeredView::setVisibility(bool visibility)
{
    medAbstractLayeredViewInteractor* inter = this->primaryInteractor(d->currentLayer);
    if(!inter)
        return;

    inter->setVisibility(visibility);
    emit visibilityChanged(visibility, d->currentLayer);
}

bool medAbstractLayeredView::visibility(unsigned int layer)
{
    medAbstractLayeredViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return true;

    return inter->visibility();
}

unsigned int medAbstractLayeredView::currentLayer() const
{
    return d->currentLayer;
}

void medAbstractLayeredView::setCurrentLayer(unsigned int layer)
{
    d->currentLayer = layer;
    emit currentLayerChanged();
}

QImage medAbstractLayeredView::generateThumbnail(const QSize &size)
{
    medAbstractLayeredViewInteractor *primaryInteractor = this->primaryInteractor(this->layerData(d->currentLayer));
    if(!primaryInteractor)
    {
        qWarning()<< "unable to find any primary interactor for view"  <<this->identifier() << "and data" << this->layerData(d->currentLayer)->identifier();
        return QImage();
    }
    return this->primaryInteractor(this->layerData(d->currentLayer))->generateThumbnail(size);
}

//TODO not sure we need this - RDE
QList <medAbstractInteractor*> medAbstractLayeredView::currentInteractors()
{
    QList <medAbstractInteractor*> interactors;

     interactors.append(this->primaryInteractor(d->currentLayer));
    interactors.append(extraInteractors(d->currentLayer));
    return interactors;
}

QList <medAbstractInteractor*>  medAbstractLayeredView::interactors(unsigned int layer)
{
    QList <medAbstractInteractor*> interactors;
    interactors << this->primaryInteractor(layer) << this->extraInteractors(layer);
    return interactors;
}
