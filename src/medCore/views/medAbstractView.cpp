/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractView.h"

#include <QPair>

#include <dtkCore/dtkAbstractData.h>
#include <medAbstractData.h>

#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medViewFactory.h>

class medAbstractViewPrivate
{
public:

    QVector2D  pan;
    double     zoom;
    bool    closable;

//    QList<medAbstractNavigator*> extraNavigators;
//    medAbstractViewNavigator* primaryNavigator;
};

medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;
    d->closable = true;

    this->getNavigators();
}

medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
}

//virtual medAbstractViewNavigator* medAbstractView::primaryNavigator() const
//{
//    return d->primaryNavigator
//}
//virtual QList<medAbstractNavigator*> medAbstractView::extraNavigator() const
//{
//    return d->extraNavigators;
//}

//virtual void medAbstractView::getPrimaryNavigator()
//{
//    medViewFactory* factory = medViewFactory::instance();
//    QStringList viewNavigatorsId = factory->navigatorsAbleToHandle(medAbstractViewNavigator::implementationOf(), this->identifier());
//    if(viewNavigatorsId.isEmpty())
//    {
//        qWarning() << "Unable to find primary navigator for: " << this->description();
//        return;
//    }

//    d->primaryNavigator = factory->createNavigator(medAbstractViewNavigator::implementationOf(),
//                                                       iewNavigatorsId.first(),
//                                                       this);
//}

//virtual void medAbstractView::getExtraNavigator()
//{
//    medViewFactory* factory = medViewFactory::instance();
//    QStringList navigatorsId = factory->navigatorsAbleToHandle(medAbstractNavigator::implementationOf(),
//                                                               this->identifier());
//    foreach(QString nId, navigatorsId)
//        d->extraNavigators << factory->createNavigator<medAbstractNavigator *>(medAbstractNavigator::implementationOf(),
//                                                                          nId,
//                                                                          this);
//}


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
    if  (d->zoom == zoom)
        return;

    d->zoom = zoom;

    this->primaryNavigator()->setZoom(zoom);
    emit zoomChanged (zoom);
}

double medAbstractView::zoom(void) const
{
    return d->zoom;
}

void medAbstractView::setPan (const QVector2D &pan)
{
    if ( d->pan == pan )
        return;

    d->pan = pan;

    this->primaryNavigator()->setPan(pan);
    emit panChanged (pan);
}

QVector2D medAbstractView::pan(void) const
{
    return d->pan;
}
