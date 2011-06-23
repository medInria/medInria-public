/* medToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 12:57:48 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 254
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */
#include <dtkCore/dtkAbstractData.h>

#include <medCore/medAbstractView.h>

#include "medToolBox.h"
#include "medToolBoxHeader.h"
#include "medToolBoxBody.h"
#include "medToolBoxTab.h"

#include <dtkCore/dtkGlobal.h>

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
    bool isMinimized;

    QStringList validDataTypes;
    bool isContextVisible;
public:
    QBoxLayout *layout;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    //d->view = 0;

    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);
    d->isContextVisible = false;

    d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header);
    d->layout->addWidget(d->body);
    d->isMinimized = false;
    connect(d->header,SIGNAL(triggered()),this,SLOT(switchMinimize()));

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d;

    d = NULL;
}

void medToolBox::setTabWidget(medToolBoxTab *tab)
{
    d->body->setTabWidget(tab);
}

void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

medToolBoxHeader *medToolBox::header(void) const
{
    return d->header;
}

medToolBoxBody *medToolBox::body(void) const
{
    return d->body;
}

void medToolBox::update(dtkAbstractView *view)
{
    qDebug()<<"update TB" ;
    medAbstractView* medView = dynamic_cast<medAbstractView*>(view);
    if (medView)
        setContextVisibility(medView->dataTypes());
    else
    {
        qDebug()<<"update on NULL";
        setContextVisibility(QHash<QString, unsigned int> ());
    }
    //DTK_DEFAULT_IMPLEMENTATION;
    //DTK_UNUSED(view);
}

void medToolBox::clear(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medToolBox::setOrientation(Qt::Orientation orientation)
{
    switch(orientation) {
    case Qt::Vertical:
        if (d->layout->direction() == QBoxLayout::TopToBottom) {
            return;
        } else {
            d->body->setOrientation(Qt::Vertical);
        }
        break;
    case Qt::Horizontal:
        if (d->layout->direction() == QBoxLayout::LeftToRight) {
            return;
        } else {
            d->body->setOrientation(Qt::Horizontal);
        }
        break;
   }
}

Qt::Orientation medToolBox::orientation (void) const
{
    if(d->layout->direction() == QBoxLayout::LeftToRight)
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

void medToolBox::switchMinimize()
{
    if (d->isMinimized)
    {
        d->body->show();
        d->isMinimized = false;
    }
    else
    {
        d->body->hide();
        d->isMinimized = true;
    }
}

void medToolBox::setContextVisible(bool contextVisibleFlag)
{
    d->isContextVisible = contextVisibleFlag;
}

bool medToolBox::ContextVisible()
{
 return d->isContextVisible;
}

void medToolBox::show()
{
    qDebug()<<"show in TB";
    if(d->isContextVisible)
        QWidget::show();
}

void medToolBox::setValidDataTypes(const QStringList & dataTypes)
{
    d->validDataTypes = QStringList(dataTypes);

}

const QStringList medToolBox::ValidDataTypes()
{
   return  d->validDataTypes;

}

void medToolBox::addValidDataType(const QString & dataType)
{
    if (!d->validDataTypes.contains(dataType))
    {
        d->validDataTypes.append(dataType);
    }
}

void medToolBox::setContextVisibility(
        const QHash<QString, unsigned int> & viewDataTypes )
{
    qDebug()<< "setContextVisibility";
    if (d->validDataTypes.isEmpty())
    {
        qDebug()<< "no datatypes";
        if (d->validDataTypes.isEmpty())
        d->isContextVisible = true;
        show();
    }
    else
    {
        qDebug()<<"View datatypes"<<viewDataTypes.keys();
        d->isContextVisible = false;
        foreach(QString validDataType, d->validDataTypes)
        {
            qDebug()<<"datatype"<< validDataType ;
            if(viewDataTypes.contains(validDataType))
                qDebug()<<"viewDataTypes: "<< viewDataTypes.value(validDataType);
            if(viewDataTypes.value(validDataType)!=0)
            {
                d->isContextVisible = true;
                break;
            }
        }
    }
    qDebug()<<"visibility" << d->isContextVisible ;
    this->setVisible(d->isContextVisible);
}
