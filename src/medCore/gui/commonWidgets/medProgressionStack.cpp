/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medProgressionStack.h>

#include <QtCore>
#include <QtGui>

#include <medAbstractJob.h>
#include <medMessageController.h>

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
    QHash<QObject *, QLabel *> completeLabels;
    QHash<QObject *, QPushButton*> buttons;
    QHash<QObject *, QObject *> buttonsSender;
    QHash<QObject *, QWidget *> widgets;
    QQueue<QObject *> itemstoBeRemoved;
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

    if (d->bars.contains (sender)) {
        qWarning () << "progression stack already has a label assigned to process " << sender << ", not assigning a new one";
	return;
    }

    QWidget *widget = new QWidget(this);

    QLabel *ilabel = new QLabel(medChop(label), widget);

    QProgressBar *bar = new QProgressBar(widget);
    bar->setRange(0, 100);
    d->bars.insert(sender, bar);

    QLabel *completeLabel = new QLabel(widget);
    completeLabel->setVisible(false);
    d->completeLabels.insert(sender, completeLabel);

    QPushButton *button= new QPushButton(widget);
    button->setText("C");
    connect(button,SIGNAL(clicked()), this,SLOT(sendCancelRequest()));
    d->buttons.insert(sender,button);
    d->buttonsSender.insert(button, sender);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(ilabel);
    layout->addWidget(bar);
    layout->addWidget(completeLabel);
    layout->addWidget(button);
    d->widgets.insert(sender, widget);

    d->layout->addWidget(widget);
}

void medProgressionStack::setProgress(QObject* sender, int progress)
{
    if (d->bars.contains(sender)) {

        if(!d->bars.value(sender)->isHidden()) {
            d->bars.value(sender)->setValue(progress);
            if (progress==100) {
                QWidget *widget = d->widgets.value(sender);
                d->bars.value(sender)->hide();
                widget->layout()->removeWidget(d->buttons.value(sender));
                d->buttons.value(sender)->hide();
                d->completeLabels.value(sender)->setText(tr("Finishing..."));
                d->completeLabels.value(sender)->setVisible(true);
            }
        }
    }
}

void medProgressionStack::onSuccess (QObject* sender)
{
    completeNotification(sender, tr("Success"));
}

void medProgressionStack::onFailure (QObject* sender)
{
    completeNotification(sender, tr("Failure"));
}

void medProgressionStack::onCancel(QObject* sender)
{
    completeNotification(sender, tr("Cancelled"));
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

/** show a final status and remove the Widget after a defined time **/
void medProgressionStack::completeNotification(QObject* sender, QString label )
{
    // check if the sender is in the list of stored senders
    if (d->bars.contains(sender)) {

        QWidget *widget = d->widgets.value(sender);

        if (!d->widgets.value(sender)->isHidden()) {
            //Completed notification
            d->completeLabels.value(sender)->setText(label);
            widget->layout()->removeWidget(d->bars.value(sender));
            d->bars.value(sender)->hide();
            widget->layout()->removeWidget(d->buttons.value(sender));
            d->buttons.value(sender)->hide();
            d->completeLabels.value(sender)->setVisible(true);
            d->itemstoBeRemoved.enqueue(sender);
            QTimer::singleShot(3000, this, SLOT(removeItem()));
        }
    }
}

/** send a cancel request to the job that the cancelbutton is connected to **/
void medProgressionStack::sendCancelRequest()
{
    QObject* sender = this->sender();
    emit cancelRequest(d->buttonsSender.value(sender));
}

/**
* AddJobItem - Add a new subclass of medAbstractJob to the Stack to create the connection between them
* @param: medAbstractJob * job - instance of medAbstractJob
* @param: QString label - the label shown on the jobToolBox
* if no label was given the job will not be added
*/
void medProgressionStack::addJobItem(medAbstractJob* job, QString label)
{
    if (label.isEmpty())
        return;

    connect(job, SIGNAL(progress(QObject*, int)), this, SLOT(setProgress(QObject*, int)), Qt::QueuedConnection);
    connect(job, SIGNAL(success(QObject*)), this, SLOT(onSuccess(QObject*)), Qt::QueuedConnection);
    connect(job, SIGNAL(failure(QObject*)), this, SLOT(onFailure(QObject*)), Qt::QueuedConnection);

    connect(job, SIGNAL(showError(const QString&, unsigned int)),
        medMessageController::instance(), SLOT(showError(const QString&, unsigned int)), Qt::QueuedConnection);

    connect(job, SIGNAL(cancelled(QObject*)), this,SLOT(onCancel(QObject*)), Qt::QueuedConnection);
    connect(this, SIGNAL(cancelRequest(QObject*)),job, SLOT(onCancel(QObject*)), Qt::QueuedConnection);

    connect(job, SIGNAL(disableCancel(QObject*)), this, SLOT(disableCancel(QObject *)));

    this->setLabel(job, label);
}

/**
 * @brief Sets the progress bar to move without knowing percentage.
 *
 * The developper can use the slot at any time to switch from a continuous
 * unknown progression phase to a step by step progression.
 *
 * @param sender the object which called the function, to identify the bar in the table.
 * @param active if true, the bar will move on its own to show activity.
 * If false, the progression will be set to a fraction of 100.
 */
void medProgressionStack::setActive(QObject *sender, bool active)
{
    if (d->bars.contains(sender) ) {
        active?d->bars.value(sender)->setMaximum(0):d->bars.value(sender)->setMaximum(100);
    }
}

/**
* Modifies the GUI so as there is no way of (attempting) to cancel
* the job anymore, as now it is not cancellable.
* @param QObject* sender
*/
void medProgressionStack::disableCancel(QObject* sender)
{
    if (d->bars.contains(sender) )
    {
        d->buttons.value(sender)->hide();
    }
}
