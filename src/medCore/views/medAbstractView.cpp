/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractView.h>

#include <QPair>

#include <dtkCore/dtkAbstractData.h>
#include <medAbstractData.h>

#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medViewFactory.h>

class medAbstractViewPrivate
{
public:
    bool    closable;

    medAbstractViewInteractor* primaryIntercator;
    QList<medAbstractInteractor*>  extraIntercators;

    medAbstractViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;
};

medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->closable = true;

    d->primaryIntercator = NULL;
    d->primaryNavigator = NULL;
}

medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
}

void medAbstractView::removeInteractors(medAbstractData *data)
{
    delete d->primaryIntercator;
    d->primaryIntercator = NULL;
    d->extraIntercators.clear();
}

void medAbstractView::initialiseInteractors(medAbstractData *data)
{
    // primary

    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
         d->primaryIntercator = NULL;
    }
    else
    {
        medAbstractViewInteractor* interactor = factory->createInteractor(primaryInt.first(), this);
        interactor->setData(data);
        d->primaryIntercator = interactor;
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
            extraIntList << interactor;
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
        }
        d->extraIntercators = extraIntList;
    }
}

void medAbstractView::initialiseNavigators()
{
    // primary
    medViewFactory* factory = medViewFactory::instance();
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
    }

    // extra
    QStringList extraNav = factory->additionalNavigatorsAbleToHandle(this->identifier());
    if(!extraNav.isEmpty())
    {
        foreach (QString n, extraNav)
        {
            medAbstractNavigator *nav = factory->createAdditionalNavigator(n, this);
            connect(this, SIGNAL(orientationChanged()), nav, SLOT(updateWidgets()));
            d->extraNavigators << nav;
        }
    }
}

medAbstractViewInteractor* medAbstractView::primaryInteractor(medAbstractData* data)
{
    DTK_UNUSED(data);
    return d->primaryIntercator;
}

QList<medAbstractInteractor*> medAbstractView::extraInteractors(medAbstractData* data)
{
    DTK_UNUSED(data);
    return d->extraIntercators;
}


medAbstractViewNavigator* medAbstractView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractView::extraNavigators()
{
    return d->extraNavigators;
}

void medAbstractView::setZoom (double zoom)
{
    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return

    nav->setZoom(zoom);
    emit zoomChanged(zoom);
}

double medAbstractView::zoom(void)
{
    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return 1;

    return nav->zoom();
}

void medAbstractView::setPan (const QVector2D &pan)
{

    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return

    nav->setPan(pan);
    emit panChanged(pan);
}

QVector2D medAbstractView::pan(void)
{
    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return QVector2D(0.0,0.0);

    return nav->pan();
}

QList<medAbstractParameter*> medAbstractView::navigatorsParameters()
{
    QList<medAbstractParameter*>  params;
    params.append(this->primaryNavigator()->parameters());

    foreach(medAbstractNavigator* nav,  d->extraNavigators)
    {
        params.append(nav->parameters());
    }
    return params;
}

bool medAbstractView::eventFilter(QObject * obj, QEvent * event)
{
    if(obj == this->viewWidget())
    {
        if (event->type() == QEvent::FocusIn)
            emit selectedRequest(true);
        else if (event->type() == QEvent::FocusOut)
            emit selectedRequest(false);
    }
    return dtkAbstractView::eventFilter(obj, event);
}

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/

void medAbstractView::copy(const dtkAbstractObject& other)
{
    DTK_UNUSED(other);
}

void   medAbstractView::link(dtkAbstractView *other)
{
    DTK_UNUSED(other);
}

void medAbstractView::unlink(dtkAbstractView *other)
{
    DTK_UNUSED(other);
}

void   medAbstractView::select(dtkAbstractData *data)
{
    DTK_UNUSED(data);
}

void medAbstractView::unselect(dtkAbstractData *data)
{
    DTK_UNUSED(data);
}

void medAbstractView::setStereo(bool on)
{
    DTK_UNUSED(on);
}

void medAbstractView::setView(void *view)
{
    DTK_UNUSED(view);
}

