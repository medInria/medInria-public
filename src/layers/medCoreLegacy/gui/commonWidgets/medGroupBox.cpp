/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medGroupBox.h>


class medGroupBoxPrivate
{
public:
    bool collapsible;
    bool collapsed;
};

medGroupBox::medGroupBox(QWidget *parent) :
    QGroupBox(parent),d(new medGroupBoxPrivate)
{
    d->collapsible = false;
    d->collapsed = false;


    connect(this, SIGNAL(toggled(bool)), this, SLOT(setExpanded(bool)));

}

medGroupBox::~medGroupBox()
{
    delete d;
    d = NULL;
}

bool medGroupBox::isCollapsible()
{
    return d->collapsible;
}



void medGroupBox::setCollapsible(bool collapsible)
{
    d->collapsible = collapsible;
    //change appearance if necessary:
    if (!collapsible)
        setCollapsed(false);
    else if (!isChecked())//hide content
        setCollapsed(true);
}

bool medGroupBox::isCollapsed()
{
    return d->collapsed;
}

void medGroupBox::setCollapsed(bool collapse)
{
    d->collapsed = collapse;
    //only collapse if collapsible,
    //and show whenever it is asked.
    if (d->collapsible || !collapse)
    {
        // show/hide children
        foreach(QObject* child, children())
        {
            if (child->isWidgetType())
                qobject_cast<QWidget*>(child)->setVisible(!collapse);
        }
    }
}

void medGroupBox::setExpanded(bool expand)
{
    setCollapsed(!expand);
}

void medGroupBox::childEvent(QChildEvent *event)
{
    //any added children must be hidden if we are collapsed!
    QObject* child = event->child();
    if (event->added() && child->isWidgetType())
    {
        QWidget* widget = qobject_cast<QWidget*>(child);
        if (d->collapsible && !isChecked())
            widget->hide();
    }
}

