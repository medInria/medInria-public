/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractView.h>

#include <QPair>

#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer>

#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medViewFactory.h>
#include <medStringListParameter.h>
#include <medParameterPoolManager.h>

class medAbstractViewPrivate
{
public:
    bool    closable;

    medAbstractViewInteractor* primaryInteractor;
    QList<medAbstractInteractor*>  extraInteractors;

    medAbstractViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;

    // Smart pointed.
    // dtkSmartPointer should only be used in views, process and dataManager.
    dtkSmartPointer<medAbstractData> data;
};


medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->closable = true;

    d->data = NULL;
    d->primaryInteractor = NULL;
    d->primaryNavigator = NULL;
}

medAbstractView::~medAbstractView( void )
{
    emit closed();
    delete d;
    d = NULL;
}


void medAbstractView::addData(medAbstractData *data)
{
    if(!data)
    {
        qWarning() << "Attempt to add a NULL data to the view: " << this;
        return;
    }

    if(data == d->data)
    {
        qDebug() << "Attempt to set twice the same data to the view: " << this;
        return;
    }

    d->data = data;

    bool initSuccess = this->initialiseInteractors(data);
    if(!initSuccess)
    {
        d->data = NULL;
        return;
    }

    this->reset();
}

void medAbstractView::clear()
{

}

void medAbstractView::removeInteractors(medAbstractData *data)
{
    delete d->primaryInteractor;
    d->primaryInteractor = NULL;
    d->extraInteractors.clear();
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
        interactor->setInputData(data);
        d->primaryInteractor = interactor;
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
            interactor->setInputData(data);
            extraIntList << interactor;
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
        }
        d->extraInteractors = extraIntList;
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
    return d->primaryInteractor;
}

QList<medAbstractInteractor*> medAbstractView::extraInteractors(medAbstractData* data)
{
    DTK_UNUSED(data);
    return d->extraInteractors;
}


medAbstractViewNavigator* medAbstractView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractView::extraNavigators()
{
    return d->extraNavigators;
}

QList<medAbstractNavigator*> medAbstractView::navigators()
{
    QList<medAbstractNavigator*> navigatorsList;
    navigatorsList << this->primaryNavigator() << d->extraNavigators;

    return navigatorsList;
}

medDoubleParameter* medAbstractView::zoomParameter()
{
    medAbstractViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return NULL;
    }

    return pNavigator->zoomParameter();
}

medAbstractVector2DParameter* medAbstractView::panParameter()
{
    medAbstractViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return NULL;
    }

    return pNavigator->panParameter();
}

bool medAbstractView::eventFilter(QObject * obj, QEvent * event)
{
    if(obj == this->viewWidget())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            if(mouseEvent && mouseEvent->button() == Qt::LeftButton)
              emit selectedRequest(true);
        }

    }
    return dtkAbstractView::eventFilter(obj, event);
}

QImage medAbstractView::generateThumbnail(const QSize &size)
{
    emit aboutToBuildThumbnail();
    return this->buildThumbnail(size);
}

QList<medAbstractParameter*> medAbstractView::linkableParameters()
{
    QList<medAbstractParameter*>  params;
    params.append(this->primaryNavigator()->linkableParameters());

    foreach(medAbstractNavigator* nav,  this->extraNavigators())
        params.append(nav->linkableParameters());

    return params;
}
