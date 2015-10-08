/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>
#include <%1ToolBox.h>

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medAbstractImageData.h>

#include <medToolBoxFactory.h>
#include <medFilteringSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medPluginManager.h>

class %1ToolBoxPrivate
{
public:
    
    dtkSmartPointer <%1> process;
    medProgressionStack * progression_stack;
};

%1ToolBox::%1ToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new %1ToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
    
    QPushButton *runButton = new QPushButton(tr("Run"), this);
    
    this->setTitle("%2");
    
    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
    
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);
    
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

%1ToolBox::~%1ToolBox()
{
    delete d;
    
    d = NULL;
}

bool %1ToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<%1ToolBox>();
}

dtkPlugin* %1ToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "%1Plugin" );
    return plugin;
}

medAbstractData* %1ToolBox::processOutput()
{
    if(!d->process)
        return NULL;
    
    return d->process->output();
}

void %1ToolBox::update(medAbstractData *data)
{
    // TODO:

}

void %1ToolBox::run()
{
    if(!this->parentToolBox())
        return;
    
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("%1");
    
    if(!this->parentToolBox()->data())
        return;
    
    d->process->setInput(this->parentToolBox()->data());
    
    // Set your parameters here
    
    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->process);
    
    d->progression_stack->addJobItem(runProcess, "Progress:");
    
    d->progression_stack->disableCancel(runProcess);
    
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)),  this, SIGNAL (failure ()));
    
    connect (runProcess, SIGNAL(activate(QObject*,bool)),
             d->progression_stack, SLOT(setActive(QObject*,bool)));
    
    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}
