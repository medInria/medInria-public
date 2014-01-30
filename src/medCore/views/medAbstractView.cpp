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

#include <medAbstractExtraNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medViewFactory.h>

class medAbstractViewPrivate
{
public:
    bool    closable;

//    QList<medAbstractExtraNavigator*> extraNavigators;
//    medAbstractViewNavigator* primaryNavigator;
};

medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->closable = true;
}

medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
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

