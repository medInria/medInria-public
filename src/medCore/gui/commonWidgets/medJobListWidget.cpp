/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobListWidget.h>

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include <medAbstractJob.h>
#include <medJobManager.h>
#include <medListWidget.h>

// /////////////////////////////////////////////////////////////////
// medJobListWidget
// /////////////////////////////////////////////////////////////////

class medJobListWidgetPrivate
{
public:
    QList <medAbstractJob *> jobsList;

    QList <QProgressBar *> progressBars;
    QList <QLabel *> jobsLabels;
    QList <QPushButton *> cancelButtons;
    QHash <QPushButton *, medAbstractJob *> lineRemovalButtons;

    medListWidget *listWidget;

    QList <medAbstractJob *> jobsToRemove;
};

medJobListWidget::medJobListWidget(QWidget *parent) : QWidget(parent), d(new medJobListWidgetPrivate)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel *titleText = new QLabel("Job management", this);
    titleText->setStyleSheet("background: none;");
    layout->addWidget(titleText);

    d->listWidget = new medListWidget(this);
    layout->addWidget(d->listWidget);

    connect(medJobManager::instance(), SIGNAL(jobRegistered(medAbstractJob*, QString)),
            this, SLOT(addJob(medAbstractJob *, QString)));

    this->setFixedWidth(350);
    this->setMinimumHeight(20);
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

    QWidget *widget = new QWidget(d->listWidget);
    QLabel *jobLabel = new QLabel(label, widget);
    d->jobsLabels.append(jobLabel);
    jobLabel->setStyleSheet("background:none;");
    QProgressBar *bar = new QProgressBar(widget);
    bar->setRange(0, 100);

    d->progressBars.append(bar);

    QPushButton *cancelButton = new QPushButton(widget);
    cancelButton->setText("C");
    cancelButton->setStyleSheet("height: 15px;width: 15px;min-width: 15px;");
    connect(cancelButton,SIGNAL(clicked()), this, SLOT(cancelJob()));
    d->cancelButtons.append(cancelButton);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(jobLabel);
    layout->addWidget(bar);
    layout->addWidget(cancelButton);

    QListWidgetItem *item = new QListWidgetItem(d->listWidget);
    item->setSizeHint(widget->sizeHint());
    d->listWidget->addItem(item);
    d->listWidget->setItemWidget(item,widget);

    connect(job, SIGNAL(progressed(int)), this, SLOT(setJobProgress(int)));
    connect(job, SIGNAL(success()), this, SLOT(setJobSuccess()));
    connect(job, SIGNAL(failure()), this, SLOT(setJobFailure()));
    connect(job, SIGNAL(disableCancel()), this, SLOT(disableJobCancel()));
    connect(job, SIGNAL(showError(QString)), this, SLOT(displayJobError(QString)));
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

    QWidget *widget = d->listWidget->itemWidget(d->listWidget->item(indexJob));
    QPushButton *jobRemovalButton = new QPushButton(widget);
    jobRemovalButton->setText("x");
    jobRemovalButton->setStyleSheet("height: 15px;width: 15px;min-width: 15px;");
    d->lineRemovalButtons[jobRemovalButton] = d->jobsList[indexJob];
    connect(jobRemovalButton,SIGNAL(clicked()), this, SLOT(removeJob()));
    widget->layout()->addWidget(jobRemovalButton);
    d->listWidget->item(indexJob)->setSizeHint(widget->sizeHint());
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
        d->jobsLabels[indexJob]->setText(d->jobsLabels[indexJob]->text() + ": failure");

    d->cancelButtons[indexJob]->setVisible(false);

    QWidget *widget = d->listWidget->itemWidget(d->listWidget->item(indexJob));
    QPushButton *jobRemovalButton = new QPushButton(widget);
    jobRemovalButton->setText("x");
    jobRemovalButton->setStyleSheet("height: 15px;width: 15px;min-width: 15px;");
    d->lineRemovalButtons[jobRemovalButton] = d->jobsList[indexJob];
    connect(jobRemovalButton,SIGNAL(clicked()), this, SLOT(removeJob()));
    widget->layout()->addWidget(jobRemovalButton);
    d->listWidget->item(indexJob)->setSizeHint(widget->sizeHint());
}

void medJobListWidget::displayJobError(QString errorMessage)
{
    medAbstractJob *job = qobject_cast <medAbstractJob *> (QObject::sender());

     if (!job || !d->jobsList.contains(job))
         return;

     unsigned int indexJob = d->jobsList.indexOf(job);

     QWidget *widget = d->listWidget->itemWidget(d->listWidget->item(indexJob));
     QScrollArea *errorScroll = new QScrollArea(widget);
     QLabel *errorLabel = new QLabel(errorMessage,errorScroll);
     errorLabel->setStyleSheet("background:none;");
     errorScroll->setMaximumHeight(errorLabel->height());
     errorScroll->setWidget(errorLabel);
     unsigned int layoutIndex = widget->layout()->indexOf(d->progressBars[indexJob]);
     d->progressBars[indexJob]->setVisible(false);

     QHBoxLayout *internalLayout = qobject_cast <QHBoxLayout *> (widget->layout());
     internalLayout->removeWidget(d->progressBars[indexJob]);
     internalLayout->insertWidget(layoutIndex,errorScroll);

     d->listWidget->item(indexJob)->setSizeHint(widget->sizeHint());
}

void medJobListWidget::removeJob()
{
    QPushButton *button = qobject_cast <QPushButton *> (QObject::sender());

    unsigned int indexJob = d->jobsList.indexOf(d->lineRemovalButtons[button]);
    d->cancelButtons[indexJob]->setVisible(false);

    d->cancelButtons.removeAt(indexJob);
    d->jobsLabels.removeAt(indexJob);

    d->jobsList.removeAt(indexJob);
    d->progressBars.removeAt(indexJob);

    d->listWidget->removeItemWidget(d->listWidget->item(indexJob));
    QListWidgetItem *item = d->listWidget->takeItem(indexJob);
    delete item;
}
