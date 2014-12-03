/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobListWidget.h>

#include <QtCore>
#include <QtGui>

#include <medAbstractJob.h>
#include <medJobManager.h>
#include <medMessageController.h>

// /////////////////////////////////////////////////////////////////
// medJobListWidget
// /////////////////////////////////////////////////////////////////

class medJobListWidgetPrivate
{
public:
    QVBoxLayout *layout;
    QList <medAbstractJob *> jobsList;
    QList <QProgressBar *> progressBars;
    QList <QLabel *> jobsLabels;
    QList <QPushButton *> cancelButtons;
    QList <QWidget *> widgets;

    QList <medAbstractJob *> jobsToRemove;
};

medJobListWidget::medJobListWidget(QWidget *parent) : QWidget(parent), d(new medJobListWidgetPrivate)
{
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    connect(medJobManager::instance(), SIGNAL(jobRegistered(medAbstractJob*, QString)),
            this, SLOT(addJob(medAbstractJob *, QString)));

    connect(medJobManager::instance(), SIGNAL(jobRemoved(medAbstractJob *)),
            this, SLOT(removeJob(medAbstractJob *)));
}

medJobListWidget::~medJobListWidget(void)
{
    delete d;
    d = 0;
}

/**
* addJob - Add a new subclass of medAbstractJob to the Stack to create the connection between them
* @param: medAbstractJob * job - instance of medAbstractJob
* @param: QString label - the label shown on the jobToolBox
* if no label was given the job will not be added
*/
void medJobListWidget::addJob(medAbstractJob *job, QString label)
{
    if (d->jobsList.contains(job))
        return;

    d->jobsList.append(job);

    QWidget *widget = new QWidget(this);
    QLabel *jobLabel = new QLabel(label, widget);
    d->jobsLabels.append(jobLabel);
    jobLabel->setStyleSheet("background:none;");
    QProgressBar *bar = new QProgressBar(widget);
    bar->setRange(0, 100);
    QPalette p = palette();
    p.setColor(QPalette::Highlight, Qt::blue);
    bar->setPalette(p);

    d->progressBars.append(bar);

    QPushButton *cancelButton = new QPushButton(widget);
    cancelButton->setText("C");
    connect(cancelButton,SIGNAL(clicked()), this, SLOT(cancelJob()));
    d->cancelButtons.append(cancelButton);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(jobLabel);
    layout->addWidget(bar);
    layout->addWidget(cancelButton);

    d->layout->addWidget(widget);
    d->widgets.append(widget);

    connect(job, SIGNAL(progressed(int)), this, SLOT(setJobProgress(int)));
    connect(job, SIGNAL(success()), this, SLOT(setJobSuccess()));
    connect(job, SIGNAL(failure()), this, SLOT(setJobFailure()));
    connect(job, SIGNAL(disableCancel()), this, SLOT(disableJobCancel()));

    connect(job, SIGNAL(cancelled()), this, SLOT(setJobFailure()));
}

void medJobListWidget::disableJobCancel()
{
    medAbstractJob *job = qobject_cast <medAbstractJob *> (QObject::sender());

     if (!job || !d->jobsList.contains(job))
         return;

     unsigned int indexJob = d->jobsList.indexOf(job);
     d->cancelButtons[indexJob]->setVisible(false);
}

void medJobListWidget::cancelJob()
{
    QPushButton *button = qobject_cast <QPushButton *> (QObject::sender());

    if (!button || !d->cancelButtons.contains(button))
        return;

    unsigned int indexJob = d->cancelButtons.indexOf(button);
    d->cancelButtons[indexJob]->setVisible(false);

    d->jobsList[indexJob]->cancel();
    d->jobsLabels[indexJob]->setText(d->jobsLabels[indexJob]->text() + " cancelled");
    d->progressBars[indexJob]->setVisible(false);
}

