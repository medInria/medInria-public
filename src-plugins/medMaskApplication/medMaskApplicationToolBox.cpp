/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medAbstractImageData.h>

#include <medToolBoxFactory.h>
#include <medFilteringSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medPluginManager.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medDropSite.h>

class medMaskApplicationToolBoxPrivate
{
public:
    
    dtkSmartPointer <medMaskApplication> process;
    medProgressionStack * progression_stack;
    medDropSite *maskDropSite;
    QDoubleSpinBox* backgroundSpinBox;

    dtkSmartPointer<medAbstractData> mask;
};

medMaskApplicationToolBox::medMaskApplicationToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new medMaskApplicationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    this->setTitle("Mask Application");
    this->addWidget(widget);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(widget);
    bundlingLayout->setAlignment(Qt::AlignCenter);

    d->maskDropSite = new medDropSite(widget);
    d->maskDropSite->setToolTip(tr("Drop the binary mask"));
    d->maskDropSite->setText(tr("Drop the mask"));
    d->maskDropSite->setCanAutomaticallyChangeAppereance(false);
    connect (d->maskDropSite,   SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(importMask(const medDataIndex&)));
    bundlingLayout->addWidget(d->maskDropSite);

    QPushButton *clearMaskButton = new QPushButton("Clear mask", widget);
    clearMaskButton->setToolTip(tr("Clear previously loaded mask."));
    clearMaskButton->setFixedWidth(d->maskDropSite->sizeHint().width());
    connect (clearMaskButton, SIGNAL(clicked()), this, SLOT(clearMask()));
    bundlingLayout->addWidget(clearMaskButton);

    d->backgroundSpinBox = new QDoubleSpinBox;
    d->backgroundSpinBox->setMinimum(-5000);
    d->backgroundSpinBox->setMaximum(5000);
    d->backgroundSpinBox->setValue(0);
    QHBoxLayout* backgroundLayout = new QHBoxLayout;
    backgroundLayout->addWidget(new QLabel("Mask background value:"));
    backgroundLayout->addWidget(d->backgroundSpinBox);
    bundlingLayout->addLayout(backgroundLayout);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    
    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
    
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);
    
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

medMaskApplicationToolBox::~medMaskApplicationToolBox()
{
    delete d;
    
    d = NULL;
}

bool medMaskApplicationToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medMaskApplicationToolBox>();
}

dtkPlugin* medMaskApplicationToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "medMaskApplicationPlugin" );
    return plugin;
}

medAbstractData* medMaskApplicationToolBox::processOutput()
{
    if(!d->process)
    {
        return NULL;
    }
    return d->process->output();
}

void medMaskApplicationToolBox::run()
{
    if (d->mask && this->parentToolBox()->data())
    {
        if(!d->process)
        {
            d->process= new medMaskApplication;
        }
        d->process->setInput(d->mask, 0);
        d->process->setInput(this->parentToolBox()->data(), 1);
        d->process->setParameter(d->backgroundSpinBox->value(), 0);

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
        QThreadPool::globalInstance()->start(runProcess);
    }
}

void medMaskApplicationToolBox::importMask(const medDataIndex& index)
{
    d->mask = medDataManager::instance()->retrieveData(index);
    d->maskDropSite->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->maskDropSite->sizeHint()));
}

void medMaskApplicationToolBox::clearMask(void)
{
    d->maskDropSite->clear();
    d->maskDropSite->setText(tr("Drop the mask"));
    if (d->process)
    {
        d->process->clearInput(0);
    }
    d->mask = 0;
}
