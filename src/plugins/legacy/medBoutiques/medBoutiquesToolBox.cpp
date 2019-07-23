/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoutiques.h>
#include <medBoutiquesToolBox.h>

#include <dtkCoreSupport/dtkAbstractDataFactory.h>
#include <dtkCoreSupport/dtkAbstractData.h>

#include <medAbstractImageData.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractViewFactory.h>
#include <dtkCoreSupport/dtkAbstractView.h>
#include <dtkCoreSupport/dtkAbstractViewInteractor.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medRunnableProcess.h>
#include <medJobManagerL.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>
#include <medProgressionStack.h>

#include <QtGui>

class medBoutiquesToolBoxPrivate
{
public:

    medBoutiquesSearchTools *searchToolsWidget;
    medBoutiquesInvocation *invocationWidget;
    medBoutiquesExecution *executionWidget;

    QLineEdit *variance;
    dtkSmartPointer <dtkAbstractProcess> process;
    medProgressionStack * progression_stack;
};

medBoutiquesToolBox::medBoutiquesToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new medBoutiquesToolBoxPrivate)
{

    d->searchToolsWidget = new medBoutiquesSearchTools(parent);
    d->invocationWidget = new medBoutiquesInvocation(parent, d->searchToolsWidget);
    d->executionWidget = new medBoutiquesExecution(parent, d->searchToolsWidget, d->invocationWidget);

    d->invocationWidget->hide();
    d->executionWidget->hide();

    connect(d->searchToolsWidget, &medBoutiquesSearchTools::toolSelected, d->invocationWidget, &medBoutiquesInvocation::toolSelected);
    connect(d->searchToolsWidget, &medBoutiquesSearchTools::toolSelected, d->executionWidget, &medBoutiquesExecution::toolSelected);
    connect(d->searchToolsWidget, &medBoutiquesSearchTools::toolDeselected, d->invocationWidget, &medBoutiquesInvocation::toolDeselected);
    connect(d->searchToolsWidget, &medBoutiquesSearchTools::toolDeselected, d->executionWidget, &medBoutiquesExecution::toolDeselected);

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d->searchToolsWidget);
    layout->addWidget(d->invocationWidget);
    layout->addWidget(d->executionWidget);
    centralWidget->setLayout(layout);

    // // Parameters:

    // QLabel *varianceLabel = new QLabel("Sigma : ");
    // d->variance = new QLineEdit("1.0");

    // QHBoxLayout *varianceLayout = new QHBoxLayout();
    // varianceLayout->addWidget(varianceLabel);
    // varianceLayout->addWidget(d->variance);

    // // Run button:

    // QPushButton *runButton = new QPushButton(tr("Run"));

    // // Principal layout:

    // QWidget *widget = new QWidget(this);

    // d->progression_stack = new medProgressionStack(widget);

    // QVBoxLayout *layprinc = new QVBoxLayout();
    // layprinc->addLayout(varianceLayout);
    // layprinc->addWidget(runButton);
    // layprinc->addWidget(d->progression_stack);

    // widget->setLayout(layprinc);

    // Main toolbox:
    this->setTitle("Boutiques");
    this->addWidget(centralWidget);

    //enable about plugin. Construtor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "medBoutiquesPlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

    // connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

}

medBoutiquesToolBox::~medBoutiquesToolBox()
{
    delete d;

    d = NULL;
}

bool medBoutiquesToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medBoutiquesToolBox>();
}


medAbstractData* medBoutiquesToolBox::processOutput()
{
    if(!d->process)
        return NULL;

    return static_cast<medAbstractData*>(d->process->output());
}

// void medBoutiquesToolBox::run()
// {
//     if(!this->parentToolBox())
//         return;

//     d->process = dtkAbstractProcessFactory::instance()->create("medBoutiquesGaussianBlur");

//     if(!this->parentToolBox()->data())
//         return;

//     d->process->setInput(this->parentToolBox()->data());

//     d->process->setParameter(d->variance->text().toDouble(),0);

//     medRunnableProcess *runProcess = new medRunnableProcess;
//     runProcess->setProcess (d->process);

//     d->progression_stack->addJobItem(runProcess, tr("Progress:"));

//     connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
//     connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));

//     medJobManagerL::instance()->registerJobItem(runProcess);
//     QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

// }

dtkPlugin* medBoutiquesToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    return pm->plugin("medBoutiquesPlugin");
}

void medBoutiquesToolBox::update(medAbstractData *data)
{

}