void medJobListWidget::setJobProgress(int progress)
{
    medAbstractJob *job = qobject_cast <medAbstractJob *> (QObject::sender());

    if (!job || !d->jobsList.contains(job))
        return;

    unsigned int indexJob = d->jobsList.indexOf(job);
    d->progressBars[indexJob]->setValue(progress);
}

void medJobListWidget::setJobSuccess()
{
    medAbstractJob *job = qobject_cast <medAbstractJob *> (QObject::sender());

    if (!job || !d->jobsList.contains(job))
        return;

    unsigned int indexJob = d->jobsList.indexOf(job);

    if (d->progressBars[indexJob]->isVisible())
    {
        d->progressBars[indexJob]->setValue(100);
        d->progressBars[indexJob]->setStyleSheet("QProgressBar::chunk {background-color: #00FF00;}");
    }
    else
        d->jobsLabels[indexJob]->setText(d->jobsLabels[indexJob]->text() + ": success");

    d->cancelButtons[indexJob]->setVisible(false);
}

void medJobListWidget::setJobFailure()
{
    medAbstractJob *job = qobject_cast <medAbstractJob *> (QObject::sender());

    if (!job || !d->jobsList.contains(job))
        return;

    unsigned int indexJob = d->jobsList.indexOf(job);

    if (d->progressBars[indexJob]->isVisible())
    {
        d->progressBars[indexJob]->setValue(100);
        d->progressBars[indexJob]->setStyleSheet("QProgressBar::chunk {background-color: #FF0000;}");
    }
    else
        d->jobsLabels[indexJob]->setText(d->jobsLabels[indexJob]->text() + ": success");

    d->cancelButtons[indexJob]->setVisible(false);
}

void medJobListWidget::removeJob(medAbstractJob *job)
{
    if (!d->jobsList.contains(job))
        return;

    unsigned int indexJob = d->jobsList.indexOf(job);
    d->cancelButtons[indexJob]->setVisible(false);
    d->jobsToRemove.append(job);

    QTimer::singleShot(2000, this, SLOT(removeJobWidgets()));
}

void medJobListWidget::removeJobWidgets()
{
    foreach(medAbstractJob *job, d->jobsToRemove)
    {
        unsigned int indexJob = d->jobsList.indexOf(job);

        d->cancelButtons.removeAt(indexJob);
        d->jobsLabels.removeAt(indexJob);
        d->jobsList.removeAt(indexJob);
        d->progressBars.removeAt(indexJob);
        d->layout->removeWidget(d->widgets[indexJob]);
        d->widgets.removeAt(indexJob);
    }

    d->jobsToRemove.clear();
}

//void medJobListWidget::cancelJob(QObject* sender)
//{
//    completeNotification(sender, tr("Cancelled"));
//}

//void medJobListWidget::removeItem()
//{
//    try
//    {
//        if(!d->itemstoBeRemoved.isEmpty())
//        {
//            QObject* object = d->itemstoBeRemoved.dequeue();
//            QWidget *widget = d->widgets.value(object);
//            delete d->bars.value(object);
//            d->bars.remove (object);
//            delete d->buttons.value(object);
//            d->buttons.remove(object);
//            d->layout->removeWidget(widget);
//            d->widgets.remove(object);
//            delete widget;
//        }
//    }
//    catch (...)
//    {
//        qWarning() << "exception caught while removing item from progression stack";
//    }

//}

/** send a cancel request to the job that the cancelbutton is connected to **/
//void medJobListWidget::sendCancelRequest()
//{
//    QObject* sender = this->sender();
//    emit cancelRequest(d->buttonsSender.value(sender));
//}

/**
* Modifies the GUI so as there is no way of (attempting) to cancel
* the job anymore, as now it is not cancellable.
* @param QObject* sender
*/
//void medJobListWidget::disableCancel(QObject* sender)
//{
//    if (d->bars.contains(sender) )
//    {
//        d->buttons.value(sender)->hide();
//    }
//}
