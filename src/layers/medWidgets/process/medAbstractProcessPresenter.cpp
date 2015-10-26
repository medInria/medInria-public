/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessPresenter.h>

#include <QThreadPool>
#include <QPushButton>

#include <dtkLog>

#include <medAbstractProcess.h>
#include <medJobManager.h>

class medAbstractProcessPresenterPrivate
{
public:
    medAbstractProcess *process;
};


medAbstractProcessPresenter::medAbstractProcessPresenter(medAbstractProcess*parent)
    : QObject(parent), d(new medAbstractProcessPresenterPrivate)
{
    if(parent == NULL)
    {
        dtkWarn() << "Constructing presenter parented to a null process"
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
    connect(runButton, &QPushButton::clicked,
            this, &medAbstractProcessPresenter::_runProcessFromThread);
    connect(d->process, &medAbstractProcess::running,
            runButton, &QPushButton::setDisabled);

    return runButton;
}

void medAbstractProcessPresenter::_runProcessFromThread()
{
    medJobManager::instance()->startJobInThread(d->process);
}
