/* medProgressionStack.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:39:47 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jan 19 16:50:15 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medProgressionStack.h"

#include <QtCore>
#include <QtGui>

class medProgressionStackPrivate
{
public:
    QVBoxLayout *layout;

    QHash<QObject *, QProgressBar *> bars;
};

medProgressionStack::medProgressionStack(QWidget *parent) : QWidget(parent), d(new medProgressionStackPrivate)
{
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
}

medProgressionStack::~medProgressionStack(void)
{
    delete d;

    d = NULL;
}

QSize medProgressionStack::sizeHint(void) const
{
    if(d->bars.count())
        return QSize(d->bars.values().first()->width(), d->bars.values().first()->height() * d->bars.values().count());
    else
        return QSize();
}

void medProgressionStack::setProgress(int progress)
{
    QObject *object = this->sender();

    if (d->bars.contains(object)) {

        d->bars.value(object)->setValue(progress);

        if (progress == 100) {
            QProgressBar *bar = d->bars.value(object);
            d->layout->removeWidget(bar);
            d->bars.remove(object);
            delete bar;
            return;
        }

    } else  {

        QProgressBar *bar = new QProgressBar(this);
        bar->setRange(0, 100);
        d->layout->addWidget(bar);
        d->bars.insert(object, bar);
        return;
    }

    this->update();
}
