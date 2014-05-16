#include <medAbstractImageView.h>

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>
#include <medViewFactory.h>

class medAbstractImageViewPrivate
{
public:
    QHash<dtkSmartPointer<medAbstractData>,  medAbstractImageViewInteractor*> primaryInteractorsHash;
    QHash<dtkSmartPointer<medAbstractData>,  QList<medAbstractInteractor*> > extraInteractorsHash;

    medAbstractImageViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;
};

medAbstractImageView::medAbstractImageView(QObject *parent) : medAbstractLayeredView(parent),
    d(new medAbstractImageViewPrivate)
{
    d->primaryNavigator = NULL;
}

medAbstractImageView::~medAbstractImageView()
{
    delete d;
}

medAbstractImageViewInteractor* medAbstractImageView::primaryInteractor(medAbstractData* data)
{
    if(d->primaryInteractorsHash.isEmpty())
        return NULL;

    return d->primaryInteractorsHash.value(data);
}

QList<medAbstractInteractor*> medAbstractImageView::extraInteractors(medAbstractData* data)
{
    return d->extraInteractorsHash.value(data);
}

medAbstractImageViewInteractor* medAbstractImageView::primaryInteractor(unsigned int layer)
{
    return d->primaryInteractorsHash.value(this->layerData(layer));
}

QList<medAbstractInteractor*> medAbstractImageView::extraInteractors(unsigned int layer)
{
    return d->extraInteractorsHash.value(this->layerData(layer));
}

medAbstractImageViewNavigator* medAbstractImageView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractImageView::extraNavigators()
{
    return d->extraNavigators;
}

void medAbstractImageView::removeInteractors(medAbstractData *data)
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

bool medAbstractImageView::initialiseInteractors(medAbstractData *data)
{
    // primary

    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
        return false;
    }
    else
    {
        medAbstractImageViewInteractor* interactor = factory->createInteractor<medAbstractImageViewInteractor>(primaryInt.first(), this);
        connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
        connect(this, SIGNAL(currentLayerChanged()), interactor, SLOT(updateWidgets()));
        interactor->setData(data);
        d->primaryInteractorsHash.insert(data, interactor);
    }

    // extra
    QStringList extraInt = factory->additionalInteractorsAbleToHandle(this->identifier(), data->identifier());
    if(!extraInt.isEmpty())
    {
        QList<medAbstractInteractor*> extraIntList;
        foreach (QString i, extraInt)
        {
            medAbstractInteractor* interactor = factory->createAdditionalInteractor(i, this);
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
            connect(this, SIGNAL(currentLayerChanged()), interactor, SLOT(updateWidgets()));
            interactor->setData(data);
            extraIntList << interactor;
        }
        d->extraInteractorsHash.insert(data, extraIntList);
    }
    return true;
}

bool medAbstractImageView::initialiseNavigators()
{
    // primary
    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryNav = factory->navigatorsAbleToHandle(this->identifier());
    if(primaryNav.isEmpty())
    {
        qWarning() << "Unable to find any primary navigator for: " << this->identifier();
        return false;
    }
    else
    {
        d->primaryNavigator = factory->createNavigator<medAbstractImageViewNavigator>(primaryNav.first(), this);
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


void medAbstractImageView::setOrientation(medImageView::Orientation orientation)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setOrientation(orientation);
    emit orientationChanged();
}


medCompositeParameter *medAbstractImageView::cameraParameter()
{
    medAbstractImageViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        qWarning() << "Unable to retrieve primary navigator";
        return NULL;
    }

    return pNavigator->cameraParameter();
}

medAbstractVector3DParameter *medAbstractImageView::positionBeinViewedParameter()
{
    medAbstractImageViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        qWarning() << "Unable to retrieve primary navigator";
        return NULL;
    }

    return pNavigator->positionBeingViewedParameter();
}

medDoubleParameter *medAbstractImageView::opacityParameter(unsigned int layer)
{
    medAbstractImageViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        qWarning() << "Unable to retrieve primary interactor for layer:" << layer;
        return NULL;
    }

    return pInteractor->opacityParameter();
}

medCompositeParameter *medAbstractImageView::windowLevelParameter(unsigned int layer)
{
    medAbstractImageViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        qWarning() << "Unable to retrieve primary interactor for layer:" << layer;
        return NULL;
    }

    return pInteractor->windowLevelParameter();
}

medImageView::Orientation medAbstractImageView::orientation()
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return medImageView::VIEW_ORIENTATION_AXIAL;

    return nav->orientation();
}
