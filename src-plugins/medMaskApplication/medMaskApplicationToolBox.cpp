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
    medDropSite *imageDropSite;
    QDoubleSpinBox* backgroundSpinBox;

    dtkSmartPointer<medAbstractData> data;
    dtkSmartPointer<medAbstractData> mask;
};

medMaskApplicationToolBox::medMaskApplicationToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new medMaskApplicationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
    
    d->maskDropSite = new medDropSite(widget);
    d->maskDropSite->setToolTip(tr("Drop the binary mask"));
    d->maskDropSite->setText(tr("Drop the mask"));
    d->maskDropSite->setCanAutomaticallyChangeAppereance(false);

    d->imageDropSite = new medDropSite(widget);
    d->imageDropSite->setToolTip(tr("Drop the image"));
    d->imageDropSite->setText(tr("Drop the image"));
    d->imageDropSite->setCanAutomaticallyChangeAppereance(true);

    QPushButton *clearMaskButton = new QPushButton("Clear mask", widget);
    clearMaskButton->setToolTip(tr("Clear previously loaded mask."));
    QPushButton *clearImageButton = new QPushButton("Clear Image", widget);
    clearImageButton->setToolTip(tr("Clear previously loaded image."));

    d->backgroundSpinBox = new QDoubleSpinBox;
    d->backgroundSpinBox->setMinimum(-5000);
    d->backgroundSpinBox->setMaximum(5000);
    d->backgroundSpinBox->setValue(0);
    QHBoxLayout* backgroundLayout = new QHBoxLayout;
    backgroundLayout->addWidget(new QLabel("Mask Background Value:"));
    backgroundLayout->addWidget(d->backgroundSpinBox);

    QVBoxLayout *roiButtonLayout = new QVBoxLayout;
    roiButtonLayout->addWidget(d->maskDropSite);
    roiButtonLayout->addWidget (clearMaskButton);
    roiButtonLayout->addWidget(d->imageDropSite);
    roiButtonLayout->addWidget (clearImageButton);
    roiButtonLayout->addLayout(backgroundLayout);
    roiButtonLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(widget);
    bundlingLayout->addLayout(roiButtonLayout);

    connect (d->maskDropSite,   SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(importMask(const medDataIndex&)));
    connect (d->imageDropSite,  SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(importImage(const medDataIndex&)));
    connect (clearMaskButton,   SIGNAL(clicked()),                          this, SLOT(clearMask()));
    connect (clearImageButton,  SIGNAL(clicked()),                          this, SLOT(clearImage()));


    this->setTitle("Mask Application");
    this->addWidget(widget);

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
    if (!d->process)
    {
        return;
    }
    if (!d->maskDropSite->pixmap() || !d->imageDropSite->pixmap())
    {
        return;
    }

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
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

void medMaskApplicationToolBox::importMask(const medDataIndex& index)
{
    d->mask = medDataManager::instance()->retrieveData(index);
    d->maskDropSite->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->maskDropSite->sizeHint()));

    if(!d->process)
    {
        d->process= new medMaskApplication;
    }
    d->process->setInput(d->mask, 0);
}

void medMaskApplicationToolBox::importImage(const medDataIndex& index)
{
    d->data = medDataManager::instance()->retrieveData(index);
    d->imageDropSite->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->imageDropSite->sizeHint()));

    if(!d->process)
    {
        d->process= new medMaskApplication;
    }
    d->process->setInput(d->data, 1);
}

void medMaskApplicationToolBox::clearMask(void)
{
    d->maskDropSite->clear();
    d->maskDropSite->setText(tr("Drop the mask"));
    if (d->process)
    {
        d->process->clearInput(0);
    }
}

void medMaskApplicationToolBox::clearImage(void)
{
    d->imageDropSite->clear();
    d->imageDropSite->setText(tr("Drop the image"));
    if (d->process)
    {
        d->process->clearInput(1);
    }
}

void medMaskApplicationToolBox::clear(void)
{
    // clear ROIs and related GUI elements
    clearMask();

    d->data = 0;
}

void medMaskApplicationToolBox::setMask(const QImage& thumbnail)
{
    d->maskDropSite->setPixmap(QPixmap::fromImage(thumbnail));
}

void medMaskApplicationToolBox::setImage(const QImage& thumbnail)
{
    d->imageDropSite->setPixmap(QPixmap::fromImage(thumbnail));
}
