/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationToolBox.h"

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medDropSite.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>

class medMaskApplicationToolBoxPrivate
{
public:
    
    dtkSmartPointer <medMaskApplication> process;
    medDropSite* maskDropSite;
    QDoubleSpinBox* backgroundSpinBox;

    dtkSmartPointer<medAbstractData> mask;
};

medMaskApplicationToolBox::medMaskApplicationToolBox(QWidget *parent) :
    medAbstractSelectableToolBox(parent), d(new medMaskApplicationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
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
    backgroundLayout->addWidget(new QLabel("Mask background value"));
    backgroundLayout->addWidget(d->backgroundSpinBox);
    bundlingLayout->addLayout(backgroundLayout);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    this->addWidget(runButton);
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

medMaskApplicationToolBox::~medMaskApplicationToolBox()
{
    delete d;
    d = nullptr;
}

bool medMaskApplicationToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medMaskApplicationToolBox>();
}

dtkPlugin* medMaskApplicationToolBox::plugin()
{
    return medPluginManager::instance().plugin("Mask Application");
}

medAbstractData* medMaskApplicationToolBox::processOutput()
{
    if(!d->process)
    {
        return nullptr;
    }
    return d->process->output();
}

void medMaskApplicationToolBox::run()
{
    if (d->mask && this->selectorToolBox()->data())
    {
        this->setToolBoxOnWaitStatus();

        if (!d->process)
        {
            d->process = new medMaskApplication;
        }
        d->process->setInput(d->mask, 0);
        d->process->setInput(this->selectorToolBox()->data(), 1);
        d->process->setParameter(d->backgroundSpinBox->value());

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->process);
        this->addConnectionsAndStartJob(runProcess);
    }
    else
    {
        displayMessageError("Drop a volume in the view and a mask in the drop area");
    }
}

void medMaskApplicationToolBox::importMask(const medDataIndex& index)
{
    d->mask = medDataManager::instance().retrieveData(index);
    d->maskDropSite->setPixmap(medDataManager::instance().thumbnail(index).scaled(d->maskDropSite->sizeHint()));
}

void medMaskApplicationToolBox::clearMask(void)
{
    d->maskDropSite->clear();
    d->maskDropSite->setText(tr("Drop the mask"));
    if (d->process)
    {
        d->process->clearInput(0);
    }
    d->mask = nullptr;
}
