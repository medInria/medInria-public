/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBinaryOperationToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractDbController.h>
#include <medAbstractImageData.h>
#include <medAbstractProcess.h>
#include <medDataManager.h>
#include <medDropSite.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>

class medBinaryOperationToolBoxPrivate
{
public:
    
    dtkSmartPointer <medAbstractProcess> process;
    medDropSite *dropsite;
    dtkSmartPointer<medAbstractData> secondInput;

    QRadioButton *xorButton;
    QRadioButton *andButton;
    QRadioButton *orButton;
    QRadioButton *notButton;
    QPushButton  *clearDropsiteButton;
};

medBinaryOperationToolBox::medBinaryOperationToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new medBinaryOperationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
    
    d->dropsite = new medDropSite(widget);
    d->dropsite->setToolTip(tr("Drop a second input"));
    d->dropsite->setText(tr("Add a second input"));
    d->dropsite->setCanAutomaticallyChangeAppereance(false);

    d->clearDropsiteButton = new QPushButton("Clear", widget);
    d->clearDropsiteButton->setToolTip(tr("Clear previously loaded mask."));

    d->xorButton  = new QRadioButton(tr("XOR"), widget);
    d->xorButton->setToolTip(tr("\"XOR\" keeps pixels which are positive on a volume and not on the other."));
    d->xorButton->setChecked(true);

    d->andButton  = new QRadioButton(tr("AND"), widget);
    d->andButton->setToolTip(tr("\"AND\" keeps pixels which are positive on both volumes."));

    d->orButton  = new QRadioButton(tr("OR"), widget);
    d->orButton->setToolTip(tr("\"OR\" keeps keep positive pixels from both volumes."));

    d->notButton  = new QRadioButton(tr("NOT"), widget);
    d->orButton->setToolTip(tr("\"NOT\" does an inversion of the volume."));

    QVBoxLayout *roiButtonLayout = new QVBoxLayout;
    roiButtonLayout->addWidget(d->dropsite);
    roiButtonLayout->addWidget(d->clearDropsiteButton);
    roiButtonLayout->addWidget(d->xorButton);
    roiButtonLayout->addWidget(d->andButton);
    roiButtonLayout->addWidget(d->orButton);
    roiButtonLayout->addWidget(d->notButton);
    roiButtonLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(widget);
    bundlingLayout->addLayout(roiButtonLayout);

    connect (d->dropsite, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onSecondInputImported(const medDataIndex&)));
    connect (d->dropsite, SIGNAL(clicked()),                          this, SLOT(onDropSiteClicked()));
    connect (d->clearDropsiteButton, SIGNAL(clicked()), this, SLOT(clearDropsite()));
    connect (d->xorButton,     SIGNAL(toggled(bool)), this, SLOT(onXorButtonToggled(bool)));
    connect (d->andButton,     SIGNAL(toggled(bool)), this, SLOT(onAndButtonToggled(bool)));
    connect (d->orButton,      SIGNAL(toggled(bool)), this, SLOT(onOrButtonToggled(bool)));
    connect (d->notButton,     SIGNAL(toggled(bool)), this, SLOT(onNotButtonToggled(bool)));

    this->onXorButtonToggled(true);
    this->addWidget(widget);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    
    this->addWidget(runButton);
    
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

    d->secondInput = 0;
}

medBinaryOperationToolBox::~medBinaryOperationToolBox()
{
    // clear ROIs and related GUI elements
    clearDropsite();

    delete d;    
    d = NULL;
}

bool medBinaryOperationToolBox::registered()
{
    return medToolBoxFactory::instance()->
    registerToolBox<medBinaryOperationToolBox>();
}

dtkPlugin* medBinaryOperationToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "Binary Operation" );
    return plugin;
}

medAbstractData* medBinaryOperationToolBox::processOutput()
{
    if(!d->process)
    {
        return NULL;
    }
    return qobject_cast<medAbstractData*>(d->process->output());
}

void medBinaryOperationToolBox::onXorButtonToggled(bool value)
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkXorOperator");
    d->dropsite->setEnabled(true); //Need two inputs
    d->clearDropsiteButton->setEnabled(true);
}

void medBinaryOperationToolBox::onAndButtonToggled(bool value)
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkAndOperator");
    d->dropsite->setEnabled(true); //Need two inputs
    d->clearDropsiteButton->setEnabled(true);
}

void medBinaryOperationToolBox::onOrButtonToggled(bool value)
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkOrOperator");
    d->dropsite->setEnabled(true); //Need two inputs
    d->clearDropsiteButton->setEnabled(true);
}

void medBinaryOperationToolBox::onNotButtonToggled(bool value)
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkNotOperator");
    clearDropsite();
    d->dropsite->setEnabled(false); //Need one input
    d->clearDropsiteButton->setEnabled(false);
}

void medBinaryOperationToolBox::run()
{
    if (!d->process)
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkXorOperator");
    }

    this->setToolBoxOnWaitStatus();

    d->process->setInput ( this->selectorToolBox()->data(), 0 );
    d->process->setInput ( d->secondInput, 1 );

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->process);
    this->addConnectionsAndStartJob(runProcess);
}

void medBinaryOperationToolBox::onSecondInputImported(const medDataIndex& index)
{
    dtkSmartPointer<medAbstractData> data = medDataManager::instance()->retrieveData(index);
    d->dropsite->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->dropsite->sizeHint()));
    d->secondInput = data;
}

void medBinaryOperationToolBox::clearDropsite(void)
{
    d->dropsite->clear();
    d->dropsite->setText(tr("Add a second input"));
    d->secondInput = NULL;
}

void medBinaryOperationToolBox::onDropSiteClicked()
{
    QString roiFileName = QFileDialog::getOpenFileName(this, tr("Open ROI"), "", tr("Image file (*.*)"));

    if (roiFileName.isEmpty())
    {
        return;
    }
    medDataManager* mdm = medDataManager::instance();
    connect(mdm, SIGNAL(dataAdded(const medDataIndex &)), this, SLOT(onRoiImported(const medDataIndex &)));
    mdm->importPath(roiFileName, true, false);
}