void medAbstractView::setData(dtkAbstractData *data)
{
    DTK_UNUSED(data);
}

void medAbstractView::setData(dtkAbstractData *data, int inputId)
{
    DTK_UNUSED(data); DTK_UNUSED(inputId);
}

void medAbstractView::setBackgroundColor(int red, int green, int blue)
{
    DTK_UNUSED(red);DTK_UNUSED(green);DTK_UNUSED(blue);
}

void medAbstractView::setBackgroundColor(double red, double green, double blue)
{
    DTK_UNUSED(red);DTK_UNUSED(green);DTK_UNUSED(blue);
}

void *medAbstractView::view(void)
{
    return 0;
}

void *medAbstractView::data(void)
{
    return 0;
}

void *medAbstractView::data(int channel)
{
    DTK_UNUSED(channel);
    return 0;
}

bool medAbstractView::stereo(void)
{
    return false;
}

void  medAbstractView::clear(void)
{}

void  medAbstractView::reset(void)
{}

void medAbstractView::update(void)
{}

void medAbstractView::close(void)
{}

void medAbstractView::showFullScreen(void)
{}

void medAbstractView::showMinimized(void)
{}

void medAbstractView::showMaximized(void)
{}

void medAbstractView::showNormal(void)
{}

void medAbstractView::show(void)
{}

void medAbstractView::resize(int width, int height)
{
    DTK_UNUSED(width);DTK_UNUSED(height);
}

void medAbstractView::addAnimator  (dtkAbstractViewAnimator   *animator)
{
    DTK_UNUSED(animator);
}

void medAbstractView::addNavigator (dtkAbstractViewNavigator  *navigator)
{
    DTK_UNUSED(navigator);
}

void medAbstractView::addInteractor(dtkAbstractViewInteractor *interactor)
{
    DTK_UNUSED(interactor);
}

void medAbstractView::enableAnimator(const QString& animator)
{
    DTK_UNUSED(animator);
}

void medAbstractView::disableAnimator(const QString& animator)
{
    DTK_UNUSED(animator);
}

void medAbstractView::enableNavigator(const QString& navigator)
{
    DTK_UNUSED(navigator);
}

void medAbstractView::disableNavigator(const QString& navigator)
{
    DTK_UNUSED(navigator);
}

void medAbstractView::enableInteractor(const QString& interactor)
{
    DTK_UNUSED(interactor);
}

void medAbstractView::disableInteractor(const QString& interactor)
{
    DTK_UNUSED(interactor);
}

dtkAbstractViewAnimator   *medAbstractView::animator  (const QString& type)
{
    DTK_UNUSED(type);
    return 0;
}

dtkAbstractViewNavigator  *medAbstractView::navigator (const QString& type)
{
    DTK_UNUSED(type);
    return 0;
}

dtkAbstractViewInteractor *medAbstractView::interactor(const QString& type)
{
    DTK_UNUSED(type);
    return 0;
}

QList<dtkAbstractViewAnimator   *> medAbstractView::animators(void) const
{
    return QList<dtkAbstractViewAnimator   *>();
}

QList<dtkAbstractViewNavigator  *> medAbstractView::navigators(void) const
{
    return QList<dtkAbstractViewNavigator *>();
}

QList<dtkAbstractViewInteractor *> medAbstractView::interactors(void) const
{
    return QList<dtkAbstractViewInteractor *>();
}

void   medAbstractView::initialize(void)
{}

void medAbstractView::uninitialize(void)
{}

void medAbstractView::setHeadPosition(dtkVector3D<double> position)
{
    DTK_UNUSED(position);
}

void medAbstractView::setHeadOrientation(dtkQuaternion<double> orientation)
{
    DTK_UNUSED(orientation);
}

void medAbstractView::setUpperLeft(dtkVector3D<double> position)
{
    DTK_UNUSED(position);
}

void medAbstractView::setLowerLeft(dtkVector3D<double> position)
{
    DTK_UNUSED(position);
}

void medAbstractView::setLowerRight(dtkVector3D<double> position)
{
    DTK_UNUSED(position);
}
