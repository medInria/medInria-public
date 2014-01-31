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

    this->initialiseNavigators();
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
        d->primaryNavigator = factory->createNavigator(primaryNav.first(), this);

    // extra
    QStringList extraNav = factory->additionalNavigatorsAbleToHandle(this->identifier());
    if(!extraNav.isEmpty())
    {
        foreach (QString n, extraNav)
        {
               d->extraNavigators << factory->createAdditionalNavigator(n, this);
        }
    }
}

medAbstractViewInteractor* medAbstractView::primaryInteractor(medAbstractData* data)
{
    DTK_UNUSED(data);
    return d->primaryIntercator;
}

QList<medAbstractInteractor*> medAbstractView::extraInteractor(medAbstractData* data)
{
    DTK_UNUSED(data);
    return d->extraIntercators;
}


medAbstractViewNavigator* medAbstractView::primaryNavigator()
{
    d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractView::extraNavigator()
{
    d->extraNavigators;
}

bool medAbstractView::isClosable() const
{
    return d->closable;
}

void medAbstractView::setClosable(bool closable)
{
    d->closable = closable;
}

void medAbstractView::setZoom (double zoom)
{
    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return

    nav->setZoom(zoom);
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
}

QVector2D medAbstractView::pan(void)
{
    medAbstractViewNavigator* nav = this->primaryNavigator();
    if(!nav)
        return QVector2D(0.0,0.0);

    return nav->pan();
}

