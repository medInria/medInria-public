/* medProgressionStack.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:39:47 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 20:01:36 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 71
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

// /////////////////////////////////////////////////////////////////
// Helper functions
// /////////////////////////////////////////////////////////////////

QString medChop(const QString& string)
{
    QString result = string;

    if(string.size() > 15) {
        result.resize(12);
        result.append("...");
    }
    
    return result;
}

// /////////////////////////////////////////////////////////////////
// medProgressionStack
// /////////////////////////////////////////////////////////////////

class medProgressionStackPrivate
{
public:
    QVBoxLayout *layout;

    QHash<QObject *, QProgressBar *> bars;
    QHash<QObject *, QWidget *> widgets;
    QQueue<QObject*> itemstoBeRemoved;
};

medProgressionStack::medProgressionStack(QWidget *parent) : QWidget(parent), d(new medProgressionStackPrivate)
{
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(4);
}

medProgressionStack::~medProgressionStack(void)
{
    delete d;

    d = NULL;
}

QSize medProgressionStack::sizeHint(void) const
{
    if(d->bars.count())
        return QSize(d->bars.values().first()->width(), (d->bars.values().first()->height()+4) * d->bars.values().count());
    else
        return QSize();
}

void medProgressionStack::setLabel(QObject *sender, QString label)
{
    if(d->bars.count() == 0)
        emit(shown());

    QWidget *widget = new QWidget(this);

    QLabel *ilabel = new QLabel(medChop(label), widget);
    //ilabel->setFixedWidth(100);
    //ilabel->setAlignment(Qt::AlignRight);

    QProgressBar *bar = new QProgressBar(widget);
    bar->setRange(0, 100);
    d->bars.insert(sender, bar);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(ilabel);
    layout->addWidget(bar);
    d->widgets.insert(sender, widget);

    d->layout->addWidget(widget);
}

void medProgressionStack::setProgress(int progress)
{
    QObject *object = this->sender();

    if (d->bars.contains(object)) {

        d->bars.value(object)->setValue(progress);

        if (progress == 100) {
            QWidget *widget = d->widgets.value(object);


            //Completed notification
            QLabel *completeLabel = new QLabel("Successfull",widget);
            widget->layout()->removeWidget(d->bars.value(object));
            delete d->bars.value(object);
            widget->layout()->addWidget(completeLabel);
            d->bars.remove(object);
            d->itemstoBeRemoved.enqueue(object);
            QTimer::singleShot(3000, this, SLOT(removeItem()));
        }
    }
}

void medProgressionStack::removeItem(){
    if(!d->itemstoBeRemoved.isEmpty()){
        QObject* object = d->itemstoBeRemoved.dequeue();
        QWidget *widget = d->widgets.value(object);
        d->layout->removeWidget(widget);
        d->widgets.remove(object);
        delete widget;
        if(d->bars.count() == 0)
            emit(hidden());
    }
}
