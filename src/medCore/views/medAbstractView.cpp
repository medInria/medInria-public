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

bool medAbstractView::initialiseInteractors(medAbstractData *data)
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
        medAbstractViewInteractor* interactor = factory->createInteractor<medAbstractViewInteractor>(primaryInt.first(), this);
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
    return true;
}

bool medAbstractView::initialiseNavigators()
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
        d->primaryNavigator = factory->createNavigator<medAbstractViewNavigator>(primaryNav.first(), this);
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
    return true;
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

    foreach(medAbstractNavigator* nav,  this->extraNavigators())
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
        {
            emit selectedRequest(true);
            this->viewWidget()->clearFocus();
        }

    }
    return dtkAbstractView::eventFilter(obj, event);
}


QImage medAbstractView::generateThumbnail(const QSize &size)
{
    emit aboutToBuildThumbnail();
    return this->buildThumbnail(size);
}
