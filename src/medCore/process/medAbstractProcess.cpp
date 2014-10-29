/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>

#include <medAbstractParameter.h>
#include <medToolBox.h>
#include <medTriggerParameter.h>
#include <medAbstractJob.h>
#include <medJobManager.h>

class medAbstractProcessPrivate
{
  public:
    QList<medProcessIOPort*> inputs;
    QList<medProcessIOPort*> outputs;
    QPointer<medToolBox> toolbox;
    QPointer<QWidget> parameterWidget;
    medTriggerParameter* runParameter;
};



medRunnableProcess::medRunnableProcess(medAbstractProcess* process, QString name): medAbstractJob(name)
{
    m_process = process;

    connect(m_process, SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));
    connect(m_process, SIGNAL(success()), this, SIGNAL(success()));
    connect(m_process, SIGNAL(failure()), this, SIGNAL(failure()));
    connect(m_process, SIGNAL(canceled()), this, SIGNAL(cancelled()));
    connect(m_process, SIGNAL(showError(QString, uint)), this, SIGNAL(showError(QString, uint)));
}

void medRunnableProcess::internalRun()
{
    m_process->start();
}

void medRunnableProcess::cancel()
{
    m_process->cancel();
}

medAbstractProcess::medAbstractProcess(medAbstractProcess * parent):dtkAbstractProcess(parent), d(new medAbstractProcessPrivate)
{
    d->toolbox = NULL;
    d->parameterWidget = NULL;
    d->runParameter = new medTriggerParameter("Run", this);
}

medAbstractProcess::~medAbstractProcess()
{
    delete d;
}


QList<medProcessIOPort*> medAbstractProcess::inputs()
{
   return d->inputs;
}

QList<medProcessIOPort*> medAbstractProcess::outputs()
{
    return d->outputs;
}

void medAbstractProcess::appendInput(medProcessIOPort *input)
{
    d->inputs.append(input);
}

void medAbstractProcess::appendOutput(medProcessIOPort *output)
{
    d->outputs.append(output);
}

medAbstractParameter* medAbstractProcess::parameter(QString parameterName)
{
    foreach(medAbstractParameter *param, this->parameters())
    {
        if(param->name() == parameterName)
            return param;
    }

    return NULL;
}

medToolBox* medAbstractProcess::toolbox()
{
    if(d->toolbox.isNull())
    {
        d->toolbox = new medToolBox;
        d->toolbox->addWidget(this->parameterWidget());
        d->toolbox->addWidget(d->runParameter->getPushButton());
    }
    return d->toolbox;
}


QWidget* medAbstractProcess::parameterWidget()
{
    if(d->parameterWidget.isNull())
    {
        d->parameterWidget = new QWidget;
        QFormLayout *layout = new QFormLayout(d->parameterWidget);
        foreach(medAbstractParameter *param, this->parameters())
            layout->addRow(param->getLabel(), param->getWidget());
    }
    return d->parameterWidget;
}

medTriggerParameter* medAbstractProcess::runParameter() const
{
    return d->runParameter;
}


int medAbstractProcess::start()
{
    int ret = this->update();
    if (ret == EXIT_SUCCESS)
        emit success();
    else if (ret == EXIT_FAILURE)
        emit failure();

    return ret;
}
