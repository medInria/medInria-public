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
    QHash<QObject *,QPushButton*> buttons;
    QHash<QObject*,QObject *> buttonsSender;
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

    QProgressBar *bar = new QProgressBar(widget);
    bar->setRange(0, 100);
    d->bars.insert(sender, bar);

    QPushButton *button= new QPushButton(widget);
    button->setText("C");
    connect(button,SIGNAL(clicked()), this,SLOT(sendCancelRequest()));
    d->buttons.insert(sender,button);
    d->buttonsSender.insert(button, sender);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(ilabel);
    layout->addWidget(bar);
    layout->addWidget(button);
    d->widgets.insert(sender, widget);

    d->layout->addWidget(widget);
}

void medProgressionStack::setProgress(int progress)
{
    QObject *object = this->sender();

    if (d->bars.contains(object)) {

        if(!d->bars.value(object)->isHidden())
            d->bars.value(object)->setValue(progress);
    }
}

void medProgressionStack::onSuccess (void)
{
    completeNotification(tr("Successful"));
}

void medProgressionStack::onFailure (void)
{
    completeNotification(tr("Failure"));
}

void medProgressionStack::removeItem(){
    try
    {
        if(!d->itemstoBeRemoved.isEmpty()){
            QObject* object = d->itemstoBeRemoved.dequeue();
            QWidget *widget = d->widgets.value(object);
            delete d->bars.value(object);
            d->bars.remove (object);
            delete d->buttons.value(object);
            d->buttons.remove(object);
            d->layout->removeWidget(widget);
            d->widgets.remove(object);
            delete widget;
            if(d->bars.count() == 0)
                emit(hidden());
        }
    }
    catch (...)
    {
        qWarning() << "exception caught while removing item from progression stack";
    }

}

void medProgressionStack::completeNotification( QString label )
{
    QObject *object = this->sender();

    if (d->bars.contains(object)) {

        QWidget *widget = d->widgets.value(object);

        if(!d->bars.value(object)->isHidden())
        {
            //Completed notification
            QLabel *completeLabel = new QLabel(label,widget);
            widget->layout()->removeWidget(d->bars.value(object));
            d->bars.value(object)->hide();
            widget->layout()->removeWidget(d->buttons.value(object));
            d->buttons.value(object)->hide();
            widget->layout()->addWidget(completeLabel);
            d->itemstoBeRemoved.enqueue(object);
            QTimer::singleShot(3000, this, SLOT(removeItem()));
        }

    }
}

void medProgressionStack::onCancel()
{
    completeNotification(tr("Cancelled"));
}

void medProgressionStack::sendCancelRequest()
{
    QObject* object = this->sender();
    emit cancelRequest(d->buttonsSender.value(object));
}
