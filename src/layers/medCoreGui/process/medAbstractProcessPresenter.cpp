/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <medAbstractProcessPresenter.h>
#include <medJobManager.h>

#include <QThreadPool>
#include <QPushButton>

#include <dtkLog>

class medAbstractProcessPresenterPrivate
{
public:
    medAbstractProcess *process;
};

medAbstractProcessPresenter::medAbstractProcessPresenter(medAbstractProcess*parent)
    : QObject(parent), d(new medAbstractProcessPresenterPrivate)
{
    if(parent == nullptr)
    {
        qWarning() << "Constructing presenter parented to a null process"
                  << this;
    }
    d->process = parent;
}

medAbstractProcessPresenter::~medAbstractProcessPresenter()
{

}

QPushButton* medAbstractProcessPresenter::buildRunButton()
{
    QPushButton *runButton = new QPushButton(tr("Run"));
    runButton->setDisabled(d->process->isRunning());
    connect(runButton, &QPushButton::clicked,
            this, &medAbstractProcessPresenter::_runProcessFromThread);
    connect(d->process, &medAbstractProcess::running,
            runButton, &QPushButton::setDisabled,
            Qt::QueuedConnection);

    return runButton;
}

QPushButton* medAbstractProcessPresenter::buildCancelButton()
{
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setEnabled(d->process->isRunning());
    connect(cancelButton, &QPushButton::clicked,
            d->process, &medAbstractProcess::cancel,
            Qt::QueuedConnection);
    connect(d->process, &medAbstractProcess::running,
            cancelButton, &QPushButton::setEnabled,
            Qt::QueuedConnection);

    return cancelButton;
}

void medAbstractProcessPresenter::_runProcessFromThread()
{
    medJobManager::instance()->startJobInThread(d->process);
}
