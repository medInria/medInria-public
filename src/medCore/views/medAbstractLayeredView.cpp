#include <medAbstractLayeredView.h>

#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractLayeredViewInteractor.h>
#include <medLayeredViewFactory.h>


class medAbstractLayeredViewPrivate
{
public:
    QList <dtkSmartPointer<medAbstractData> > layersDataList;
    QHash<dtkSmartPointer<medAbstractData>,  medAbstractLayeredViewInteractor*> primaryIntercatorsHash;
    QHash<dtkSmartPointer<medAbstractData>,  QList<medAbstractInteractor*> > extraIntercatorsHash;

    medAbstractLayeredViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;
    unsigned int currentLayer;

};

medAbstractLayeredView::medAbstractLayeredView(QObject *parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->primaryNavigator = NULL;
}

medAbstractLayeredView::~medAbstractLayeredView()
{
    delete d;
}

void medAbstractLayeredView::removeInteractors(medAbstractData *data)
{
    medAbstractLayeredViewInteractor* pInteractor = d->primaryIntercatorsHash.take(data);
    pInteractor->removeData();
    delete pInteractor;

    QList<medAbstractInteractor*> extraInt =  d->extraIntercatorsHash.take(data);
    foreach(medAbstractInteractor* extra, extraInt)
    {
        delete extra;
    }
    extraInt.clear();
}

void medAbstractLayeredView::initialiseInteractors(medAbstractData *data)
{
    // primary

    medLayeredViewFactory* factory = medLayeredViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
         d->primaryIntercatorsHash.insert(data, NULL);
    }
    else
    {
        medAbstractLayeredViewInteractor* interactor = factory->createInteractor(primaryInt.first(), this);
        interactor->setData(data);
        d->primaryIntercatorsHash.insert(data, interactor);
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
        d->extraIntercatorsHash.insert(data, extraIntList);
    }
}

void medAbstractLayeredView::initialiseNavigators()
{
    // primary
    medLayeredViewFactory* factory = medLayeredViewFactory::instance();
    QStringList primaryNav = factory->navigatorsAbleToHandle(this->identifier());
    if(primaryNav.isEmpty())
    {
        qWarning() << "Unable to find any primary navigator for: " << this->identifier();
        d->primaryNavigator = NULL;

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
}

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(medAbstractData* data)
{
    if(d->primaryIntercatorsHash.isEmpty())
        return NULL;

    return d->primaryIntercatorsHash.value(data);
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(medAbstractData* data)
{
    return d->extraIntercatorsHash.value(data);
}

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(unsigned int layer)
{
    return d->primaryIntercatorsHash.value(this->data(layer));
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(unsigned int layer)
{
    return d->extraIntercatorsHash.value(this->data(layer));
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

QList<dtkSmartPointer<medAbstractData> > medAbstractLayeredView::data() const
{
    return d->layersDataList;
}

unsigned int medAbstractLayeredView::layer(medAbstractData * data)
{
    return d->layersDataList.indexOf(data);
}

void medAbstractLayeredView::removeData(medAbstractData *data)
{
    this->removeInteractors(data);
    int layer = this->layer(data);
    d->layersDataList.removeAll(data);

    emit layerRemoved(layer);
    if(d->layersDataList.count() == 0)
        this->~medAbstractLayeredView();
    else if (layer == d->layersDataList.count())
        this->setCurrentLayer(layer -1);
    else
        this->setCurrentLayer(layer);
}

void medAbstractLayeredView::removeLayer(unsigned int layer)
{
    this->removeData(this->data(layer));
}

void medAbstractLayeredView::removeLayer()
{
    this->removeData(this->data(d->currentLayer));
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
    this->initialiseInteractors(data);
    emit layerAdded(layer);

    this->setCurrentLayer(layer);
    if(this->layersCount() == 1)
        this->reset();
}

void medAbstractLayeredView::moveLayer(unsigned int fromLayer, unsigned int toLayer)
{
    d->layersDataList.move(fromLayer, toLayer);
    //TODO I'm not sure we really want this. find out which signal emit. - RDE.
}

medAbstractData * medAbstractLayeredView::data(unsigned int layer) const
{
    if (layer > (unsigned int)d->layersDataList.size())
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

QImage& medAbstractLayeredView::generateThumbnail(const QSize &size)
{
    medAbstractLayeredViewInteractor *primaryInteractor = this->primaryInteractor(this->data(d->currentLayer));
    if(!primaryInteractor)
    {
        qWarning()<< "unable to find any primary interactor for view"  <<this->identifier() << 'and data' << this->data(d->currentLayer)->identifier();
        QImage img;
        return img;
    }
    return this->primaryInteractor(this->data(d->currentLayer))->generateThumbnail(size);
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
