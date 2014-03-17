#include <medAbstractImageView.h>

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>
#include <medImageViewFactory.h>

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

    medImageViewFactory* factory = medImageViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
        return false;
    }
    else
    {
        medAbstractImageViewInteractor* interactor = factory->createInteractor(primaryInt.first(), this);
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
    medImageViewFactory* factory = medImageViewFactory::instance();
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

void medAbstractImageView::moveToSliceAtPosition (const QVector3D &position)
{
    foreach (medAbstractData *data, this->dataList())
    {
        medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
        if(!inter)
            break;

        inter->moveToSliceAtPosition(position);
    }

    emit positionViewedChanged(position);
}

void medAbstractImageView::moveToSlice (int slice)
{
    foreach (medAbstractData *data, this->dataList())
    {
        medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
        if(!inter)
            break;

        inter->moveToSlice(slice);
    }

    emit sliceChanged(slice);
}

void medAbstractImageView::setOrientation(medImageView::Orientation orientation)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setOrientation(orientation);
    emit orientationChanged();
}

QVector3D medAbstractImageView::positionBeingViewed(void)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return QVector3D(0.0,0.0,0.0);

    return nav->positionBeingViewed();
}

void medAbstractImageView::setDataWindowLevel(medAbstractData *data, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->setWindowLevel(window, level);

    emit windowLevelChanged(window, level, this->layer(data));
}

void medAbstractImageView::setLayerWindowLevel(unsigned int layer, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->setWindowLevel(window, level);

    emit windowLevelChanged(window, level, layer);
}

void medAbstractImageView::setWindowLevel(double window, double level)
{
    this->setLayerWindowLevel(this->currentLayer(), window, level);
}

void medAbstractImageView::dataWindowLevel(medAbstractData *data, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->windowLevel(window, level);
}

void medAbstractImageView::layerWindowLevel(unsigned int layer, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->windowLevel(window, level);
}

void medAbstractImageView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setCamera(position, viewup, focal, parallelScale);

    emit cameraChanged(position, viewup, focal, parallelScale);
}

void medAbstractImageView::camera (QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->camera(position, viewup, focal, parallelScale);
}

void medAbstractImageView::setOpacity(unsigned int layer, double opacity)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->setOpacity(opacity);
}

void medAbstractImageView::setOpacity(medAbstractData *data, double opacity)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->setOpacity(opacity);
}

double medAbstractImageView::opacity(unsigned int layer)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return 1;

    return inter->opacity();
}

double medAbstractImageView::opacity(medAbstractData *data)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return 1;

    return inter->opacity();
}

medImageView::Orientation medAbstractImageView::orientation()
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return medImageView::VIEW_ORIENTATION_AXIAL;

    return nav->orientation();
}
