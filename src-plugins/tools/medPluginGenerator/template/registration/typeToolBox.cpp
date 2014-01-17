/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

#include <medAbstractDataImage.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>

#include <rpiCommonTools.hxx>

class %1ToolBoxPrivate
{
public:
    
    medProgressionStack * progression_stack;
};

%1ToolBox::%1ToolBox(QWidget *parent) : medRegistrationAbstractToolBox(parent), d(new %1ToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
    
    QPushButton *runButton = new QPushButton(tr("Run"), this);
    
    //QFormLayout *layout = new QFormLayout(widget);
    
    this->setTitle("%2");
    //layout->addRow(new QLabel(tr(""),this),d->iterationsBox);
    
    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
    
    this->addWidget(widget);
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
    return medToolBoxFactory::instance()->
    registerToolBox<%1ToolBox>("%1ToolBox",
                               tr("Friendly name"),
                               tr("short tooltip description"),
                               QStringList() << "registration");
}

void %1ToolBox::run()
{
    
    if(!this->parentToolBox())
        return;
    medRegistrationSelectorToolBox * parentTB = this->parentToolBox();
    dtkSmartPointer <dtkAbstractProcess> process;
    
    if (this->parentToolBox()->process() &&
        (parentTB->process()->identifier() == "%1"))
    {
        process = parentTB->process();
        
    }
    else
    {
        process = dtkAbstractProcessFactory::instance()->createSmartPointer("%1");
        parentTB->setProcess(process);
    }
    dtkAbstractData *fixedData = parentTB->fixedData();
    dtkAbstractData *movingData = parentTB->movingData();
    
    
    if (!fixedData || !movingData)
        return;
    
    
    %1 *process_Registration = dynamic_cast<%1 *>(process.data());
    if (!process_Registration)
    {
        qWarning() << "registration process doesn't exist" ;
        return;
    }
    // process_Registration->setMyWonderfullParameter(fronTheGui);
    // process_Registration->setMyWonderfullParameter(fronTheGui);
    
    process->setInput(fixedData,  0);
    process->setInput(movingData, 1);
    
    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);
    
    d->progression_stack->addJobItem(runProcess, tr("Progress:"));
    //If there is no observer to track the progression,
    //make the progress bar spin:
    //d->progression_stack->setActive(runProcess,true);
    
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));
    //First have the moving progress bar,
    //and then display the remaining % when known
    connect (runProcess, SIGNAL(activate(QObject*,bool)),
             d->progression_stack, SLOT(setActive(QObject*,bool)));
    
    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}
