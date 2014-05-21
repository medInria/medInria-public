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

    medStringListParameter *linkParameter;
};

medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->closable = true;

    d->primaryInteractor = NULL;
    d->primaryNavigator = NULL;

    d->linkParameter = NULL;  
}

medAbstractView::~medAbstractView( void )
{
    emit closed();
    delete d;
    d = NULL;
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
        interactor->setData(data);
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
            interactor->setData(data);
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


medStringListParameter* medAbstractView::linkParameter()
{
    if(!d->linkParameter)
    {
        d->linkParameter = new medStringListParameter("Link view", this);
        d->linkParameter->addItem("None", medStringListParameter::createIconFromColor("transparent"));
        d->linkParameter->addItem("View group 1", medStringListParameter::createIconFromColor("red"));
        d->linkParameter->addItem("View group 2", medStringListParameter::createIconFromColor("green"));
        d->linkParameter->addItem("View group 3", medStringListParameter::createIconFromColor("blue"));

        QString tooltip = QString(tr("Link View properties ("));
        foreach(medAbstractParameter *param, this->navigatorsParameters())
            tooltip += param->name() + ", ";
        tooltip += ")";
        d->linkParameter->setToolTip(tooltip);

        connect(d->linkParameter, SIGNAL(valueChanged(QString)), this, SLOT(link(QString)));

        d->linkParameter->setValue("None");
    }
    return d->linkParameter;
}

void medAbstractView::link(QString pool)
{
    unlink();

    if(pool!="None")
    {
        foreach(medAbstractParameter *param, this->navigatorsParameters())
            medParameterPoolManager::instance()->linkParameter(param, pool);
    }
}

void medAbstractView::unlink()
{
    foreach(medAbstractParameter *param, this->navigatorsParameters())
        medParameterPoolManager::instance()->unlinkParameter(param);
}

QList<medAbstractParameter*> medAbstractView::navigatorsParameters()
{
    QList<medAbstractParameter*>  params;
    params.append(this->primaryNavigator()->linkableParameters());

    foreach(medAbstractNavigator* nav,  this->extraNavigators())
        params.append(nav->linkableParameters());

    return params;
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
